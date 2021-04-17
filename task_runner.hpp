#ifndef CPP_UTILITY_TASK_RUNNER_HPP
#define CPP_UTILITY_TASK_RUNNER_HPP

#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

class task_runner
{
public:

    task_runner() = default;
    task_runner(const task_runner&) = delete;
    task_runner& operator=(const task_runner&) = delete;
    task_runner(task_runner&&) = delete;
    task_runner& operator=(task_runner&&) = delete;

    ~task_runner()
    {
        stop();
    }

    void start(const std::function<void()>& func,
        const std::chrono::duration<int64_t, std::milli>& delay = std::chrono::milliseconds(100))
    {
        m_condition = true;
        m_runner = std::thread{&task_runner::loop, this, func, delay};
    }

    void stop()
    {
        m_condition = false;
        m_cv.notify_one();

        if(m_runner.joinable())
            m_runner.join();
    }

private:

    /// Private member functions

    void loop(std::function<void()> func, std::chrono::duration<int64_t, std::milli> delay)
    {
        while(m_condition)
        {
            func();

            std::unique_lock<std::mutex> lock {m_mutex};
            m_cv.wait_for(lock, delay, [this]() { return !this->m_condition; });
        }
    }

    /// Private member variables

    std::mutex m_mutex;

    std::condition_variable m_cv;

    bool m_condition = false;

    std::thread m_runner;

};

#endif
