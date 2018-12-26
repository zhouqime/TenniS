//
// Created by kier on 2018/12/26.
//

#include <utils/mutex.h>
#include <runtime/inside/thread_pool.h>

#include <sstream>
#include <iostream>
#include <chrono>
#include <utils/assert.h>

int mul(int *base, int times, ts::rwmutex *mutex, bool lock = true) {
    int sum = 0;
    for (int i = 0; i < times; ++i) {
        std::shared_ptr<ts::unique_read_lock<ts::rwmutex>> _lock;
        if (lock) _lock.reset(new ts::unique_read_lock<ts::rwmutex>(*mutex));
        sum += *base;
    }
    return sum;
}

class time_log {
public:
    using self = time_log;

    using microseconds = std::chrono::microseconds;
    using system_clock = std::chrono::system_clock;
    using time_point = decltype(system_clock::now());

    explicit time_log(ts::LogLevel level, const std::string &header = "") :
        m_duration(0) {
        m_level = level;
        m_header = header;
        m_start = system_clock::now();
    }

    ~time_log() {
        m_end = system_clock::now();
        m_duration = m_end - m_start;

        std::ostringstream oss;
        ts::LogStream(m_level) << m_header << m_duration.count() / 1000.0 << "ms";
    }

    time_log(const self &) = delete;
    self &operator=(const self &) = delete;

private:
    ts::LogLevel m_level;
    std::string m_header;
    microseconds m_duration;
    time_point m_start;
    time_point m_end;
};

int main() {
    int threads = 1;
    ts::ThreadPool pool(threads);

    int base = 1;
    int times = 10000;
    ts::rwmutex mutex;

    TS_LOG_INFO << "Test " << threads * times << " read lock in " << threads << " threads.";

    {
        time_log _log(ts::LOG_INFO, "Spent ");
        for (int i = 0; i < threads; ++i) {
            pool.run([&](int id) {
                auto sum = mul(&base, times, &mutex, true);
                std::ostringstream oss;
                oss << "Thread-" << id << " got " << sum << std::endl;
                std::cout << oss.str();
            });
        }


        pool.join();
    }


    return 0;
}


