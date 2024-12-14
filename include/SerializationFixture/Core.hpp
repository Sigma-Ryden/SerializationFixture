#ifndef SF_CORE_HPP
#define SF_CORE_HPP

#include <SerializationFixture/Core/Config.hpp>

#include <SerializationFixture/Core/Serialization.hpp>
#include <SerializationFixture/Core/Hash.hpp>

#include <SerializationFixture/Dynamic/InstantiableRegistry.hpp>

#include <SerializationFixture/OArchive.hpp>
#include <SerializationFixture/IArchive.hpp>

#include <SerializationFixture/Common.hpp>
#include <SerializationFixture/Aggregate.hpp>
#include <SerializationFixture/Union.hpp>

#include <SerializationFixture/ApplyFunctor.hpp>
#include <SerializationFixture/HierarchyTrack.hpp>
#include <SerializationFixture/DataTrack.hpp>

#include <SerializationFixture/Serializable.hpp>

EXPORT_SERIALIZATION_ARCHIVE(0, i, ::sf::iarchive_t<::sf::wrapper::ibyte_stream_t<>>)
EXPORT_SERIALIZATION_ARCHIVE(0, o, ::sf::oarchive_t<::sf::wrapper::obyte_stream_t<>>)

#ifndef SF_DEFAULT_DISABLE

EXPORT_SERIALIZATION_ARCHIVE(1, i, ::sf::iarchive_t<::sf::wrapper::ifile_stream_t<>>)
EXPORT_SERIALIZATION_ARCHIVE(1, o, ::sf::oarchive_t<::sf::wrapper::ofile_stream_t<>>)

#endif // SF_DEFAULT_DISABLE

#endif // SF_CORE_HPP
