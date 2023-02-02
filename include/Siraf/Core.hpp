#ifndef SIRAF_CORE_HPP
#define SIRAF_CORE_HPP

#ifndef SIRAF_SMART_DISABLE
    #define SIRAF_SMART // Use additional macro defs
#endif // SIRAF_SMART_DISABLE

#ifndef SIRAF_DEFAULT_DISABLE
#include <fstream> // ifstream, ofstream
#endif // SIRAF_DEFAULT_DISABLE

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

#ifndef SIRAF_DEFAULT_DISABLE

EXPORT_SERIALIZATION_ARCHIVE(0, ReadArchive, std::ifstream, wrapper::IFileStream<std::ifstream>)
EXPORT_SERIALIZATION_ARCHIVE(1, ReadArchive, std::vector<unsigned char>, wrapper::IByteStream)

EXPORT_SERIALIZATION_ARCHIVE(0, WriteArchive, std::ofstream, wrapper::OFileStream<std::ofstream>)
EXPORT_SERIALIZATION_ARCHIVE(1, WriteArchive, std::vector<unsigned char>, wrapper::OByteStream)

#endif // SIRAF_DEFAULT_DISABLE

#endif // SIRAF_CORE_HPP
