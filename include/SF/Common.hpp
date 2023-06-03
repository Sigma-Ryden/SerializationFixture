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

EXTERN_CONDITIONAL_SERIALIZATION(Save, object, ::Serialization::has_save_mode<T>::value)
{
    ::Serialization::save(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, object, ::Serialization::has_load_mode<T>::value)
{
    ::Serialization::load(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, number, std::is_arithmetic<T>::value)
{
    archive.stream().call(&number, sizeof(number));
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Save, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    return archive & value;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff{};
    archive & buff;

    enumerator = static_cast<T>(buff);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, array, std::is_array<T>::value)
{
    compress::zip(archive, array);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, pointer, meta::is_serializable_raw_pointer<T>::value)
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

CONDITIONAL_TYPE_REGISTRY(meta::all<::Serialization::has_save_mode<T>, ::Serialization::has_load_mode<T>>::value)

CONDITIONAL_TYPE_REGISTRY(std::is_arithmetic<T>::value)
CONDITIONAL_TYPE_REGISTRY(std::is_enum<T>::value)
CONDITIONAL_TYPE_REGISTRY(std::is_array<T>::value)
CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_raw_pointer<T>::value)

#endif // SF_COMMON_HPP
