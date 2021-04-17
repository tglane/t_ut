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

    size_t start(const std::function<void()>& func,
        const std::chrono::duration<int64_t, std::milli>& delay = std::chrono::milliseconds(100))
    {
        condition_ = true;
        runner_.emplace_back(&task_runner::loop, this, func, delay);
        return runner_.size() - 1;
    }

    void stop()
    {
        condition_ = false;
        cv_.notify_all();

        for(std::thread& task : runner_)
        {
            if(task.joinable())
                task.join();
        }

        runner_.clear();
    }

    size_t tasks() const
    {
        return runner_.size();
    }

    std::thread& task(size_t index)
    {
        return runner_[index];
    }

private:

    /// Private member functions

    void loop(std::function<void()> func, std::chrono::duration<int64_t, std::milli> delay)
    {
        while(condition_)
        {
            func();

            std::unique_lock<std::mutex> lock {mut_};
            cv_.wait_for(lock, delay, [this]() { return !this->condition_; });
        }
    }

    /// Private member variables

    std::mutex mut_;

    std::condition_variable cv_;

    bool condition_ = false;

    std::vector<std::thread> runner_;

};

#endif
