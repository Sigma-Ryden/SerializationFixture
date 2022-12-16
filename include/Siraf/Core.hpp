#ifndef SIRAF_CORE_HPP
#define SIRAF_CORE_HPP

#ifndef SIRAF_SMART_DISABLE
    #define SIRAF_SMART // Use additional macro defs
#endif // SIRAF_SMART_DISABLE

#include <fstream> // ifstream, ofstream

#include <Siraf/Access.hpp>
#include <Siraf/Serializable.hpp>

#include <Siraf/Dynamic/FactoryTable.hpp>
#include <Siraf/Dynamic/Polymorphic.hpp>
#include <Siraf/Dynamic/Registry.hpp>

#include <Siraf/ArchiveTrait.hpp>

#include <Siraf/TypeRegistry.hpp>

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>

#include <Siraf/ApplyFunctor.hpp>
#include <Siraf/DataTrack.hpp>

#include <Siraf/Hash.hpp>
#include <Siraf/Common.hpp>

#include <Siraf/Config.hpp>

EXPORT_SERIALIZATION_ARCHIVE(0, ReadArchive, std::ifstream)
EXPORT_SERIALIZATION_ARCHIVE(0, WriteArchive, std::ofstream)

#endif // SIRAF_CORE_HPP
