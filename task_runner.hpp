#ifndef CPP_UTILITY_TASK_RUNNER_HPP
#define CPP_UTILITY_TASK_RUNNER_HPP

#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std::literals::chrono_literals;

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

    void start(const std::function<void()>& func)
    {
        condition_ = true;
        // runner_ = std::thread(&server::loop, this);
        runner_ = std::thread(&task_runner::loop, this, func);
    }

    void stop()
    {
        condition_ = false;
        cv_.notify_all();

        if(runner_.joinable())
            runner_.join();
    }

private:

    void loop(std::function<void()> func)
    {
        while(condition_)
        {
            func();

            std::unique_lock<std::mutex> lock {mut_};
            cv_.wait_for(lock, 100ms, [this]() { return !this->condition_; });
        }
    }

    std::mutex mut_;
    std::condition_variable cv_;

    bool condition_ = true;

    std::thread runner_;

};

#endif
