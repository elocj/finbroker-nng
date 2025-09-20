#include "finbroker.h"
#include <iostream>
#include <nng/protocol/pubsub0/sub.h>  // Add this line
#include <nng/protocol/pubsub0/pub.h>  // Add this line
#include <algorithm>
#include <thread>

FinBroker::FinBroker()
{
    d_running = false;
}

FinBroker::~FinBroker()
{
}

bool FinBroker::start(const std::string& subscribe_url, const std::string& publish_url)
{
    if (d_running)
    {
        return false;
    }

    d_running = true;

    int rv;
    rv = nng_sub0_open(&d_sub_socket);
    if (rv != 0) std::cerr << "Failed to open NNG subscriber socket: " << nng_strerror(rv) << std::endl;
    rv = nng_pub0_open(&d_pub_socket);
    if (rv != 0) std::cerr << "Failed to open NNG publisher socket: " << nng_strerror(rv) << std::endl;

    // ADD THESE LINES - Subscribe to topics
    rv = nng_socket_set(d_sub_socket, NNG_OPT_SUB_SUBSCRIBE, "NYSE_feed", 9);
    if (rv != 0) std::cerr << "Failed to subscribe to NYSE_feed: " << nng_strerror(rv) << std::endl;
    
    rv = nng_socket_set(d_sub_socket, NNG_OPT_SUB_SUBSCRIBE, "NASDAQ_feed", 11);
    if (rv != 0) std::cerr << "Failed to subscribe to NASDAQ_feed: " << nng_strerror(rv) << std::endl;

    nng_listener sub_ls;
	nng_listener pub_ls;
    rv = nng_listener_create(&sub_ls, d_sub_socket, subscribe_url.c_str());
    if (rv != 0) std::cerr << "Failed to create sub NNG listener: " << nng_strerror(rv) << std::endl;
    rv = nng_listener_create(&pub_ls, d_pub_socket, publish_url.c_str());
    if (rv != 0) std::cerr << "Failed to create pub NNG listener: " << nng_strerror(rv) << std::endl;
    rv = nng_listener_start(sub_ls, 0);
    if (rv != 0) std::cerr << "Failed to start pub NNG listener: " << nng_strerror(rv) << std::endl;
    rv = nng_listener_start(pub_ls, 0);
    if (rv != 0) std::cerr << "Failed to start sub NNG listener: " << nng_strerror(rv) << std::endl;

    return true;
}

void FinBroker::stop()
{
    d_running = false;
}

void FinBroker::run()
{
    while (d_running)
    {
        auto recv_start = std::chrono::high_resolution_clock::now(); 
        // std::cout << "YAP" << std::endl;
        char* msg = nullptr;
        size_t msg_len = 0;

        int rv = nng_recv(d_sub_socket, &msg, &msg_len, NNG_FLAG_ALLOC | NNG_FLAG_NONBLOCK);
        if (rv == 0)
        {
            std::string topic;
            std::string message(msg, msg_len);
            std::string errStr;
            
            if (!extract_topic(message, &topic, &errStr))
            {
                std::cerr << errStr << std::endl;
            }

            d_metrics.record_message_received(msg_len, topic);
            std::cout << "Received: " << msg << std::endl;
            nng_send(d_pub_socket, msg, msg_len, 0);

            d_metrics.record_message_sent(msg_len);

            auto recv_end = std::chrono::high_resolution_clock::now();
            auto processing_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                recv_end - recv_start
            );
            d_metrics.record_processing_time(processing_time);
                
            // Route to all subscribers
            // route_message(publisher, data)

            nng_free(msg, msg_len);
        }
        else if (rv == NNG_EAGAIN)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        else
        {
            // Handle other errors
            std::cerr << "Error receiving message: " << nng_strerror(rv) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void FinBroker::print_stats() {
    d_metrics.print_summary();
}

// Add this helper method
bool FinBroker::extract_topic(const std::string& message, std::string* topic, std::string* errStr) {
    size_t colon_pos = message.find(':');
    if (colon_pos != std::string::npos) {
        *topic = message.substr(0, colon_pos);
        return true;
    }
    *errStr = "ERROR: Cannot find topic name";
    return false;
}

void FinBroker::route_message(const std::string& publisher, const std::string& message)
{

}

bool FinBroker::add_publisher(const std::string& name)
{
    if (std::find(d_publishers.begin(), d_publishers.end(), name) != d_publishers.end())
    {
        std::cerr << "Publisher with name '" << "name" << "' already exists!";
        return false;
    }
    
    d_publishers.push_back(name);
    return true;
}

void FinBroker::remove_publisher(const std::string& name)
{
    if (std::find(d_publishers.begin(), d_publishers.end(), name) != d_publishers.end())
    {
        d_publishers.erase(std::remove(d_publishers.begin(), d_publishers.end(), name), 
                   d_publishers.end());
    }
    else
    {
        std::cerr << "No publisher with name '" << "name" << "' found!";
    }
}

bool FinBroker::add_subscriber(const std::string& name)
{
    if (std::find(d_subscribers.begin(), d_subscribers.end(), name) != d_subscribers.end())
    {
        std::cerr << "Subscriber with name '" << "name" << "' already exists!";
        return false;
    }
    
    d_subscribers.push_back(name);
    return true;
}

void FinBroker::remove_subscriber(const std::string& name)
{
    if (std::find(d_subscribers.begin(), d_subscribers.end(), name) != d_subscribers.end())
    {
        d_subscribers.erase(std::remove(d_subscribers.begin(), d_subscribers.end(), name), 
                    d_subscribers.end());
    }
    else
    {
        std::cerr << "No subscriber with name '" << "name" << "' found!";
    }
}