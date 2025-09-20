#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    nng_socket pub_sock;
    nng_pub0_open(&pub_sock);
    
    // Connect to broker's publisher port
    nng_dial(pub_sock, "tcp://localhost:5555", NULL, 0);
    
    // Send market data
    std::string message = "NYSE_feed:MSFT,150.25,1000";
    for (int i = 0; i < 5; i++) {
        std::cout << "Sending message " << (i + 1) << "/5: " << message << std::endl;
        nng_send(pub_sock, (void*)message.c_str(), message.length(), 0);
        
        // Wait 10 seconds before sending the next message (except for the last one)
        if (i < 4) {
            std::cout << "Waiting 10 seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
    
    nng_close(pub_sock);
    return 0;
}