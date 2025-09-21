#pragma once
#include <string>
#include <vector>

class FinPublisher
{
  private:
    nng_socket d_pub_sock;
    bool d_connected = false;

  public:
    FinPublisher();
    ~FinPublisher();

    bool connect(const std::string &address = "tcp://localhost:5555");
    bool send_message(const std::string &message);
    void send_continuous(const std::string &message, int delay_ms = 1000);
    void disconnect();
};