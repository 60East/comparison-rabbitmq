#include <thread>
#include <iostream>
#include <chrono>
#include <ampsplusplus.hpp>
#include <vector>
#include <atomic>
#include <signal.h>


std::chrono::high_resolution_clock::time_point s_start;
volatile bool running = false;
volatile size_t receivedCount = 0;
unsigned long MESSAGES;
uint64_t *times;

void handler(int signal_)
{
  running = true;
  s_start = std::chrono::high_resolution_clock::now();
}

void usr2Handler(int signal_)
{
  std::cerr << "pid " << getpid() << " received " << receivedCount << std::endl;
}

void messageHandler(const AMPS::Message& message, void* vp)
{
  const char* data = NULL;
  size_t len = 0;
  uint64_t now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
  message.getRawData(&data,&len);
  uint64_t origin = *(uint64_t*)data;
  times[receivedCount] = now-origin;

  if(receivedCount < MESSAGES)
  {
    message.ack();
    ++receivedCount;
  }
}
int main(int argc, char **argv)
{
  if(argc!=4)
  {
    std::cerr << "usage: " << argv[0] << " <uri> <topic> <message_count>" << std::endl;
    return -1;
  }

  std::string uri(argv[1]);
  std::string topic(argv[2]);
  MESSAGES = stoul(std::string(argv[3]));
  times = new uint64_t[MESSAGES];
  memset(times,0,MESSAGES*sizeof(uint64_t));

  try
  {
    std::stringstream str;
    str<< "q_subscriber-" << getpid() ;
    AMPS::Client client(str.str());
    client.connect(uri);
    client.logon();
    client.setAckBatchSize(80);
    signal(SIGUSR1,&handler);
    signal(SIGUSR2,&usr2Handler);

    client.executeAsync(AMPS::Command("subscribe").setTopic(topic).setOptions("max_backlog=100"),
        AMPS::MessageHandler(messageHandler,NULL));
    int retryCount = 0;
    while(!running)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    while(receivedCount < MESSAGES)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto end = std::chrono::high_resolution_clock::now();
    client.flushAcks();
    client.disconnect();
    std::chrono::duration<double,std::milli> elapsed = end-s_start;
    std::cout << "Consumed " << MESSAGES << " in " << elapsed.count() << "ms (" << (MESSAGES*1000/elapsed.count()) << "/s)" << std::endl;
    char fname_buf[256];
    sprintf(fname_buf,"consumer-%d.out", getpid());
    FILE* fp = fopen(fname_buf,"w");
    for(int i = 0; i < MESSAGES; ++i)
    {
      fprintf(fp,"%lu\n",times[i]);
    }
    fclose(fp);

  } catch(std::exception &ex)
  {
    std::cerr << "error: " << ex.what() << std::endl;
    return -1;
  }
}
