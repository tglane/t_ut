#ifndef CPP_UTILITY_IN_OUT_PTR_HPP
#define CPP_UTILITY_IN_OUT_PTR_HPP

namespace t_ut {

// TODO Currently only working with unique_ptr, so make it work with shared_ptr
// Problem is, that shared_ptr's release function takes the deleter as a parameter
template <typename SMART, typename POINTER>
class in_out_ptr
{
public:
    using smart_ptr_type = SMART;
    using ptr_type = POINTER;

    in_out_ptr(smart_ptr_type& smart_ptr)
        : m_smart{&smart_ptr},
          m_ptr{smart_ptr.release()}
    {}

    in_out_ptr(const in_out_ptr&) = delete;
    in_out_ptr& operator=(const in_out_ptr&) = delete;

    in_out_ptr(in_out_ptr&& rhs) noexcept
        : m_smart{rhs.m_smart},
          m_ptr{rhs.m_ptr}
    {
        rhs.m_smart = nullptr;
    }

    in_out_ptr& operator=(in_out_ptr&& rhs) noexcept
    {
        m_smart = rhs.m_smart;
        m_ptr = rhs.m_ptr;

        rhs.m_smart = nullptr;
    }

    ~in_out_ptr() noexcept
    {
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

/// Deduction guide for in_out_ptr type from a smart pointer
template <typename SMART>
in_out_ptr(SMART& smart) ->
    in_out_ptr<typename std::remove_reference<decltype(smart)>::type, decltype(smart.get())>;

} // namespace t_ut

#endif
