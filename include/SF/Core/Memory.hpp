// You can include given file to other dir level
#ifndef SF_CORE_MEMORY_HPP
#define SF_CORE_MEMORY_HPP

#include <memory> // shared_ptr

#include <SF/Core/Serialization.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace meta
{

template <typename T> struct is_memory_shared : std::is_same<T, shared_type> {};
template <typename T> struct is_memory_raw : std::is_same<T, raw_type> {};

template <typename T> struct is_memory : one<is_memory_shared<T>, is_memory_raw<T>> {};

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
    template <typename T>
    struct Factory
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

private:
    template <class From, class To> struct is_pointer_cast_allowed
        : ::Serialization::is_pointer_cast_allowed<From, To> {};

public:
    template <typename To, typename T,
              SFREQUIRE(meta::is_shared_pointer<T>::value)>
    static shared_ptr<To> dynamic_pointer_cast(const T& pointer)
    {
        auto address = dynamic_pointer_cast<To>(pointer.get());
        return address == nullptr ? shared_ptr<To>() : shared_ptr<To>(pointer, address);
    }

    template <typename To, typename T,
              SFREQUIRE(meta::is_raw_pointer<T>::value)>
    static raw_ptr<To> dynamic_pointer_cast(const T& pointer)
    {
        return dynamic_cast<raw_ptr<To>>(pointer);
    }

    template <typename To, typename T,
              typename Trait = ptr_trait<T>,
              SFREQUIRE(meta::one<meta::is_null_pointer<T>,
                                  meta::all<meta::is_pointer<T>,
                                            meta::negation<is_pointer_cast_allowed<typename Trait::item, To>>>>::value)>
    static typename Trait::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
    {
        return nullptr;
    }

    template <typename To, typename T,
              SFREQUIRE(meta::all<meta::is_shared_pointer<T>,
                                  is_pointer_cast_allowed<typename ptr_trait<T>::item, To>>::value)>
    static shared_ptr<To> static_pointer_cast(const T& pointer) noexcept
    {
        auto address = static_pointer_cast<To>(pointer.get());
        return shared_ptr<To>(pointer, address);
    }

    template <typename To, typename T,
              SFREQUIRE(meta::all<meta::is_raw_pointer<T>,
                                  is_pointer_cast_allowed<typename ptr_trait<T>::item, To>>::value)>
    static raw_ptr<To> static_pointer_cast(const T& pointer) noexcept
    {
        return static_cast<raw_ptr<To>>(pointer);
    }

    template <typename To, typename From, typename T,
              typename Trait = ptr_trait<T>,
              SFREQUIRE(meta::all<meta::is_pointer<T>,
                                  meta::negation<is_pointer_cast_allowed<From, To>>>::value)>
    static typename Trait::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
    {
        return nullptr;
    }

    template <typename To, typename From, typename T,
              typename Trait = ptr_trait<T>,
              SFREQUIRE(meta::all<meta::is_pointer<T>,
                                  is_pointer_cast_allowed<typename Trait::item, From>,
                                  is_pointer_cast_allowed<From, To>>::value)>
    static typename Trait::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
    {
        return static_pointer_cast<To>(static_pointer_cast<From>(pointer));
    }

public:
    template <typename T,
              SFREQUIRE(meta::all<meta::is_pointer<T>,
                                  meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
    static void_ptr<T> pure(const T& pointer) noexcept
    {
        return static_pointer_cast<void>(pointer);
    }

    template <typename T,
              SFREQUIRE(meta::all<meta::is_pointer<T>,
                                  meta::is_pointer_to_polymorphic<T>>::value)>
    static void_ptr<T> pure(const T& pointer_to_polymorphic)
    {
        return dynamic_pointer_cast<void>(pointer_to_polymorphic);
    }

    static raw_ptr<void> pure(std::nullptr_t pointer) noexcept { return nullptr; }

    template <typename dT, typename T,
              SFREQUIRE(meta::is_pointer<T>::value)>
    static void assign(T& pointer, const void_ptr<T>& address) noexcept
    {
        pointer = static_pointer_cast<dT>(address);
    }

public:
    template <typename To, typename From = To, typename TraitType,
              SFREQUIRE(meta::all<meta::is_memory<TraitType>,
                                  meta::one<meta::negation<is_pointer_cast_allowed<From, To>>,
                                            std::is_abstract<From>>>::value)>
    static std::nullptr_t allocate() noexcept
    {
        return nullptr;
    }

    template <typename To, typename From = To, typename TraitType,
              SFREQUIRE(meta::all<meta::negation<std::is_abstract<From>>,
                                  meta::is_memory_shared<TraitType>,
                                  is_pointer_cast_allowed<From, To>>::value)>
    static shared_ptr<To> allocate()
    {
        auto instance = Factory<From>::shared();
        return static_pointer_cast<To>(instance);
    }

    template <typename To, typename From = To, typename TraitType,
              SFREQUIRE(meta::all<meta::negation<std::is_abstract<From>>,
                                  meta::is_memory_raw<TraitType>,
                                  is_pointer_cast_allowed<From, To>>::value)>
    static raw_ptr<To> allocate()
    {
        auto instance = Factory<From>::raw();
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

    template <typename To, typename From = To, typename T,
              SFREQUIRE(meta::is_pointer<T>::value)>
    static void allocate(T& pointer)
    {
        pointer = allocate<To, From, typename ptr_trait<T>::trait>();
    }

public:
    template <typename T, typename dT = typename ptr_trait<T>::item,
              SFREQUIRE(meta::is_raw_pointer<T>::value)>
    static raw_ptr<dT> raw(const T& pointer) { return pointer; }

    template <typename T, typename dT = typename ptr_trait<T>::item,
              SFREQUIRE(meta::is_shared_pointer<T>::value)>
    static raw_ptr<dT> raw(const T& pointer) { return pointer.get(); }

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

} // namespace sf

#endif // SF_CORE_MEMORY_HPP
