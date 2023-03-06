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
    using Core = Adapter<Type, Container, Args...>;

    struct base_inner : public Core
    {
        static Container& underlying(Core& core)
        {
            auto underlying_container = &base_inner::c;
            return core.*underlying_container;
        }
    };

    return base_inner::underlying(adapter);
}

} // namespace meta

} // namespace siraf

#endif // SIRAF_SUPPORT_DETAIL_META_HPP
