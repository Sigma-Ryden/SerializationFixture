#ifndef SIRAF_CORE_HPP
#define SIRAF_CORE_HPP

#ifndef SIRAF_SMART_DISABLE
    #define SIRAF_SMART // Use additional macro defs
#endif // SIRAF_SMART_DISABLE

#include <Siraf/Core/Config.hpp>

#include <Siraf/Core/Access.hpp>
#include <Siraf/Core/TypeRegistry.hpp>
#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/Hash.hpp>

#include <Siraf/Dynamic/FactoryTable.hpp>
#include <Siraf/Dynamic/Polymorphic.hpp>
#include <Siraf/Dynamic/Registry.hpp>

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>

#include <Siraf/Common.hpp>

#include <Siraf/ApplyFunctor.hpp>
#include <Siraf/Hierarchy.hpp>
#include <Siraf/DataTrack.hpp>

#include <Siraf/Serializable.hpp>

EXPORT_SERIALIZATION_ARCHIVE(0, ReadArchive,
    wrapper::ByteContainer, wrapper::IByteStream<wrapper::ByteContainer>)

EXPORT_SERIALIZATION_ARCHIVE(0, WriteArchive,
    wrapper::ByteContainer, wrapper::OByteStream<wrapper::ByteContainer>)

#ifndef SIRAF_DEFAULT_DISABLE

#include <fstream> // ifstream, ofstream

EXPORT_SERIALIZATION_ARCHIVE(1, ReadArchive,
    std::ifstream, wrapper::IFileStream<std::ifstream>)

EXPORT_SERIALIZATION_ARCHIVE(1, WriteArchive,
    std::ofstream, wrapper::OFileStream<std::ofstream>)

#endif // SIRAF_DEFAULT_DISABLE

#endif // SIRAF_CORE_HPP
