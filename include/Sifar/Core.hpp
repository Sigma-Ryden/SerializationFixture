#ifndef SIFAR_CORE_HPP
#define SIFAR_CORE_HPP

#include <Sifar/Access.hpp>
#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Registry.hpp>

#include <Sifar/Hash.hpp>

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/Span.hpp>

#define SERIALIZATION_LOAD(archive)                                                                     \
    template <class Archive> void load(Archive& archive)

#define SERIALIZATION_SAVE(archive)                                                                     \
    template <class Archive> void save(Archive& archive)

#define SERIALIZATION_UNIFIED(archive)                                                                  \
    SERIALIZATION_LOAD(archive) { return this->save(archive); }                                         \
    SERIALIZATION_SAVE(archive)

#endif // SIFAR_CORE_HPP
