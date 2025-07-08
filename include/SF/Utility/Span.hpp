#ifndef SF_UTILITY_SPAN_HPP
#define SF_UTILITY_SPAN_HPP

#include <cstddef> // size_t
#include <tuple> // tuple

#include <SF/Core/ArchiveBase.hpp>

#include <SF/Utility/Alias.hpp>

#include <SF/ApplyFunctor.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace utility
{

template <typename ValueType, std::size_t DimensionNumberValue>
class span_t;

template <typename ValueType, std::size_t DimensionNumberValue>
class span_base_t
{
public:
    using size_type = std::size_t;
    using value_type = span_t<ValueType, DimensionNumberValue - 1>;
    using pointer = typename meta::pointer<ValueType, DimensionNumberValue>::type;

protected:
    using dimension_size_type = size_type[DimensionNumberValue];

protected:
    alias_t<pointer> xxdata;
    dimension_size_type xxdimension_size;

protected:
    span_base_t(pointer& data) noexcept : xxdata(data), xxdimension_size() {}
    template <typename SizeType, typename... SizeTypes>
    span_base_t(pointer& data, SizeType dimension, SizeTypes... dimensions) noexcept
        : xxdata(data), xxdimension_size{dimension, dimensions...} {}

public:
    void init(pointer data) noexcept { xxdata.get() = data; }
    void data(alias_t<pointer> data) noexcept { xxdata = data; }

    pointer& data() noexcept { return xxdata; }
    size_type size() const noexcept { return xxdimension_size[0]; }
    dimension_size_type& dim() noexcept { return xxdimension_size; }
};

template <typename ValueType, std::size_t DimensionNumberValue>
class span_t : public span_base_t<ValueType, DimensionNumberValue>
{
protected:
    using BaseSpanType = span_base_t<ValueType, DimensionNumberValue>;

public:
    using typename BaseSpanType::size_type;
    using typename BaseSpanType::pointer;

public:
    using value_type = span_t<ValueType, DimensionNumberValue - 1>;
    using dereference_type = typename meta::pointer<ValueType, DimensionNumberValue - 1>::type;

protected:
    using typename BaseSpanType::dimension_size_type;

private:
    mutable value_type xxchild_scope;

public:
    template <typename SizeType, typename... SizeTypes,
              SF_REQUIRES(std::negation<std::is_array<SizeType>>::value)>
    span_t(pointer& data, SizeType dimension, SizeTypes... dimensions) noexcept
        : BaseSpanType(data, dimension, dimensions...), xxchild_scope(data[0], dimensions...) {}

    void size(size_type value) noexcept { this->xxdimension_size[0] = value; }

    value_type& operator[] (size_type i) noexcept
    {
        xxchild_scope.data(this->xxdata[i]);
        return xxchild_scope;
    }

public:
    using BaseSpanType::size; // prevent Core function hiding
};

template <typename ValueType>
class span_t<ValueType, 1> : public span_base_t<ValueType, 1>
{
protected:
    using BaseSpanType = span_base_t<ValueType, 1>;

public:
    using typename BaseSpanType::size_type;
    using typename BaseSpanType::pointer;

public:
    using value_type = ValueType;
    using dereference_type = ValueType;

protected:
    using typename BaseSpanType::dimension_size_type;

public:
    span_t(pointer& data, size_type size) noexcept : BaseSpanType(data, size) {}

    dereference_type& operator[] (size_type i) noexcept { return this->xxdata[i]; }
};

} // namespace utility

namespace meta
{

template <typename> struct is_span : std::false_type {};
template <typename ValueType, std::size_t DimensionNumberValue>
struct is_span<utility::span_t<ValueType, DimensionNumberValue>> : std::true_type {};

template <typename PointerType, typename SizeType, typename... SizeTypes>
struct is_span_set
    : std::conjunction<std::integral_constant<bool, meta::pointer_count<PointerType>::value >= sizeof...(SizeTypes) + 1>,
                       std::conjunction<std::is_arithmetic<SizeType>,
                                        std::is_arithmetic<SizeTypes>...>> {};

} // namespace meta

namespace utility
{

template <typename PointerType, typename SizeType, typename... SizeTypes,
          std::size_t DimensionNumberValue = sizeof...(SizeTypes) + 1,
          typename Type = typename meta::remove_pointer<PointerType, DimensionNumberValue>::type,
          SF_REQUIRES(meta::is_span_set<PointerType, SizeType, SizeTypes...>::value)>
span_t<Type, DimensionNumberValue> make_span(PointerType& data, SizeType dimension, SizeTypes... dimensions)
{
    using size_type = typename span_t<Type, DimensionNumberValue>::size_type;
    return { data, static_cast<size_type>(dimension), static_cast<size_type>(dimensions)... };
}

} // namespace utility

namespace detail
{

template <class ArchiveType, typename SpanType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       std::negation<meta::is_span<SpanType>>>::value)>
void span_impl(ArchiveType& archive, SpanType& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class ArchiveType, typename SpanType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       meta::is_span<SpanType>>::value)>
void span_impl(ArchiveType& archive, SpanType& array)
{
    using size_type = typename SpanType::size_type;

    for (size_type i = 0; i < array.size(); ++i)
        span_impl(archive, array[i]);
}

template <class ArchiveType, typename SpanType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       meta::is_span<SpanType>>::value)>
void span_impl(ArchiveType& archive, SpanType& array)
{
    using size_type = typename SpanType::size_type;
    using dereference_type = typename SpanType::dereference_type;

    auto pointer = new dereference_type [array.size()] {};
    array.init(pointer);

    for (size_type i = 0; i < array.size(); ++i)
        span_impl(archive, array[i]);
}

} // namespace detail

// inline namespace common also used in namespace library
inline namespace common
{

template <class ArchiveType, typename PointerType,
          typename SizeType, typename... SizeTypes,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       meta::is_span_set<PointerType, SizeType, SizeTypes...>>::value)>
void span(ArchiveType& archive, PointerType& pointer, SizeType& dimension, SizeTypes&... dimensions)
{
    if (not detail::tracking_key(archive, pointer)) return; // serialize refer info
    archive(dimension, dimensions...);

    auto span_data = utility::make_span(pointer, dimension, dimensions...);
    detail::span_impl(archive, span_data);
}

} // inline namespace common

namespace apply
{

template <typename PointerType, typename SizeType, typename... SizeTypes>
struct span_functor_t : apply_functor_t
{
    using PackType = std::tuple<PointerType&, SizeType&, SizeTypes&...>;

    PackType pack;

    span_functor_t(PointerType& pointer, SizeType& dimension, SizeTypes&... dimensions) noexcept
        : pack(pointer, dimension, dimensions...) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const
    {
        invoke(archive, std::make_index_sequence<std::tuple_size<PackType>::value>{});
    }

private:
    template <class ArchiveType, std::size_t... SizeTypeIndexValues>
    void invoke(ArchiveType& archive, std::index_sequence<SizeTypeIndexValues...>) const
    {
        span(archive, std::get<SizeTypeIndexValues>(pack)...);
    }
};

} // namespace apply

// inline namespace common also used in namespace library
inline namespace common
{

template <typename PointerType, typename SizeType, typename... SizeTypes,
          SF_REQUIRES(meta::is_span_set<PointerType, SizeType, SizeTypes...>::value)>
apply::span_functor_t<PointerType, SizeType, SizeTypes...> span(PointerType& pointer, SizeType& dimension, SizeTypes&... dimensions) noexcept
{
    return { pointer, dimension, dimensions... };
}

} // inline namespace common

} // namespace sf

#endif // SF_UTILITY_SPAN_HPP
