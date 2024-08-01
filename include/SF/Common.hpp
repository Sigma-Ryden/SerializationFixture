#ifndef SF_COMMON_HPP
#define SF_COMMON_HPP

#include <SF/Core/Serialization.hpp>

#include <SF/OArchive.hpp>
#include <SF/IArchive.hpp>

#include <SF/Binary.hpp>
#include <SF/DataTrack.hpp>
#include <SF/Compress.hpp>

template <class T>
struct xxsf_save<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
{
    template <class Archive>
    xxsf_save(Archive& archive, T& number)
    {
        ::sf::binary(archive, number);
    }
};

template <class T>
struct xxsf_load<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
{
    template <class Archive>
    xxsf_load(Archive& archive, T& number)
    {
        ::sf::binary(archive, number);
    }
};

CONDITIONAL_SERIALIZATION(save, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    archive & value;
}

CONDITIONAL_SERIALIZATION(load, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff{};
    archive & buff;

    enumerator = static_cast<T>(buff);
}

CONDITIONAL_SERIALIZATION(saveload, array, std::is_array<T>::value)
{
    ::sf::compress::zip(archive, array);
}

CONDITIONAL_SERIALIZATION(saveload, pointer, ::sf::meta::is_serializable_raw_pointer<T>::value)
{
#ifdef SF_PTRTRACK_DISABLE
    ::sf::tracking::raw(archive, pointer);
#else
    ::sf::tracking::track(archive, pointer);
#endif // SF_PTRTRACK_DISABLE
}

#endif // SF_COMMON_HPP
