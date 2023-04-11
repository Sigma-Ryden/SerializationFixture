#ifndef SF_CORE_HPP
#define SF_CORE_HPP

#include <SF/Core/Config.hpp>

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/Serialization.hpp>
#include <SF/Core/Hash.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/Dynamic/ExternRegistry.hpp>

#include <SF/WriteArchive.hpp>
#include <SF/ReadArchive.hpp>

#include <SF/Common.hpp>
#include <SF/Aggregate.hpp>

#include <SF/ApplyFunctor.hpp>
#include <SF/Hierarchy.hpp>
#include <SF/DataTrack.hpp>

#include <SF/Serializable.hpp>

EXPORT_SERIALIZATION_ARCHIVE(0, ReadArchive,
    wrapper::ByteContainer, wrapper::IByteStream<wrapper::ByteContainer>)

EXPORT_SERIALIZATION_ARCHIVE(0, WriteArchive,
    wrapper::ByteContainer, wrapper::OByteStream<wrapper::ByteContainer>)

#ifndef SF_DEFAULT_DISABLE

#include <fstream> // ifstream, ofstream

EXPORT_SERIALIZATION_ARCHIVE(1, ReadArchive,
    std::ifstream, wrapper::IFileStream<std::ifstream>)

EXPORT_SERIALIZATION_ARCHIVE(1, WriteArchive,
    std::ofstream, wrapper::OFileStream<std::ofstream>)

#endif // SF_DEFAULT_DISABLE

#endif // SF_CORE_HPP
