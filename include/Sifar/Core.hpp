#ifndef SIFAR_CORE_HPP
#define SIFAR_CORE_HPP

#ifndef SIFAR_SMART_DISABLE
    #define SIFAR_SMART // Use additional macro defs
#endif // SIFAR_SMART_DISABLE

#include <fstream> // ifstream, ofstream

#include <Sifar/Access.hpp>
#include <Sifar/Serializable.hpp>

#include <Sifar/FactoryTable.hpp>
#include <Sifar/Polymorphic.hpp>

#include <Sifar/ArchiveTrait.hpp>

#include <Sifar/Registry.hpp>
#include <Sifar/TypeRegistry.hpp>

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/ApplyFunctor.hpp>
#include <Sifar/DataTrack.hpp>

#include <Sifar/Hash.hpp>

#include <Sifar/Common.hpp>
#include <Sifar/Span.hpp>
#include <Sifar/Ref.hpp>

#define EXPORT_SERIALIZATION_ARCHIVE(archive_key, archive_type, ...)                                    \
    _EXPORT_SERIALIZATION_ARCHIVE_TRAIT_KEY(archive_key, archive_type, __VA_ARGS__)                     \
    _EXPORT_SERIALIZATION_ARCHIVE_TYPE_TRAIT(archive_type, __VA_ARGS__)

EXPORT_SERIALIZATION_ARCHIVE(0, ReadArchive, std::ifstream)
EXPORT_SERIALIZATION_ARCHIVE(0, WriteArchive, std::ofstream)

#endif // SIFAR_CORE_HPP
