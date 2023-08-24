#ifndef SF_CORE_HPP
#define SF_CORE_HPP

#include <SF/Core/Config.hpp>

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/Serialization.hpp>
#include <SF/Core/Hash.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/Dynamic/ExternRegistry.hpp>

#include <SF/OArchive.hpp>
#include <SF/IArchive.hpp>

#include <SF/Common.hpp>
#include <SF/Aggregate.hpp>
#include <SF/Union.hpp>

#include <SF/ApplyFunctor.hpp>
#include <SF/Hierarchy.hpp>
#include <SF/DataTrack.hpp>

#include <SF/Serializable.hpp>

EXPORT_SERIALIZATION_ARCHIVE(0, I, IArchive<wrapper::IByteStream<wrapper::ByteContainer>>)
EXPORT_SERIALIZATION_ARCHIVE(0, O, OArchive<wrapper::OByteStream<wrapper::ByteContainer>>)

#ifndef SF_DEFAULT_DISABLE

#include <fstream> // ifstream, ofstream

EXPORT_SERIALIZATION_ARCHIVE(1, I, IArchive<wrapper::IFileStream<std::ifstream>>)
EXPORT_SERIALIZATION_ARCHIVE(1, O, OArchive<wrapper::OFileStream<std::ofstream>>)

#endif // SF_DEFAULT_DISABLE

#endif // SF_CORE_HPP
