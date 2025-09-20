#include <nng/nng.h>
#include "finbroker.h"

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
    broker.run();
    
    return 0;
}
