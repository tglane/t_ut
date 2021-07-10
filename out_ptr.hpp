#ifndef CPP_UTILITY_OUT_PTR_HPP
#define CPP_UTILITY_OUT_PTR_HPP

namespace utility {

// TODO Currently only working with unique_ptr, so make it work with shared_ptr
template<typename SMART, typename POINTER>
class out_ptr
{
    using smart_ptr_type = SMART;
    using ptr_type = POINTER;

public:

    out_ptr(smart_ptr_type& smart)
        : m_smart {&smart},
          m_ptr {smart.get()}
    {}

    ~out_ptr() noexcept
    {
        if(m_smart)
            m_smart->reset(m_ptr);
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

};

/// Deduction guide for out_ptr type from a smart pointer
template<typename SMART>
out_ptr(SMART& smart) ->
    out_ptr<typename std::remove_reference<decltype(smart)>::type, decltype(smart.get())>;

} // namespace utility

#endif
