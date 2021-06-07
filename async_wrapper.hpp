#ifndef CPP_UTILITY_ASYNC_WRAPPER_HPP
#define CPP_UTILITY_ASYNC_WRAPPER_HPP

#include <future>
#include <optional>
#include <chrono>

namespace utility
{

template <typename RETURN, typename ... PARAMS>
class async_wrapper
{
    using return_type = RETURN;

public:

    async_wrapper(const std::function<return_type (PARAMS...)>& callback, PARAMS... params)
    {
        m_fut = std::async(std::launch::async, callback, params...);
    }

    async_wrapper(const async_wrapper&) = delete;
    async_wrapper& operator=(const async_wrapper&) = delete;

    async_wrapper(async_wrapper&&) noexcept = default;
    async_wrapper& operator=(async_wrapper&&) noexcept = default;

    ~async_wrapper()
    {
        if(m_fut.valid())
            m_fut.get();
    }

    std::optional<return_type> get()
    {
        using namespace std::chrono_literals;

        if(m_fut.valid() && m_fut.wait_for(0ms) == std::future_status::ready)
            return m_fut.get();
        else
            return std::nullopt;
    }

private:

    std::future<return_type> m_fut;

};

} // namespace utility

#endif
