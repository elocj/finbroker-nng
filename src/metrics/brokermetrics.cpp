#include "brokermetrics.h"
#include <iomanip>  // ← ADD THIS LINE

BrokerMetrics::BrokerMetrics() {
    d_start_time = std::chrono::high_resolution_clock::now();
}

// Recording methods
void BrokerMetrics::record_message_received(size_t msg_len, const std::string& topic) {
    d_messages_received.fetch_add(1);
    d_bytes_received.fetch_add(msg_len);
    
    if (!topic.empty()) {
        d_topic_counts[topic].fetch_add(1);
    }
}

void BrokerMetrics::print_latency_chart() const {
    if (d_processing_times.empty()) {
        std::cout << "No latency data available for chart" << std::endl;
        return;
    }
    
    std::cout << "\n=== Processing Latency Over Time ===" << std::endl;
    
    // Show every 10th message for readability
    const int step = std::max(1, (int)d_processing_times.size() / 50);
    
    std::cout << "Message# | Latency(ns) | Chart" << std::endl;
    std::cout << "---------|-------------|" << std::string(40, '-') << std::endl;
    
    for (size_t i = 0; i < d_processing_times.size(); i += step) {
        auto latency = d_processing_times[i];
        
        // Simple bar chart
        int bar_length = std::min(40, (int)(latency.count() / 1000)); // Scale to microseconds
        
        std::cout << std::setw(8) << i 
                  << " | " << std::setw(10) << latency.count() 
                  << " | " << std::string(bar_length, '*') << std::endl;
    }
}

void BrokerMetrics::record_message_sent(size_t msg_len) {
    d_messages_sent.fetch_add(1);
    d_bytes_sent.fetch_add(msg_len);
}

void BrokerMetrics::record_processing_time(std::chrono::nanoseconds duration) {
    d_processing_times.push_back(duration);
}

// Reporting methods
void BrokerMetrics::print_summary() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - d_start_time);
    
    std::cout << "\n=== Broker Performance Summary ===" << std::endl;
    std::cout << "Runtime: " << duration.count() << " seconds" << std::endl;
    std::cout << "Messages received: " << d_messages_received.load() << std::endl;
    std::cout << "Messages sent: " << d_messages_sent.load() << std::endl;
    std::cout << "Bytes received: " << d_bytes_received.load() << " bytes" << std::endl;
    std::cout << "Bytes sent: " << d_bytes_sent.load() << " bytes" << std::endl;
    std::cout << "Throughput: " << (d_messages_received.load() / duration.count()) << " msg/sec" << std::endl;
    
    // Topic breakdown
    if (!d_topic_counts.empty()) {
        std::cout << "\nTopic breakdown:" << std::endl;
        for (const auto& [topic, count] : d_topic_counts) {
            std::cout << "  " << topic << ": " << count.load() << " messages" << std::endl;
        }
    }
    
    // Latency percentiles
    print_latency_stats();

    print_latency_chart();
}
    
void BrokerMetrics::print_latency_stats() const {
    if (d_processing_times.empty()) return;
    
    auto times = d_processing_times;  // Copy for sorting
    std::sort(times.begin(), times.end());
    
    size_t p50_idx = times.size() * 0.5;
    size_t p95_idx = times.size() * 0.95;
    size_t p99_idx = times.size() * 0.99;
    
    std::cout << "\nProcessing latency:" << std::endl;
    std::cout << "  P50: " << times[p50_idx].count() << " ns" << std::endl;
    std::cout << "  P95: " << times[p95_idx].count() << " ns" << std::endl;
    std::cout << "  P99: " << times[p99_idx].count() << " ns" << std::endl;
}