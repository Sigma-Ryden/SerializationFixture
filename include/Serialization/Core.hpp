#ifndef SERIALIZATION_CORE_HPP
#define SERIALIZATION_CORE_HPP

#include <Serialization/Access.hpp>

#include <Serialization/Registry.hpp>
#include <Serialization/Hash.hpp>

#include <Serialization/WriteArchive.hpp>
#include <Serialization/ReadArchive.hpp>

#include <Serialization/Span.hpp>

#define SERIALIZATION_LOAD(archive)                                                                     \
    template <class Archive> void load(Archive& archive)

#define SERIALIZATION_SAVE(archive)                                                                     \
    template <class Archive> void save(Archive& archive)

#define SERIALIZATION_UNIFIED(archive)                                                                  \
    SERIALIZATION_LOAD(archive) { return this->save(archive); }                                         \
    SERIALIZATION_SAVE(archive)

#endif // SERIALIZATION_CORE_HPP
