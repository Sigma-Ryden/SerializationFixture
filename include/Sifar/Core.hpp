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

EXPORT_SERIALIZATION_ARCHIVE(0, ReadArchive, std::ifstream)
EXPORT_SERIALIZATION_ARCHIVE(0, WriteArchive, std::ofstream)

#endif // SIFAR_CORE_HPP
