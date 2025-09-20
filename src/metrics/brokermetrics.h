#pragma once

#include <atomic>
#include <chrono>
#include <vector>
#include <map>
#include <string>
#include <iostream>

class BrokerMetrics {
private:
    std::atomic<uint64_t> d_messages_received{0};
    std::atomic<uint64_t> d_messages_sent{0};
    std::atomic<uint64_t> d_bytes_received{0};
    std::atomic<uint64_t> d_bytes_sent{0};
    
    std::chrono::high_resolution_clock::time_point d_start_time;
    std::vector<std::chrono::nanoseconds> d_processing_times;
    
    // Optional: Topic-specific metrics
    std::map<std::string, std::atomic<uint64_t>> d_topic_counts;
    
public:
    BrokerMetrics();
    
    // Recording methods
    void record_message_received(size_t msg_len, const std::string& topic = "");
    void record_message_sent(size_t msg_len);
    void record_processing_time(std::chrono::nanoseconds duration);

    void print_latency_chart() const;
    
    // Reporting methods
    void print_summary() const;
    
private:
    void print_latency_stats() const;
};