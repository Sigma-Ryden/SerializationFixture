#ifndef SERIALIZATION_CORE_HPP
#define SERIALIZATION_CORE_HPP

#include <Serialization/Access.hpp>

#include <Serialization/Registry.hpp>
#include <Serialization/Hash.hpp>

#include <Serialization/WriteArchive.hpp>
#include <Serialization/ReadArchive.hpp>

#include <Serialization/Scope.hpp>

#define SERIALIZATION_LOAD(archive)                                                                     \
    template <class Archive>                                                                            \
    auto load(Archive& archive) -> Archive&                                                             \

#define SERIALIZATION_SAVE(archive)                                                                     \
    template <class Archive>                                                                            \
    auto save(Archive& archive) -> Archive&

#define SERIALIZATION_UNIFIED(archive)                                                                  \
    SERIALIZATION_LOAD(archive) { return this->save(archive); }                                         \
    SERIALIZATION_SAVE(archive)

#endif // SERIALIZATION_CORE_HPP