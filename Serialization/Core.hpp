#ifndef SERIALIZATION_CORE_HPP
#define SERIALIZATION_CORE_HPP

#include "Access.hpp"

#include "Registry.hpp"
#include "Hash.hpp"

#include "WriteArchive.hpp"
#include "ReadArchive.hpp"

#include "Scope.hpp"

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
