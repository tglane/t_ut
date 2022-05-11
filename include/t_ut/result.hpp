#ifndef CPP_UTILITY_COW_HPP
#define CPP_UTILITY_COW_HPP

#include <variant>
#include <functional>

namespace t_ut
{

/// Basic implementation of a result type inspired by rusts result
template <typename OK, typename ERR>
class result
{
public:
    using ok_type = OK;
    using err_type = ERR;

    result(const ok_type& ok)
        : m_data{ok}
    {}

    result(const err_type& err)
        : m_data{err}
    {}

    bool is_ok() const
    {
        return std::holds_alternative<ok_type>(m_data);
    }

    bool is_err() const
    {
        return !is_ok();
    }

    template <typename RET>
    RET match(const std::function<RET(const OK&)>& ok_cb, const std::function<RET(const ERR&)>& err_cb) const
    {
        if(is_ok())
            return ok_cb(std::get<ok_type>(m_data));
        else
            return err_cb(std::get<err_type>(m_data));
    }

    template <typename RET>
    RET match(const std::function<RET(OK&)>& ok_cb, const std::function<RET(ERR&)>& err_cb)
    {
        if(is_ok())
            return ok_cb(std::get<ok_type>(m_data));
        else
            return err_cb(std::get<err_type>(m_data));
    }

private:
    std::variant<ok_type, err_type> m_data;
};

} // namespace t_ut

#endif
