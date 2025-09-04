#pragma once

#include <functional>
#include <mutex>
#include <queue>
#include <vector>

namespace util
{
class thread_pool
{
public:
    explicit thread_pool(size_t n) : stop_(false)
    {
        for (size_t i = 0; i < n; i++)
            workers_.emplace_back([this] { worker_loop(); });
    }

    ~thread_pool()
    {
        {
            std::unique_lock lk(m_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& worker_thread : workers_)
            worker_thread.join();
    }

    template <class F> void post(F&& f)
    {
        {
            std::unique_lock lk(m_);
            q_.emplace(std::move(f));
        }
        cv_.notify_one();
    }

private:
    void worker_loop()
    {
        for (;;)
        {
            std::function<void()> job;
            {
                std::unique_lock lk(m_);
                cv_.wait(lk, [this] { return !q_.empty() || stop_; });
                if (stop_ && q_.empty())
                    return;
                job = std::move(q_.front());
                q_.pop();
            }
            job();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> q_;
    std::mutex m_;
    std::condition_variable cv_;
    bool stop_;
};
} // namespace util