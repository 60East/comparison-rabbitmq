#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>
#include <signal.h>

std::chrono::high_resolution_clock::time_point g_start;
void handler(int)
{
  g_start = std::chrono::high_resolution_clock::now();
}
int main(int argc, char **argv)
{
  if(argc != 3)
  {
    std::cerr << "usage: " << argv[0] << " <host> <messages>" << std::endl;
    return -1;
  }
  std::string HOST(argv[1]);
  unsigned long MESSAGES = stoul(std::string(argv[2]));

  auto sub_conn = amqp_new_connection();

  auto sub_socket = amqp_tcp_socket_new(sub_conn);
  int status = amqp_socket_open(sub_socket, HOST.c_str(), 5672);
  if(status) {
    std::cerr << "could not open " << HOST << ":5672" << std::endl;
    return -1;
  }
  amqp_login(sub_conn,"/",0,131072,0,AMQP_SASL_METHOD_PLAIN,"guest","guest");
  amqp_channel_open(sub_conn,1);
  //amqp_confirm_select(sub_conn,1);
  amqp_get_rpc_reply(sub_conn);

  amqp_basic_qos(sub_conn,1,0,100,false);
  amqp_get_rpc_reply(sub_conn);

  ::amqp_exchange_declare(sub_conn,1,amqp_cstring_bytes("test-exchange"),amqp_cstring_bytes("fanout"),0,0,0,0,amqp_empty_table);
  amqp_get_rpc_reply(sub_conn);

  ::amqp_queue_declare(sub_conn,1,amqp_cstring_bytes("test-queue"),0,1,0,1,amqp_empty_table);
  amqp_get_rpc_reply(sub_conn);
  ::amqp_queue_bind(sub_conn,1,amqp_cstring_bytes("test-queue"),amqp_cstring_bytes("test-exchange"),amqp_cstring_bytes("test-queue"),amqp_empty_table);
  amqp_get_rpc_reply(sub_conn);

  amqp_basic_consume(sub_conn,1,amqp_cstring_bytes("test-queue"),amqp_empty_bytes,0,0,0,amqp_empty_table);
  amqp_get_rpc_reply(sub_conn);
  size_t count = 0;
  signal(SIGUSR1, handler);

  while(1)
  {
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;
    amqp_maybe_release_buffers(sub_conn);
    res=amqp_consume_message(sub_conn,&envelope,NULL,0);
    if(AMQP_RESPONSE_NORMAL != res.reply_type) break;
    amqp_destroy_envelope(&envelope);
    if(++count == MESSAGES)
    {
      amqp_basic_ack(sub_conn,1,envelope.delivery_tag,true);
      break;
    }
    if(count % 80 == 0)
    {
      amqp_basic_ack(sub_conn,1,envelope.delivery_tag,true);
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double,std::milli> elapsed = end-g_start;
  std::cout << "Consume " << MESSAGES << " complete in " << elapsed.count() << "ms (" << (MESSAGES*1000/elapsed.count()) << "/s)" << std::endl;
  return 0;
}
