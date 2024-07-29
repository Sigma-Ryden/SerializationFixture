#ifndef SF_COMMON_HPP
#define SF_COMMON_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/OArchive.hpp>
#include <SF/IArchive.hpp>

#include <SF/ExternSerialization.hpp>

#include <SF/Binary.hpp>
#include <SF/DataTrack.hpp>
#include <SF/Compress.hpp>

namespace sf
{

// inline namespace common also used in namespace library
inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(save, object, meta::is_has_any_save_mode<T>::value)
{
    ::xxsf::save(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(load, object, meta::is_has_any_load_mode<T>::value)
{
    ::xxsf::load(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(saveload, number, std::is_arithmetic<T>::value)
{
    binary(archive, number);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(save, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    return archive & value;
}

EXTERN_CONDITIONAL_SERIALIZATION(load, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff{};
    archive & buff;

    enumerator = static_cast<T>(buff);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(saveload, array, std::is_array<T>::value)
{
    compress::zip(archive, array);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(saveload, pointer, meta::is_serializable_raw_pointer<T>::value)
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

CONDITIONAL_TYPE_REGISTRY(::sf::meta::all<::sf::meta::is_has_any_save_mode<T>, ::sf::meta::is_has_any_load_mode<T>>::value)

CONDITIONAL_TYPE_REGISTRY(std::is_arithmetic<T>::value)
CONDITIONAL_TYPE_REGISTRY(std::is_enum<T>::value)
CONDITIONAL_TYPE_REGISTRY(std::is_array<T>::value)
CONDITIONAL_TYPE_REGISTRY(::sf::meta::is_serializable_raw_pointer<T>::value)

#endif // SF_COMMON_HPP
