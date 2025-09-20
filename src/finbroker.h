#pragma once

#include <nng/nng.h>
#include <string>
#include <vector>
#include <memory>

class FinBroker {
public:
    FinBroker();
    ~FinBroker();
    
    // Core broker operations
    bool start(const std::string& subscribe_url, const std::string& publish_url);
    void stop();
    void run();  // Main event loop
    
    // Publisher management
    bool add_publisher(const std::string& name);
    void remove_publisher(const std::string& name);
    
    // Subscriber management  
    bool add_subscriber(const std::string& name);
    void remove_subscriber(const std::string& name);
    
    // Message routing
    void route_message(const std::string& publisher, const std::string& message);
    
private:
    // NNG sockets
    nng_socket d_pub_socket;    // For receiving from publishers
    nng_socket d_sub_socket;    // For sending to subscribers
    
    // Connection management
    std::vector<std::string> d_publishers;
    std::vector<std::string> d_subscribers;
    
    // State
    bool d_running;
    
    // Internal methods
    void broadcast_to_subscribers(const std::string& message);
};