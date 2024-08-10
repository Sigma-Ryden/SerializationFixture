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

template <typename TraitsType> struct is_memory_shared : std::is_same<TraitsType, memory::shared_t> {};
template <typename TraitsType> struct is_memory_raw : std::is_same<TraitsType, memory::raw_t> {};

template <typename TraitsType> struct is_memory : one<is_memory_shared<TraitsType>, is_memory_raw<TraitsType>> {};

} // namespace meta

namespace memory
{

template <typename Type>
using shared_ptr = std::shared_ptr<Type>;

template <typename Type>
using raw_ptr = Type*;

template <typename PointerType>
struct ptr_traits
{
    using item = std::nullptr_t;
};

template <typename Type>
struct ptr_traits<std::shared_ptr<Type>>
{
    using traits = shared_t;

    template <typename OtherType>
    using wrapper = shared_ptr<OtherType>;

    using type = std::shared_ptr<Type>;
    using item = Type;

    using void_ptr = std::shared_ptr<void>;
};

template <typename Type>
struct ptr_traits<Type*>
{
    using traits = raw_t;

    template <typename OtherType>
    using wrapper = raw_ptr<OtherType>;

    using type = Type*;
    using item = Type;

    using void_ptr = void*;
};

template <typename Type>
struct ptr_traits<std::weak_ptr<Type>>
{
    using type = std::weak_ptr<Type>;
    using item = Type;
};

template <typename Type>
struct ptr_traits<std::unique_ptr<Type>>
{
    using type = std::unique_ptr<Type>;
    using item = Type;
};

template <typename PointerType>
using void_ptr = typename ptr_traits<PointerType>::void_ptr;

template <typename Type>
struct factory_t
{
    static std::shared_ptr<Type> shared()
    {
        return std::make_shared<Type>();
    }

    static Type* raw()
    {
        return new Type{};
    }
};

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::is_raw_pointer<PointerType>::value)>
raw_ptr<ToType> dynamic_pointer_cast(PointerType const& pointer)
{
    return dynamic_cast<raw_ptr<ToType>>(pointer);
}

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::is_shared_pointer<PointerType>::value)>
shared_ptr<ToType> dynamic_pointer_cast(PointerType const& pointer)
{
    auto address = memory::dynamic_pointer_cast<ToType>(pointer.get());
    return address == nullptr ? shared_ptr<ToType>() : shared_ptr<ToType>(pointer, address);
}

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::one<meta::is_null_pointer<PointerType>,
                                meta::all<meta::is_pointer<PointerType>,
                                          meta::negation<meta::is_static_castable<typename ptr_traits<PointerType>::item*, ToType*>>>>::value)>
typename ptr_traits<PointerType>::template wrapper<ToType> static_pointer_cast(PointerType const&) noexcept
{
    return nullptr;
}

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_raw_pointer<PointerType>,
                                meta::is_static_castable<typename ptr_traits<PointerType>::item*, ToType*>>::value)>
raw_ptr<ToType> static_pointer_cast(PointerType const& pointer) noexcept
{
    return static_cast<raw_ptr<ToType>>(pointer);
}

template <typename ToType, typename FromType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_pointer<PointerType>,
                                meta::negation<meta::is_static_castable<FromType*, ToType*>>>::value)>
typename ptr_traits<PointerType>::template wrapper<ToType> static_pointer_cast(PointerType const&) noexcept
{
    return nullptr;
}

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_shared_pointer<PointerType>,
                                meta::is_static_castable<typename ptr_traits<PointerType>::item*, ToType*>>::value)>
shared_ptr<ToType> static_pointer_cast(PointerType const& pointer) noexcept
{
    auto address = memory::static_pointer_cast<ToType>(pointer.get());
    return shared_ptr<ToType>(pointer, address);
}

template <typename ToType, typename FromType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_pointer<PointerType>,
                                meta::is_static_castable<typename ptr_traits<PointerType>::item*, FromType*>,
                                meta::is_static_castable<FromType*, ToType*>>::value)>
typename ptr_traits<PointerType>::template wrapper<ToType> static_pointer_cast(PointerType const& pointer) noexcept
{
    return memory::static_pointer_cast<ToType>(memory::static_pointer_cast<FromType>(pointer));
}

template <typename PointerType,
          SF_REQUIRES(meta::all<meta::is_pointer<PointerType>,
                                meta::negation<meta::is_pointer_to_polymorphic<PointerType>>>::value)>
void_ptr<PointerType> pure(PointerType const& pointer) noexcept
{
    return memory::static_pointer_cast<void>(pointer);
}

template <typename PointerType,
          SF_REQUIRES(meta::all<meta::is_pointer<PointerType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void_ptr<PointerType> pure(PointerType const& pointer_to_polymorphic)
{
    return memory::dynamic_pointer_cast<void>(pointer_to_polymorphic);
}

inline raw_ptr<void> pure(std::nullptr_t) noexcept { return nullptr; }

template <typename Type, typename PointerType,
          SF_REQUIRES(meta::is_pointer<PointerType>::value)>
void assign(PointerType& pointer, void_ptr<PointerType> const& address) noexcept
{
    pointer = memory::static_pointer_cast<Type>(address);
}

template <typename ToType, typename FromType = ToType, typename TraitsType,
          SF_REQUIRES(meta::all<meta::is_memory<TraitsType>,
                                meta::one<meta::negation<meta::is_static_castable<FromType*, ToType*>>,
                                          std::is_abstract<FromType>>>::value)>
std::nullptr_t allocate() noexcept
{
    return nullptr;
}

template <typename ToType, typename FromType = ToType, typename TraitsType,
          SF_REQUIRES(meta::all<meta::negation<std::is_abstract<FromType>>,
                                meta::is_memory_shared<TraitsType>,
                                meta::is_static_castable<FromType*, ToType*>>::value)>
shared_ptr<ToType> allocate()
{
    auto instance = factory_t<FromType>::shared();
    return memory::static_pointer_cast<ToType>(instance);
}

template <typename ToType, typename FromType = ToType, typename TraitsType,
          SF_REQUIRES(meta::all<meta::negation<std::is_abstract<FromType>>,
                                meta::is_memory_raw<TraitsType>,
                                meta::is_static_castable<FromType*, ToType*>>::value)>
raw_ptr<ToType> allocate()
{
    auto instance = factory_t<FromType>::raw();
    return memory::static_pointer_cast<ToType>(instance);
}

template <typename ToType, typename FromType = ToType>
shared_ptr<ToType> allocate_shared()
{
    return memory::allocate<ToType, FromType, shared_t>();
}

template <typename ToType, typename FromType = ToType>
raw_ptr<ToType> allocate_raw()
{
    return memory::allocate<ToType, FromType, raw_t>();
}

template <typename ToType, typename FromType = ToType, typename PointerType,
          SF_REQUIRES(meta::is_pointer<PointerType>::value)>
void allocate(PointerType& pointer)
{
    pointer = memory::allocate<ToType, FromType, typename ptr_traits<PointerType>::traits>();
}

template <typename PointerType,
          SF_REQUIRES(meta::is_raw_pointer<PointerType>::value)>
raw_ptr<typename ptr_traits<PointerType>::item> raw(PointerType const& pointer) { return pointer; }

template <typename PointerType,
          SF_REQUIRES(meta::is_shared_pointer<PointerType>::value)>
raw_ptr<typename ptr_traits<PointerType>::item> raw(PointerType const& pointer) { return pointer.get(); }

template <typename ByteType = char, typename DataType>
ByteType const* const_byte_cast(DataType* data) noexcept
{
    return reinterpret_cast<ByteType const*>(data);
}

template <typename ByteType = char, typename DataType>
ByteType* byte_cast(DataType* data) noexcept
{
    return reinterpret_cast<ByteType*>(data);
}

} // namespace memory

} // namespace sf

#endif // SF_CORE_MEMORY_HPP
