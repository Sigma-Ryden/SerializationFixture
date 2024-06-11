// You can include given file to other dir level
#ifndef SF_CORE_MEMORY_HPP
#define SF_CORE_MEMORY_HPP

#include <memory> // shared_ptr

#include <SF/Core/Serialization.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace memory
{

using shared_t = meta::shared_common_t;
using raw_t = meta::raw_common_t;

} // namespace memory

namespace meta
{

template <typename T> struct is_memory_shared : std::is_same<T, memory::shared_t> {};
template <typename T> struct is_memory_raw : std::is_same<T, memory::raw_t> {};

template <typename T> struct is_memory : one<is_memory_shared<T>, is_memory_raw<T>> {};

} // namespace meta

namespace memory
{

template <typename T>
using shared_ptr = std::shared_ptr<T>;

template <typename T>
using raw_ptr = T*;

template <typename T>
struct ptr_traits
{
    using item = std::nullptr_t;
};

template <typename T>
struct ptr_traits<std::shared_ptr<T>>
{
    using traits    = shared_t;

    template <typename U>
    using wrapper  = shared_ptr<U>;

    using type     = std::shared_ptr<T>;
    using item     = T;

    using void_ptr = std::shared_ptr<void>;
};

template <typename T>
struct ptr_traits<T*>
{
    using traits    = raw_t;

    template <typename U>
    using wrapper  = raw_ptr<U>;

    using type     = T*;
    using item     = T;

    using void_ptr = void*;
};

template <typename T>
struct ptr_traits<std::weak_ptr<T>>
{
    using type     = std::weak_ptr<T>;
    using item     = T;
};

template <typename T>
struct ptr_traits<std::unique_ptr<T>>
{
    using type     = std::unique_ptr<T>;
    using item     = T;
};

template <typename T>
using void_ptr = typename ptr_traits<T>::void_ptr;

template <typename T>
struct factory_t
{
    static std::shared_ptr<T> shared()
    {
        return std::make_shared<T>();
    }

    static T* raw()
    {
        return new T{};
    }
};

namespace detail
{

template <class From, class To> struct is_pointer_cast_allowed
    : ::__sf::is_pointer_cast_allowed<From, To> {};

} // namespace detail

template <typename To, typename T,
          SF_REQUIRE(meta::is_raw_pointer<T>::value)>
raw_ptr<To> dynamic_pointer_cast(const T& pointer)
{
    return dynamic_cast<raw_ptr<To>>(pointer);
}

template <typename To, typename T,
          SF_REQUIRE(meta::is_shared_pointer<T>::value)>
shared_ptr<To> dynamic_pointer_cast(const T& pointer)
{
    auto address = memory::dynamic_pointer_cast<To>(pointer.get());
    return address == nullptr ? shared_ptr<To>() : shared_ptr<To>(pointer, address);
}

template <typename To, typename T,
          typename Traits = ptr_traits<T>,
          SF_REQUIRE(meta::one<meta::is_null_pointer<T>,
                               meta::all<meta::is_pointer<T>,
                                         meta::negation<detail::is_pointer_cast_allowed<typename Traits::item, To>>>>::value)>
typename Traits::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
{
    return nullptr;
}

template <typename To, typename T,
          SF_REQUIRE(meta::all<meta::is_raw_pointer<T>,
                               detail::is_pointer_cast_allowed<typename ptr_traits<T>::item, To>>::value)>
raw_ptr<To> static_pointer_cast(const T& pointer) noexcept
{
    return static_cast<raw_ptr<To>>(pointer);
}

template <typename To, typename From, typename T,
          typename Traits = ptr_traits<T>,
          SF_REQUIRE(meta::all<meta::is_pointer<T>,
                               meta::negation<detail::is_pointer_cast_allowed<From, To>>>::value)>
typename Traits::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
{
    return nullptr;
}

template <typename To, typename T,
          SF_REQUIRE(meta::all<meta::is_shared_pointer<T>,
                               detail::is_pointer_cast_allowed<typename ptr_traits<T>::item, To>>::value)>
shared_ptr<To> static_pointer_cast(const T& pointer) noexcept
{
    auto address = memory::static_pointer_cast<To>(pointer.get());
    return shared_ptr<To>(pointer, address);
}

template <typename To, typename From, typename T,
          typename Traits = ptr_traits<T>,
          SF_REQUIRE(meta::all<meta::is_pointer<T>,
                               detail::is_pointer_cast_allowed<typename Traits::item, From>,
                               detail::is_pointer_cast_allowed<From, To>>::value)>
typename Traits::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
{
    return memory::static_pointer_cast<To>(memory::static_pointer_cast<From>(pointer));
}

template <typename T,
          SF_REQUIRE(meta::all<meta::is_pointer<T>,
                               meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void_ptr<T> pure(const T& pointer) noexcept
{
    return memory::static_pointer_cast<void>(pointer);
}

template <typename T,
          SF_REQUIRE(meta::all<meta::is_pointer<T>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void_ptr<T> pure(const T& pointer_to_polymorphic)
{
    return memory::dynamic_pointer_cast<void>(pointer_to_polymorphic);
}

inline raw_ptr<void> pure(std::nullptr_t pointer) noexcept { return nullptr; }

template <typename dT, typename T,
          SF_REQUIRE(meta::is_pointer<T>::value)>
void assign(T& pointer, const void_ptr<T>& address) noexcept
{
    pointer = memory::static_pointer_cast<dT>(address);
}

template <typename To, typename From = To, typename TraitsType,
          SF_REQUIRE(meta::all<meta::is_memory<TraitsType>,
                               meta::one<meta::negation<detail::is_pointer_cast_allowed<From, To>>,
                                         std::is_abstract<From>>>::value)>
std::nullptr_t allocate() noexcept
{
    return nullptr;
}

template <typename To, typename From = To, typename TraitsType,
          SF_REQUIRE(meta::all<meta::negation<std::is_abstract<From>>,
                               meta::is_memory_shared<TraitsType>,
                               detail::is_pointer_cast_allowed<From, To>>::value)>
shared_ptr<To> allocate()
{
    auto instance = factory_t<From>::shared();
    return memory::static_pointer_cast<To>(instance);
}

template <typename To, typename From = To, typename TraitsType,
          SF_REQUIRE(meta::all<meta::negation<std::is_abstract<From>>,
                               meta::is_memory_raw<TraitsType>,
                               detail::is_pointer_cast_allowed<From, To>>::value)>
raw_ptr<To> allocate()
{
    auto instance = factory_t<From>::raw();
    return memory::static_pointer_cast<To>(instance);
}

template <typename To, typename From = To>
shared_ptr<To> allocate_shared()
{
    return memory::allocate<To, From, shared_t>();
}

template <typename To, typename From = To>
raw_ptr<To> allocate_raw()
{
    return memory::allocate<To, From, raw_t>();
}

template <typename To, typename From = To, typename T,
          SF_REQUIRE(meta::is_pointer<T>::value)>
void allocate(T& pointer)
{
    pointer = memory::allocate<To, From, typename ptr_traits<T>::traits>();
}

template <typename T, typename dT = typename ptr_traits<T>::item,
          SF_REQUIRE(meta::is_raw_pointer<T>::value)>
raw_ptr<dT> raw(const T& pointer) { return pointer; }

template <typename T, typename dT = typename ptr_traits<T>::item,
          SF_REQUIRE(meta::is_shared_pointer<T>::value)>
raw_ptr<dT> raw(const T& pointer) { return pointer.get(); }

template <typename ByteType = char, typename T>
const ByteType* const_byte_cast(T* data) noexcept
{
    return reinterpret_cast<const ByteType*>(data);
}

template <typename ByteType = char, typename T>
ByteType* byte_cast(T* data) noexcept
{
    return reinterpret_cast<ByteType*>(data);
}

} // namespace memory

} // namespace sf

#endif // SF_CORE_MEMORY_HPP
