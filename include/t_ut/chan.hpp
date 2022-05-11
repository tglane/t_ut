#ifndef CPP_UTILITY_CHAN_HPP
#define CPP_UTILITY_CHAN_HPP

#include <condition_variable>
#include <mutex>
#include <optional>

#include "ringbuffer.hpp"

namespace t_ut
{

template <typename value_type, size_t buffer_size = 1>
class chan
{
public:
    void send(const value_type& data)
    {
        {
            std::lock_guard<std::mutex> lock{m_storage->mutex};
            m_storage->data.push_or_override(data);
        }
        m_storage->cond.notify_one();
    }

    const value_type& peek()
    {
        if(m_storage->data.empty())
        {
            std::unique_lock<std::mutex> lock{m_storage->mutex};
            m_storage->cond.wait(lock, [this]() {
                return !m_storage->data.empty();
            });
        }
        return m_storage->data.peek();
    }

    value_type receive()
    {
        if(m_storage->data.empty())
        {
            std::unique_lock<std::mutex> lock{m_storage->mutex};
            m_storage->cond.wait(lock, [this]() {
                return !m_storage->data.empty();
            });
        }
        return m_storage->data.pop_unchecked();
    }

private:
    template <typename internal_type, size_t internal_size>
    struct buffered_chan_storage
    {
        std::mutex mutex;
        std::condition_variable cond;
        ringbuffer<internal_type, internal_size + 1> data;
    };

    using storage_type = buffered_chan_storage<value_type, buffer_size>;

    std::shared_ptr<storage_type> m_storage = std::make_shared<storage_type>();
};

/// Simple class that mimics the basic behaviour of golangs chan
template <typename value_type>
class chan<value_type, 1>
{
public:

    void send(const value_type& data)
    {
        {
            std::lock_guard<std::mutex> lock{m_storage->mutex};
            m_storage->data = data;
        }
        m_storage->cond.notify_one();
    }

    void send(value_type&& data)
    {
        {
            std::lock_guard<std::mutex> lock{m_storage->mutex};
            m_storage->data = std::move(data);
        }
        m_storage->cond.notify_one();
    }

    const value_type& peek()
    {
        if(!m_storage->data)
        {
            std::unique_lock<std::mutex> lock{m_storage->mutex};
            m_storage->cond.wait(lock, [this]() {
                return m_storage->data != std::nullopt;
            });
        }
        return *(m_storage->data);
    }

    value_type receive()
    {
        if(!m_storage->data)
        {
            std::unique_lock<std::mutex> lock{m_storage->mutex};
            m_storage->cond.wait(lock, [this]() {
                return m_storage->data != std::nullopt;
            });
        }
        return std::move(*(m_storage->data));
    }

private:
    template <typename internal_type>
    struct chan_storage
    {
        std::mutex mutex;
        std::condition_variable cond;
        std::optional<internal_type> data;
    };

    using storage_type = chan_storage<value_type>;

    std::shared_ptr<storage_type> m_storage = std::make_shared<storage_type>();
};

template <typename value_type, size_t buffer_size>
void operator<<(chan<value_type, buffer_size>& ch, const value_type& data)
{
    ch.send(data);
}

template <typename value_type, size_t buffer_size>
void operator>>(chan<value_type, buffer_size>& ch, value_type& data_to)
{
    data_to = ch.receive();
}

} // namespace t_ut

#endif
