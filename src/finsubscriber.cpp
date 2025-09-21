#include <iostream>
#include <nng/nng.h>
#include <nng/protocol/pubsub0/sub.h>
#include <thread>
#include <chrono>

class FinSubscriber
{
private:
    nng_socket d_sub_sock;
    bool d_connected = false;
    bool d_running = false;

public:
    FinSubscriber() = default;
    
    ~FinSubscriber()
    {
        stop();
        if (d_connected)
        {
            nng_close(d_sub_sock);
        }
    }
    
    bool connect(const std::string& address = "tcp://localhost:5556")
    {
        int rv = nng_sub0_open(&d_sub_sock);
        if (rv != 0)
        {
            std::cerr << "Failed to open subscriber socket: " << nng_strerror(rv) << std::endl;
            return false;
        }
        
        rv = nng_dial(d_sub_sock, address.c_str(), NULL, 0);
        if (rv != 0)
        {
            std::cerr << "Failed to connect to broker: " << nng_strerror(rv) << std::endl;
            nng_close(d_sub_sock);
            return false;
        }
        
        // Subscribe to all messages (empty subscription)
        rv = nng_socket_set(d_sub_sock, NNG_OPT_SUB_SUBSCRIBE, "", 0);
        if (rv != 0)
        {
            std::cerr << "Failed to subscribe: " << nng_strerror(rv) << std::endl;
            nng_close(d_sub_sock);
            return false;
        }
        
        d_connected = true;
        std::cout << "Subscriber connected to " << address << std::endl;
        return true;
    }
    
    void start_listening()
    {
        if (!d_connected)
        {
            std::cerr << "Subscriber not connected" << std::endl;
            return;
        }
        
        d_running = true;
        std::cout << "Subscriber started listening..." << std::endl;
        
        while (d_running)
        {
            char* msg = nullptr;
            size_t msg_len = 0;
            
            int rv = nng_recv(d_sub_sock, &msg, &msg_len, NNG_FLAG_ALLOC | NNG_FLAG_NONBLOCK);
            if (rv == 0)
            {
                std::cout << "Received: " << std::string(msg, msg_len) << std::endl;
                nng_free(msg, msg_len);
            }
            else if (rv == NNG_EAGAIN)
            {
                // No message available, sleep briefly
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            else
            {
                std::cerr << "Failed to receive message: " << nng_strerror(rv) << std::endl;
                break;
            }
        }
    }
    
    void stop()
    {
        d_running = false;
        std::cout << "Subscriber stopped" << std::endl;
    }
    
    void disconnect()
    {
        stop();
        if (d_connected)
        {
            nng_close(d_sub_sock);
            d_connected = false;
            std::cout << "Subscriber disconnected" << std::endl;
        }
    }
};