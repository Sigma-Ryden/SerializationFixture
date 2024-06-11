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

EXPORT_SERIALIZATION_ARCHIVE(0, i, iarchive_t<wrapper::ibyte_stream_t<wrapper::byte_container_t>>)
EXPORT_SERIALIZATION_ARCHIVE(0, o, oarchive_t<wrapper::obyte_stream_t<wrapper::byte_container_t>>)

#ifndef SF_DEFAULT_DISABLE

#include <fstream> // ifstream, ofstream

EXPORT_SERIALIZATION_ARCHIVE(1, i, iarchive_t<wrapper::ifile_stream_t<std::ifstream>>)
EXPORT_SERIALIZATION_ARCHIVE(1, o, oarchive_t<wrapper::ofile_stream_t<std::ofstream>>)

#endif // SF_DEFAULT_DISABLE

#endif // SF_CORE_HPP
