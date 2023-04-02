// You can include given file to other dir level
#ifndef SIRAF_CORE_MEMORY_HPP
#define SIRAF_CORE_MEMORY_HPP

#include <memory> // shared_ptr

#include <Siraf/Core/Serialization.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace meta
{

template <typename T> constexpr bool is_memory_shared() noexcept
{
    return std::is_same<T, shared_type>::value;
}

template <typename T> constexpr bool is_memory_raw() noexcept
{
    return std::is_same<T, raw_type>::value;
}

template <typename T> constexpr bool is_memory() noexcept
{
    return is_memory_shared<T>()
        or is_memory_raw<T>();
}

} // namespace meta

class Memory
{
public:
    using Shared = meta::shared_type;
    using Raw = meta::raw_type;

    template <typename T>
    using shared_ptr = std::shared_ptr<T>;

    template <typename T>
    using raw_ptr = T*;

public:
    template <typename T>
    struct ptr_trait
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

public:
    template <typename T>
    using void_ptr = typename ptr_trait<T>::void_ptr;

public:
    template <typename To, typename Pointer,
              SIREQUIRE(meta::is_shared_pointer<Pointer>())>
    static shared_ptr<To> dynamic_pointer_cast(const Pointer& pointer)
    {
        auto address = dynamic_pointer_cast<To>(pointer.get());
        return address == nullptr ? shared_ptr<To>() : shared_ptr<To>(pointer, address);
    }

    template <typename To, typename Pointer,
              SIREQUIRE(meta::is_raw_pointer<Pointer>())>
    static raw_ptr<To> dynamic_pointer_cast(const Pointer& pointer)
    {
        return dynamic_cast<raw_ptr<To>>(pointer);
    }

    template <typename To, typename Pointer,
              typename Trait = ptr_trait<Pointer>,
              SIREQUIRE(meta::is_null_pointer<Pointer>() or
                        meta::is_pointer<Pointer>() and
                        not ::Serialization::is_pointer_cast_allowed<typename Trait::item, To>())>
    static typename Trait::template wrapper<To> static_pointer_cast(const Pointer& pointer)
    {
        return nullptr;
    }

    template <typename To, typename Pointer,
              SIREQUIRE(meta::is_shared_pointer<Pointer>() and
                        ::Serialization::is_pointer_cast_allowed<typename ptr_trait<Pointer>::item, To>())>
    static shared_ptr<To> static_pointer_cast(const Pointer& pointer)
    {
        auto address = static_pointer_cast<To>(pointer.get());
        return shared_ptr<To>(pointer, address);
    }

    template <typename To, typename Pointer,
              SIREQUIRE(meta::is_raw_pointer<Pointer>() and
                        ::Serialization::is_pointer_cast_allowed<typename ptr_trait<Pointer>::item, To>())>
    static raw_ptr<To> static_pointer_cast(const Pointer& pointer)
    {
        return static_cast<raw_ptr<To>>(pointer);
    }

    template <typename To, typename From, typename Pointer,
              typename Trait = ptr_trait<Pointer>,
              SIREQUIRE(meta::is_pointer<Pointer>() and
                        not ::Serialization::is_pointer_cast_allowed<From, To>())>
    static typename Trait::template wrapper<To> static_pointer_cast(const Pointer& pointer)
    {
        return nullptr;
    }

    template <typename To, typename From, typename Pointer,
              typename Trait = ptr_trait<Pointer>,
              SIREQUIRE(meta::is_pointer<Pointer>() and
                        ::Serialization::is_pointer_cast_allowed<typename Trait::item, From>() and
                        ::Serialization::is_pointer_cast_allowed<From, To>())>
    static typename Trait::template wrapper<To> static_pointer_cast(const Pointer& pointer)
    {
        return static_pointer_cast<To>(static_pointer_cast<From>(pointer));
    }

public:
    template <typename Pointer,
              SIREQUIRE(meta::is_pointer<Pointer>() and
                        not meta::is_pointer_to_polymorphic<Pointer>())>
    static void_ptr<Pointer> pure(const Pointer& pointer)
    {
        return static_pointer_cast<void>(pointer);
    }

    template <typename Pointer,
              SIREQUIRE(meta::is_pointer<Pointer>() and
                        meta::is_pointer_to_polymorphic<Pointer>())>
    static void_ptr<Pointer> pure(const Pointer& pointer_to_polymorphic)
    {
        return dynamic_pointer_cast<void>(pointer_to_polymorphic);
    }

    static raw_ptr<void> pure(std::nullptr_t pointer) { return nullptr; }

    template <typename T, typename Pointer,
              SIREQUIRE(meta::is_pointer<Pointer>())>
    static void assign(Pointer& pointer, const void_ptr<Pointer>& address)
    {
        pointer = static_pointer_cast<T>(address);
    }

public:
    template <typename To, typename From = To, typename TraitType,
              SIREQUIRE(meta::is_memory<TraitType>() and
                        not ::Serialization::is_pointer_cast_allowed<From, To>())>
    static std::nullptr_t allocate()
    {
        return nullptr;
    }

    template <typename To, typename From = To, typename TraitType,
              SIREQUIRE(meta::is_memory_shared<TraitType>() and
                        ::Serialization::is_pointer_cast_allowed<From, To>())>
    static shared_ptr<To> allocate()
    {
        auto instance = std::make_shared<From>();
        return static_pointer_cast<To>(instance);
    }

    template <typename To, typename From = To, typename TraitType,
              SIREQUIRE(meta::is_memory_raw<TraitType>() and
                        ::Serialization::is_pointer_cast_allowed<From, To>())>
    static raw_ptr<To> allocate()
    {
        auto instance = new From;
        return static_pointer_cast<To>(instance);
    }

    template <typename To, typename From = To>
    static shared_ptr<To> allocate_shared()
    {
        return allocate<To, From, Shared>();
    }

    template <typename To, typename From = To>
    static raw_ptr<To> allocate_raw()
    {
        return allocate<To, From, Raw>();
    }

    template <typename To, typename From = To, typename Pointer,
              SIREQUIRE(meta::is_pointer<Pointer>())>
    static void allocate(Pointer& pointer)
    {
        pointer = allocate<To, From, typename ptr_trait<Pointer>::trait>();
    }

public:
    template <typename Pointer, typename T = typename ptr_trait<Pointer>::item,
              SIREQUIRE(meta::is_raw_pointer<Pointer>())>
    static raw_ptr<T> raw(const Pointer& pointer) { return pointer; }

    template <typename Pointer, typename T = typename ptr_trait<Pointer>::item,
              SIREQUIRE(meta::is_shared_pointer<Pointer>())>
    static raw_ptr<T> raw(const Pointer& pointer) { return pointer.get(); }

public:
    template <typename ByteType = char, typename T>
    static const ByteType* const_byte_cast(T* data) noexcept
    {
        return reinterpret_cast<const ByteType*>(data);
    }

    template <typename ByteType = char, typename T>
    static ByteType* byte_cast(T* data) noexcept
    {
        return reinterpret_cast<ByteType*>(data);
    }
};

} // namespace siraf

#endif // SIRAF_CORE_MEMORY_HPP
