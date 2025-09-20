#include <nng/nng.h>
#include "finbroker.h"
#include <thread>
#include <chrono>

int main() {
    FinBroker broker;
    
    // Start broker on two ports
    broker.start("tcp://localhost:5555", "tcp://localhost:5556");
    
    // Add some publishers and subscribers
    broker.add_publisher("NYSE_feed");
    broker.add_publisher("NASDAQ_feed");
    broker.add_subscriber("trading_algo_1");
    broker.add_subscriber("trading_algo_2");
    
    // Run the broker (blocks forever)
    // Run the broker in a separate thread
    std::thread broker_thread([&broker]() {
        broker.run();
    });
    
    // Let it run for 30 seconds, then stop and print stats
    std::this_thread::sleep_for(std::chrono::seconds(15));
    
    broker.stop();
    broker_thread.join();
    
    // Print performance metrics
    broker.print_stats();  // ← Print the metrics
    // broker.run();
    
    return 0;
}
