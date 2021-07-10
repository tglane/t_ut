#ifndef CPP_UTILITY_OUT_PTR_HPP
#define CPP_UTILITY_OUT_PTR_HPP

#include <utility>
#include <type_traits>

namespace utility {

// TODO Currently only working with unique_ptr, so make it work with shared_ptr
// Problem is, that shared_ptr's release function takes the deleter as a parameter


// f(std::get<Indices>(::std::forward<ArgTuple>(args))...)

template<typename SMART, typename POINTER, typename ... DELETER>
class out_ptr
{
    using smart_ptr_type = SMART;
    using ptr_type = POINTER;

public:

    out_ptr(smart_ptr_type& smart, DELETER ... deleter)
        : m_smart {&smart},
          m_ptr {smart.get()},
          m_deleter {std::move(deleter)...}
    {}

    out_ptr(const out_ptr&) = delete;
    out_ptr& operator=(const out_ptr&) = delete;

    out_ptr(out_ptr&& rhs) noexcept
        : m_smart {rhs.m_smart},
          m_ptr {rhs.m_ptr},
          m_deleter {std::move(rhs.m_deleter)}
    {
        rhs.m_smart = nullptr;
    }

    out_ptr& operator=(out_ptr&& rhs) noexcept
    {
        m_smart = rhs.m_smart;
        m_ptr = rhs.m_ptr;
        m_deleter = std::move(rhs.m_deleter);

        rhs.m_smart = nullptr;
    }

    ~out_ptr() noexcept
    {
        if(m_smart)
        {
            if constexpr(sizeof...(DELETER) != 0)
                    m_smart->reset(m_ptr, std::get<0>(m_deleter));
            else
                m_smart->reset(m_ptr);
}
    }

    operator ptr_type*() noexcept
    {
        return &m_ptr;
    }

    operator void**() noexcept
    {
        return reinterpret_cast<void**>(&m_ptr);
    }

private:

    smart_ptr_type* m_smart;
    ptr_type m_ptr;

    std::tuple<DELETER...> m_deleter;

};

/// Deduction guide for out_ptr type from a smart pointer
template<typename SMART>
out_ptr(SMART& smart) ->
    out_ptr<typename std::remove_reference<decltype(smart)>::type, decltype(smart.get())>;

/// Deduction guide for out_ptr type from a shared_ptr with a custom deleter
template<typename SMART, typename ... DELETER>
out_ptr(SMART& smart, DELETER&& ... deleter) ->
    out_ptr<typename std::remove_reference<decltype(smart)>::type, decltype(smart.get()), DELETER ...>;

} // namespace utility

#endif
