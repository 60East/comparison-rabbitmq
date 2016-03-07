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
  if(argc!=3)
  {
    std::cerr << "usage: " << argv[0] << " <uri> <topic>" << std::endl;
    return -1;
  }

  std::string uri(argv[1]);
  std::string topic(argv[2]);

  try
  {
    AMPS::Client client("q_perf_clean");
    client.connect(uri);
    client.logon();
    client.sowDelete(topic,"1=1");
  } catch(std::exception &ex)
  {
    std::cerr << "error: " << ex.what() << std::endl;
    return -1;
  }



}
