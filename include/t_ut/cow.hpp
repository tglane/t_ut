#ifndef CPP_UTILITY_COW_HPP
#define CPP_UTILITY_COW_HPP

#include <variant>

namespace t_ut
{

/// Simple implementation of a Copy-on Write class
template <typename T>
class cow
{
public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;

    cow(const_reference ref_to)
        : m_data{&ref_to}
    {}

    cow(const cow& rhs)
    {
        *this = rhs;
    }

    cow& operator=(const cow& rhs)
    {
        if(rhs.is_borrowed())
            m_data = &rhs.ref();
        else
            m_data = rhs.ref();
    }

    cow(cow&&) = default;
    cow& operator=(cow&&) = default;

    bool is_borrowed() const
    {
        return std::holds_alternative<const_pointer>(m_data);
    }

    bool is_owned() const
    {
        return !is_borrowed();
    }

    const_reference ref() const
    {
        if(is_borrowed())
            return *std::get<const_pointer>(m_data);
        else
            return std::get<value_type>(m_data);
    }

    reference mut_ref()
    {
        if(is_borrowed())
        {
            m_data = *std::get<const_pointer>(m_data);
        }
        return std::get<value_type>(m_data);
    }

    const_reference operator*() const
    {
        return ref();
    }

    reference operator*()
    {
        return mut_ref();
    }

private:
    std::variant<const_pointer, value_type> m_data;
};

} // namespace t_ut

#endif
