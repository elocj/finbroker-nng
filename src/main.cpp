#include "finbroker.h"
#include "finpublisher.h"
#include "finsubscriber.h"
#include <chrono>
#include <iostream>
#include <thread>

void run_broker()
{
    FinBroker broker;

    broker.start("tcp://localhost:5555", "tcp://localhost:5556");

    broker.add_publisher("NYSE_feed");
    broker.add_publisher("NASDAQ_feed");
    broker.add_subscriber("trading_algo_1");
    broker.add_subscriber("trading_algo_2");

    // `run()` blocks forever so put in thread
    std::thread run_thread([&broker]() { broker.run(); });

    std::this_thread::sleep_for(std::chrono::seconds(15));

    broker.stop();
    run_thread.join();

    broker.print_stats();
}

void run_publishers()
{
    FinPublisher publisher;

    if (!publisher.connect())
    {
        std::cerr << "Failed to connect publisher" << std::endl;
        return;
    }

    std::vector<std::string> messages = {"NYSE_feed:MSFT,150.25,1000", "NYSE_feed:AAPL,175.50,500",
                                         "NASDAQ_feed:GOOGL,2800.75,250",
                                         "NYSE_feed:TSLA,800.00,750"};

    // Currently sending a fixed number of messages
    for (int i = 0; i < 5; ++i)
    {
        for (const auto &message : messages)
        {
            if (!publisher.send_message(message))
            {
                std::cerr << "Failed to send message" << std::endl;
                return;
            }
            std::cout << "Publisher sent: " << message << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    publisher.disconnect();
}

void run_subscribers()
{
    FinSubscriber subscriber;

    if (!subscriber.connect())
    {
        std::cerr << "Failed to connect subscriber" << std::endl;
        return;
    }

    std::thread run_thread([&subscriber]() { subscriber.start_listening(); });

    std::this_thread::sleep_for(std::chrono::seconds(15));

    subscriber.stop();
    run_thread.join();
    subscriber.disconnect();
}

int main()
{
    std::thread broker_thread(run_broker);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::thread subscriber_thread(run_subscribers);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::thread publisher_thread(run_publishers);

    publisher_thread.join();
    subscriber_thread.join();
    broker_thread.join();

    return 0;
}