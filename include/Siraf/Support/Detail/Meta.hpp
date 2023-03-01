#ifndef SIRAF_SUPPORT_DETAIL_META_HPP
#define SIRAF_SUPPORT_DETAIL_META_HPP

namespace siraf
{

namespace meta
{

template <template <typename...> class Adapter,
          typename Type, class Container, typename... Args>
Container& underlying(Adapter<Type, Container, Args...>& adapter)
{
    using Base = Adapter<Type, Container, Args...>;

    struct base_inner : public Base
    {
        static Container& underlying(Base& base)
        {
            auto underlying_container = &base_inner::c;
            return base.*underlying_container;
        }
    };

    return base_inner::underlying(adapter);
}

} // namespace meta

} // namespace siraf

#endif // SIRAF_SUPPORT_DETAIL_META_HPP
