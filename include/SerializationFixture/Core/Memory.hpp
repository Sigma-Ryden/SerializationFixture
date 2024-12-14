// You can include given file to other dir level
#ifndef SF_CORE_MEMORY_HPP
#define SF_CORE_MEMORY_HPP

#include <memory> // shared_ptr

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
{

namespace memory
{

template <typename>
struct pointer_traits;

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


// raw ptr
namespace memory
{

template <typename SerializableType>
struct pointer_traits<SerializableType*>
{
    using element_type = SerializableType;
};

template <typename ToType, typename SerializableType>
ToType* dynamic_pointer_cast(SerializableType* pointer)
{
    return dynamic_cast<ToType*>(pointer);
}

template <typename ToType, typename FromType,
          SF_REQUIRES(meta::negation<meta::is_static_castable<FromType*, ToType*>>::value)>
ToType* static_pointer_cast(FromType*) noexcept
{
    return nullptr;
}

template <typename ToType, typename FromType,
          SF_REQUIRES(meta::is_static_castable<FromType*, ToType*>::value)>
ToType* static_pointer_cast(FromType* pointer) noexcept
{
    return static_cast<ToType*>(pointer);
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<std::is_polymorphic<SerializableType>>::value)>
void* pure(SerializableType* pointer) noexcept
{
    return memory::static_pointer_cast<void>(pointer);
}

template <typename SerializableType,
          SF_REQUIRES(std::is_polymorphic<SerializableType>::value)>
void* pure(SerializableType* pointer_to_polymorphic)
{
    return memory::dynamic_pointer_cast<void>(pointer_to_polymorphic);
}

template <typename SerializableType>
SerializableType* raw(SerializableType* pointer)
{
    return pointer;
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<std::is_abstract<SerializableType>>::value)>
void allocate(SerializableType*& pointer)
{
    pointer = new SerializableType;
}

template <typename SerializableType,
          SF_REQUIRES(std::is_abstract<SerializableType>::value)>
void allocate(SerializableType*& pointer)
{
    pointer = nullptr;
}

} // namespace memory


// std shared ptr
namespace memory
{

template <typename SerializableType>
struct pointer_traits<std::shared_ptr<SerializableType>>
{
    using element_type = SerializableType;
};

template <typename ToType, typename FromType>
std::shared_ptr<ToType> dynamic_pointer_cast(std::shared_ptr<FromType> const& pointer)
{
    return std::dynamic_pointer_cast<ToType>(pointer);
}

template <typename ToType, typename FromType,
          SF_REQUIRES(meta::negation<meta::is_static_castable<FromType*, ToType*>>::value)>
std::shared_ptr<ToType> static_pointer_cast(std::shared_ptr<FromType> const&) noexcept
{
    return nullptr;
}

template <typename ToType, typename FromType,
          SF_REQUIRES(meta::is_static_castable<FromType*, ToType*>::value)>
std::shared_ptr<ToType> static_pointer_cast(std::shared_ptr<FromType> const& pointer) noexcept
{
    return std::static_pointer_cast<ToType>(pointer);
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<std::is_polymorphic<SerializableType>>::value)>
std::shared_ptr<void> pure(std::shared_ptr<SerializableType> const& pointer) noexcept
{
    return memory::static_pointer_cast<void>(pointer);
}

template <typename SerializableType,
          SF_REQUIRES(std::is_polymorphic<SerializableType>::value)>
std::shared_ptr<void> pure(std::shared_ptr<SerializableType> const& pointer_to_polymorphic)
{
    return memory::dynamic_pointer_cast<void>(pointer_to_polymorphic);
}

template <typename SerializableType>
SerializableType* raw(std::shared_ptr<SerializableType> const& pointer)
{
    return pointer.get();
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<std::is_abstract<SerializableType>>::value)>
void allocate(std::shared_ptr<SerializableType>& pointer)
{
    pointer = std::make_shared<SerializableType>();
}

template <typename SerializableType,
          SF_REQUIRES(std::is_abstract<SerializableType>::value)>
void allocate(std::shared_ptr<SerializableType>& pointer)
{
    pointer = nullptr;
}

} // namespace memory

} // namespace sf

#endif // SF_CORE_MEMORY_HPP
