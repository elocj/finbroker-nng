#pragma once
#include <string>

class FinSubscriber
{
  private:
    nng_socket d_sub_sock;
    bool d_connected = false;
    bool d_running = false;

  public:
    FinSubscriber();
    ~FinSubscriber();

    bool connect(const std::string &address = "tcp://localhost:5556");
    void start_listening();
    void stop();
    void disconnect();
};