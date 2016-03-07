#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>
#include <signal.h>


volatile bool g_running = false;

void handler(int)
{
  g_running = true;
}
int main(int argc, char**argv)
{
  if(argc != 4)
  {
    std::cerr<<"usage: " << argv[0] << " <host> <messages> <size>" << std::endl;
    return -1;
  }
  std::string HOST(argv[1]);
  unsigned long MESSAGES = stoul(std::string(argv[2]));
  unsigned long SIZE = stoul(std::string(argv[3]));

  auto pub_conn = amqp_new_connection();

  auto pub_socket = amqp_tcp_socket_new(pub_conn);
  int status = amqp_socket_open(pub_socket, HOST.c_str(), 5672);
  if(status) {
    std::cerr << "could not open " << HOST << ":5672" << std::endl;
    return -1;
  }
  char* data = new char[SIZE];
  memset(data,'a',SIZE);
  amqp_bytes_t message_bytes;
  message_bytes.len = SIZE;
  message_bytes.bytes = data;

  amqp_rpc_reply_t response = amqp_login(pub_conn,"/",0,131072,0,AMQP_SASL_METHOD_PLAIN,"guest","guest");
  if(response.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION)
  {
    std::cerr << "SERVER EXCEPTION" << std::endl;
    return -1;
  }
  else if (response.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
  {
    std::cerr << "CLIENT EXCEPTION" << std::endl;
  }

  amqp_channel_open(pub_conn,1);
  //amqp_confirm_select(pub_conn,1);
  amqp_get_rpc_reply(pub_conn);


  signal(SIGUSR1, handler);
  while(!g_running)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  auto start = std::chrono::high_resolution_clock::now();
  for(int i = 0; i<MESSAGES; ++i)
  {
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("text/plain");
    props.delivery_mode = 2; /* persistent delivery mode */
    amqp_basic_publish(pub_conn,1,amqp_cstring_bytes("test-exchange"),
      amqp_cstring_bytes("test-queue"),0,0,&props,message_bytes);
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double,std::milli> elapsed = end-start;
  std::cout << "\nPublish " << MESSAGES << " complete in " << elapsed.count() << "ms (" << (MESSAGES*1000/elapsed.count()) << "/s)" << std::endl;
  amqp_channel_close(pub_conn,1,AMQP_REPLY_SUCCESS);
  amqp_connection_close(pub_conn,AMQP_REPLY_SUCCESS);
}
