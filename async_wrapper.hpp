#ifndef CPP_UTILITY_ASYNC_WRAPPER_HPP
#define CPP_UTILITY_ASYNC_WRAPPER_HPP

#include <future>
#include <chrono>

template<typename RETURN, typename ... PARAMS>
class async_wrapper
{
public:

    async_wrapper(const std::function<RETURN (PARAMS...)>& callback, PARAMS... params)
    {
        m_fut = std::async(std::launch::async, callback, params...);
    }

    ~async_wrapper()
    {
        m_fut.get();
    }

private:

    std::future<RETURN> m_fut;

};

#endif
