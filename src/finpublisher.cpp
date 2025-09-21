#include <chrono>
#include <iostream>
#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <thread>

class FinPublisher
{
private:
    nng_socket d_pub_sock;
    bool d_connected = false;

public:
    FinPublisher() = default;
    
    ~FinPublisher()
    {
        if (d_connected)
        {
            nng_close(d_pub_sock);
        }
    }
    
    bool connect(const std::string& address = "tcp://localhost:5555")
    {
        int rv = nng_pub0_open(&d_pub_sock);
        if (rv != 0)
        {
            std::cerr << "Failed to open publisher socket: " << nng_strerror(rv) << std::endl;
            return false;
        }
        
        rv = nng_dial(d_pub_sock, address.c_str(), NULL, 0);
        if (rv != 0)
        {
            std::cerr << "Failed to connect to broker: " << nng_strerror(rv) << std::endl;
            nng_close(d_pub_sock);
            return false;
        }
        
        d_connected = true;
        std::cout << "Publisher connected to " << address << std::endl;
        return true;
    }
    
    bool send_message(const std::string& message)
    {
        if (!d_connected)
        {
            std::cerr << "Publisher not connected" << std::endl;
            return false;
        }
        
        int rv = nng_send(d_pub_sock, (void*)message.c_str(), message.length(), 0);
        if (rv != 0)
        {
            std::cerr << "Failed to send message: " << nng_strerror(rv) << std::endl;
            return false;
        }
        
        return true;
    }
    
    void send_continuous(const std::string& message, int delay_ms = 1000)
    {
        while (d_connected)
        {
            if (!send_message(message))
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        }
    }
    
    void disconnect()
    {
        if (d_connected)
        {
            nng_close(d_pub_sock);
            d_connected = false;
            std::cout << "Publisher disconnected" << std::endl;
        }
    }
};