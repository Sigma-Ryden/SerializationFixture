#ifndef SIFAR_MEMORY_HPP
#define SIFAR_MEMORY_HPP

#include <memory> // shared_ptr

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
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

template <typename T>
struct ptr_trait<shared_ptr<T>>
{
    using trait    = Shared;

    template <typename U>
    using wrapper  = shared_ptr<U>;

    using type     = shared_ptr<T>;
    using element  = T;

    using void_ptr = shared_ptr<void>;
};

template <typename T>
struct ptr_trait<T*>
{
    using trait    = Raw;

    template <typename U>
    using wrapper  = raw_ptr<U>;

    using type     = T*;
    using element  = T;

    using void_ptr = void*;
};

template <typename T, typename Pointer,
          SIREQUIRE(meta::is_shared_pointer<Pointer>())>
inline shared_ptr<T> dynamic_pointer_cast(const Pointer& pointer)
{
    return std::dynamic_pointer_cast<T>(pointer);
}

template <typename T, typename Pointer,
          SIREQUIRE(meta::is_raw_pointer<Pointer>())>
inline raw_ptr<T> dynamic_pointer_cast(const Pointer& pointer)
{
    return dynamic_cast<raw_ptr<T>>(pointer);
}

template <typename T, typename Pointer,
          SIREQUIRE(meta::is_shared_pointer<Pointer>())>
inline shared_ptr<T> static_pointer_cast(const Pointer& pointer)
{
    return std::static_pointer_cast<T>(pointer);
}

template <typename T, typename Pointer,
          SIREQUIRE(meta::is_raw_pointer<Pointer>())>
inline raw_ptr<T> static_pointer_cast(const Pointer& pointer)
{
    return static_cast<raw_ptr<T>>(pointer);
}

template <typename Pointer,
          typename VoidPointer = typename ptr_trait<Pointer>::void_ptr,
          SIREQUIRE(meta::is_pointer<Pointer>() and
                    not meta::is_pointer_to_polymorphic<Pointer>())>
inline VoidPointer pure(const Pointer& pointer)
{
    return static_pointer_cast<void>(pointer);
}

template <typename Pointer,
          typename VoidPointer = typename ptr_trait<Pointer>::void_ptr,
          SIREQUIRE(meta::is_pointer<Pointer>() and
                    meta::is_pointer_to_polymorphic<Pointer>())>
inline VoidPointer pure(const Pointer& pointer_to_polymorphic)
{
    return dynamic_pointer_cast<void>(pointer_to_polymorphic);
}

inline raw_ptr<void> pure(std::nullptr_t pointer) { return nullptr; }

template <typename T, typename Pointer,
          typename VoidPointer = typename ptr_trait<Pointer>::void_ptr,
          SIREQUIRE(meta::is_pointer<Pointer>())>
inline void assign(Pointer& pointer, const VoidPointer& address)
{
    pointer = static_pointer_cast<T>(address);
}

template <typename T, typename Pointer,
          SIREQUIRE(meta::is_raw_pointer<Pointer>())>
inline void allocate(Pointer& pointer)
{
    pointer = new T;
}

template <typename T, typename Pointer,
          SIREQUIRE(meta::is_shared_pointer<Pointer>())>
inline void allocate(Pointer& pointer)
{
    pointer = std::make_shared<T>();
}

template <typename Pointer, typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_raw_pointer<Pointer>())>
inline raw_ptr<T> raw(const Pointer& pointer) { return pointer; }

template <typename Pointer, typename T = meta::dereference<Pointer>,
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

} // namespace meta

} // namespace sifar

#endif // SIFAR_MEMORY_HPP
