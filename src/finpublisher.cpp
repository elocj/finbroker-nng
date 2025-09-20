#include <chrono>
#include <iostream>
#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <thread>

int main()
{
    nng_socket pub_sock;
    nng_pub0_open(&pub_sock);

    // Connect to broker's publisher port
    nng_dial(pub_sock, "tcp://localhost:5555", NULL, 0);

    // Send market data
    std::string message = "NYSE_feed:MSFT,150.25,1000";
    while (true) {
        nng_send(pub_sock, (void *)message.c_str(), message.length(), 0);
    }

    std::cout << "All messages sent, waiting 2 seconds before closing..."
              << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    nng_close(pub_sock);
    return 0;
}