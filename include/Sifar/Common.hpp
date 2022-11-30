#ifndef SIFAR_COMMON_HPP
#define SIFAR_COMMON_HPP

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/UnifiedData.hpp>

#include <Sifar/Strict.hpp>
#include <Sifar/Compress.hpp>

namespace sifar
{

// inline namespace common also used in namespace library
inline namespace common
{

CONDITIONAL_SAVE_SERIALIZABLE_TYPE(object, Access::is_save_class<T>())
{
    Access::save(archive, object);

    return archive;
}

CONDITIONAL_LOAD_SERIALIZABLE_TYPE(object, Access::is_load_class<T>())
{
    Access::load(archive, object);

    return archive;
}

CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(number, meta::is_arithmetic<T>())
{
    archive.stream().call(&number, sizeof(number));

    return archive;
}

CONDITIONAL_SAVE_SERIALIZABLE_TYPE(enumerator, meta::is_enum<T>())
{
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    return archive & value;
}

CONDITIONAL_LOAD_SERIALIZABLE_TYPE(enumerator, meta::is_enum<T>())
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff = 0;

    archive & buff;

    enumerator = static_cast<T>(buff);

    return archive;
}

CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(array, meta::is_array<T>())
{
    compress::zip(archive, array);

    return archive;
}

CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(pointer, meta::is_serializable_raw_pointer<T>())
{
    auto success = detail::is_refer(archive, pointer); // read/write refer info
    if (success) strict(archive, pointer);

    return archive;
}

} // inline namespace common

} // namespace sifar

#endif // SIFAR_COMMON_HPP
