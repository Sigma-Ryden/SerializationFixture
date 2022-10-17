#ifndef SIFAR_COMMON_HPP
#define SIFAR_COMMON_HPP

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/UnifiedData.hpp>

#include <Sifar/Compress.hpp>

namespace sifar
{

// inline namespace common also used in namespace library
inline namespace common
{

SERIALIZATION_SAVE_DATA(object, Access::is_save_class<T>())
{
    Access::save(archive, object);

    return archive;
}

SERIALIZATION_LOAD_DATA(object, Access::is_load_class<T>())
{
    Access::load(archive, object);

    return archive;
}

SERIALIZATION_SAVE_LOAD_DATA(number, meta::is_arithmetic<T>())
{
    archive.stream().call(&number, sizeof(number));

    return archive;
}

SERIALIZATION_SAVE_DATA(enumerator, meta::is_enum<T>())
{
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    return archive & value;
}

SERIALIZATION_LOAD_DATA(enumerator, meta::is_enum<T>())
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff = 0;

    archive & buff;

    enumerator = static_cast<T>(buff);

    return archive;
}

SERIALIZATION_SAVE_LOAD_DATA(array, meta::is_array<T>())
{
    compress::zip(archive, array);

    return archive;
}

SERIALIZATION_SAVE_DATA(pod_pointer, meta::is_pod_pointer<T>())
{
    if (pod_pointer == nullptr)
        throw "the write pointer must be allocated.";

    archive & (*pod_pointer);

    return archive;
}

SERIALIZATION_LOAD_DATA(pod_pointer, meta::is_pod_pointer<T>())
{
    using value_type = meta::dereference<T>;

    if (pod_pointer != nullptr)
        throw "the read pointer must be initialized to nullptr.";

    pod_pointer = new value_type;

    archive & (*pod_pointer);

    return archive;
}

SERIALIZATION_SAVE_DATA(pointer, meta::is_pointer_to_polymorphic<T>())
{
    auto& registry = archive.registry();

    auto id = registry.save_key(archive, pointer);
    registry.save(archive, pointer, id);

    return archive;
}

SERIALIZATION_LOAD_DATA(pointer, meta::is_pointer_to_polymorphic<T>())
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.load(archive, pointer, id);

    return archive;
}

} // inline namespace common

} // namespace sifar

#endif // SIFAR_COMMON_HPP
