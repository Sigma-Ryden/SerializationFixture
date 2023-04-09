#ifndef SF_COMMON_HPP
#define SF_COMMON_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/WriteArchive.hpp>
#include <SF/ReadArchive.hpp>

#include <SF/ExternSerialization.hpp>

#include <SF/DataTrack.hpp>
#include <SF/Compress.hpp>

namespace sf
{

// inline namespace common also used in namespace library
inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, object, ::Serialization::has_save_mode<T>())
{
    ::Serialization::save(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, object, ::Serialization::has_load_mode<T>())
{
    ::Serialization::load(archive, object);
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
#ifdef SF_PTRTRACK_DISABLE
    tracking::raw(archive, pointer);
#else
    tracking::track(archive, pointer);
#endif // SF_PTRTRACK_DISABLE

    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(::Serialization::has_save_mode<T>() and ::Serialization::has_load_mode<T>())

CONDITIONAL_TYPE_REGISTRY(meta::is_arithmetic<T>())
CONDITIONAL_TYPE_REGISTRY(meta::is_enum<T>())
CONDITIONAL_TYPE_REGISTRY(meta::is_array<T>())
CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_raw_pointer<T>())

#endif // SF_COMMON_HPP
