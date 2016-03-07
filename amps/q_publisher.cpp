#include <thread>
#include <iostream>
#include <chrono>
#include <ampsplusplus.hpp>
#include <signal.h>

volatile bool s_running =0;

void handler(int signal_)
{
  s_running = 1;
}
int main(int argc, char **argv)
{
  if(argc!=5)
  {
    std::cerr << "usage: " << argv[0] << " <uri> <topic> <message_count> <message_size>" << std::endl;
    return -1;
  }

  std::string uri(argv[1]);
  std::string topic(argv[2]);
  unsigned long MESSAGES = stoul(std::string(argv[3]));
  unsigned long SIZE = stoul(std::string(argv[4]));

  try
  {
    std::stringstream str;
    str << "q_publisher-" << getpid();
    AMPS::Client client(str.str());
    client.connect(uri);
    client.logon();
    volatile size_t receivedCount = 0;
    char *data = new char[SIZE];
    AMPS::Message publishMessage;
    publishMessage.assignTopic(topic).setCommand("publish").assignData(data,SIZE);
    signal(SIGUSR1, &handler);

    while(!s_running)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < MESSAGES; ++i)
    {
      *((uint64_t*)data) = (uint64_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
      client.send(publishMessage);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> elapsed = end-start;
    std::cout << "Published " << MESSAGES << " in " << elapsed.count() << "ms (" << (MESSAGES*1000/elapsed.count()) << "/s)" << std::endl;

  } catch(std::exception &ex)
  {
    std::cerr << "error: " << ex.what() << std::endl;
    return -1;
  }



}
