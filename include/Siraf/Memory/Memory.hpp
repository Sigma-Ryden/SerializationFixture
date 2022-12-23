#ifndef SIRAF_MEMORY_HPP
#define SIRAF_MEMORY_HPP

#include <memory> // shared_ptr

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace memory
{

using Shared = meta::shared_type;
using Raw = meta::raw_type;

template <typename T>
using shared_ptr = std::shared_ptr<T>;

template <typename T>
using raw_ptr = T*;

template <typename T> struct ptr_trait;

template <>
struct ptr_trait<std::nullptr_t>
{
    using item = std::nullptr_t;
};

template <typename T>
struct ptr_trait<std::shared_ptr<T>>
{
    using trait    = Shared;

    template <typename U>
    using wrapper  = shared_ptr<U>;

    using type     = std::shared_ptr<T>;
    using item     = T;

    using void_ptr = std::shared_ptr<void>;
};

template <typename T>
struct ptr_trait<T*>
{
    using trait    = Raw;

    template <typename U>
    using wrapper  = raw_ptr<U>;

    using type     = T*;
    using item     = T;

    using void_ptr = void*;
};

template <typename T>
struct ptr_trait<std::weak_ptr<T>>
{
    using type     = std::weak_ptr<T>;
    using item     = T;
};

template <typename T>
struct ptr_trait<std::unique_ptr<T>>
{
    using type     = std::unique_ptr<T>;
    using item     = T;
};

} // namespace memory

namespace meta
{

template <typename T> constexpr bool is_memory_shared() noexcept
{
    return std::is_same<T, memory::Shared>::value;
}

template <typename T> constexpr bool is_memory_raw() noexcept
{
    return std::is_same<T, memory::Raw>::value;
}

template <typename T> constexpr bool is_memory() noexcept
{
    return is_memory_shared<T>()
        or is_memory_raw<T>();
}

} // namespace meta

namespace memory
{

template <typename To, typename Pointer,
          SIREQUIRE(meta::is_shared_pointer<Pointer>())>
inline shared_ptr<To> dynamic_pointer_cast(const Pointer& pointer)
{
    return std::dynamic_pointer_cast<To>(pointer);
}

template <typename To, typename Pointer,
          SIREQUIRE(meta::is_raw_pointer<Pointer>())>
inline raw_ptr<To> dynamic_pointer_cast(const Pointer& pointer)
{
    return dynamic_cast<raw_ptr<To>>(pointer);
}

template <typename To, typename Pointer,
          typename Trait = ptr_trait<Pointer>,
          SIREQUIRE(meta::is_null_pointer<Pointer>() or
                    meta::is_pointer<Pointer>() and
                    not meta::is_convertible<typename Trait::item, To>())>
inline typename Trait::template wrapper<To> static_pointer_cast(const Pointer& pointer)
{
    return nullptr;
}

template <typename To, typename Pointer,
          SIREQUIRE(meta::is_shared_pointer<Pointer>() and
                    meta::is_convertible<typename ptr_trait<Pointer>::item, To>())>
inline shared_ptr<To> static_pointer_cast(const Pointer& pointer)
{
    return std::static_pointer_cast<To>(pointer);
}

template <typename To, typename Pointer,
          SIREQUIRE(meta::is_raw_pointer<Pointer>() and
                    meta::is_convertible<typename ptr_trait<Pointer>::item, To>())>
inline raw_ptr<To> static_pointer_cast(const Pointer& pointer)
{
    return static_cast<raw_ptr<To>>(pointer);
}

template <typename To, typename From, typename Pointer,
          typename Trait = ptr_trait<Pointer>,
          SIREQUIRE(meta::is_pointer<Pointer>() and
                    not meta::is_convertible<From, To>())>
inline typename Trait::template wrapper<To> static_pointer_cast(const Pointer& pointer)
{
    return nullptr;
}

template <typename To, typename From, typename Pointer,
          typename Trait = ptr_trait<Pointer>,
          SIREQUIRE(meta::is_pointer<Pointer>() and
                    meta::is_convertible<typename Trait::item, From>() and
                    meta::is_convertible<From, To>())>
inline typename Trait::template wrapper<To> static_pointer_cast(const Pointer& pointer)
{
    return static_pointer_cast<To>(static_pointer_cast<From>(pointer));
}

template <typename Pointer,
          SIREQUIRE(meta::is_pointer<Pointer>() and
                    not meta::is_pointer_to_polymorphic<Pointer>())>
inline typename ptr_trait<Pointer>::void_ptr pure(const Pointer& pointer)
{
    return static_pointer_cast<void>(pointer);
}

template <typename Pointer,
          SIREQUIRE(meta::is_pointer<Pointer>() and
                    meta::is_pointer_to_polymorphic<Pointer>())>
inline typename ptr_trait<Pointer>::void_ptr pure(const Pointer& pointer_to_polymorphic)
{
    return dynamic_pointer_cast<void>(pointer_to_polymorphic);
}

inline raw_ptr<void> pure(std::nullptr_t pointer) { return nullptr; }

template <typename T, typename Pointer,
          SIREQUIRE(meta::is_pointer<Pointer>())>
inline void assign(Pointer& pointer, const typename ptr_trait<Pointer>::void_ptr& address)
{
    pointer = static_pointer_cast<T>(address);
}

template <typename To, typename From = To, typename TraitType,
          SIREQUIRE(meta::is_memory<TraitType>() and
                    not meta::is_convertible<From, To>())>
inline std::nullptr_t allocate()
{
    return nullptr;
}

template <typename To, typename From = To, typename TraitType,
          SIREQUIRE(meta::is_memory_shared<TraitType>() and
                    meta::is_convertible<From, To>())>
inline shared_ptr<To> allocate()
{
    return std::make_shared<From>();
}

template <typename To, typename From = To, typename TraitType,
          SIREQUIRE(meta::is_memory_raw<TraitType>() and
                    meta::is_convertible<From, To>())>
inline raw_ptr<To> allocate()
{
    return new From;
}

template <typename To, typename From = To>
inline shared_ptr<To> allocate_shared()
{
    return allocate<To, From, Shared>();
}

template <typename To, typename From = To>
inline raw_ptr<To> allocate_raw()
{
    return allocate<To, From, Raw>();
}

template <typename To, typename From = To, typename Pointer,
          SIREQUIRE(meta::is_pointer<Pointer>())>
inline void allocate(Pointer& pointer)
{
    pointer = allocate<To, From, typename ptr_trait<Pointer>::trait>();
}

template <typename Pointer, typename T = typename ptr_trait<Pointer>::item,
          SIREQUIRE(meta::is_raw_pointer<Pointer>())>
inline raw_ptr<T> raw(const Pointer& pointer) { return pointer; }

template <typename Pointer, typename T = typename ptr_trait<Pointer>::item,
          SIREQUIRE(meta::is_shared_pointer<Pointer>())>
inline raw_ptr<T> raw(const Pointer& pointer) { return pointer.get(); }

template <typename T> inline const char* const_byte_cast(T* data) noexcept
{
    return reinterpret_cast<const char*>(data);
}

template <typename T> inline char* byte_cast(T* data) noexcept
{
    return reinterpret_cast<char*>(data);
}

} // namespace memory

} // namespace siraf

#endif // SIRAF_MEMORY_HPP
