#ifndef SIRAF_COMMON_HPP
#define SIRAF_COMMON_HPP

#include <Siraf/Core/Access.hpp>

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>

#include <Siraf/ExternSerialization.hpp>

#include <Siraf/DataTrack.hpp>
#include <Siraf/Compress.hpp>

namespace siraf
{

// inline namespace common also used in namespace library
inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, object,
    core::Access::is_save_class<T>() or core::Access::is_saveload_class<T>())
{
    core::Access::save(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, object,
    core::Access::is_load_class<T>() or core::Access::is_saveload_class<T>())
{
    core::Access::load(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, number, meta::is_arithmetic<T>())
{
    archive.stream().call(&number, sizeof(number));
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Save, enumerator, meta::is_enum<T>())
{
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    return archive & value;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, enumerator, meta::is_enum<T>())
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff = 0;
    archive & buff;

    enumerator = static_cast<T>(buff);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, array, meta::is_array<T>())
{
    compress::zip(archive, array);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, pointer, meta::is_serializable_raw_pointer<T>())
{
    tracking::track(archive, pointer);
    return archive;
}

} // inline namespace common

} // namespace siraf

#endif // SIRAF_COMMON_HPP
