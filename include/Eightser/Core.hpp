#ifndef EIGHTSER_CORE_HPP
#define EIGHTSER_CORE_HPP

#include <Eightser/Core/Config.hpp>

#include <Eightser/Core/Serialization.hpp>
#include <Eightser/Core/Hash.hpp>

#include <Eightser/Dynamic/InstantiableRegistry.hpp>

#include <Eightser/OArchive.hpp>
#include <Eightser/IArchive.hpp>

#include <Eightser/Common.hpp>
#include <Eightser/Aggregate.hpp>
#include <Eightser/Union.hpp>

#include <Eightser/ApplyFunctor.hpp>
#include <Eightser/HierarchyTrack.hpp>
#include <Eightser/DataTrack.hpp>

#include <Eightser/Serializable.hpp>

EXPORT_SERIALIZABLE_ARCHIVE(0, i, ::eightser::iarchive_t<::eightser::wrapper::ibyte_stream_t<>>)
EXPORT_SERIALIZABLE_ARCHIVE(0, o, ::eightser::oarchive_t<::eightser::wrapper::obyte_stream_t<>>)

#ifndef EIGHTSER_DEFAULT_DISABLE

EXPORT_SERIALIZABLE_ARCHIVE(1, i, ::eightser::iarchive_t<::eightser::wrapper::ifile_stream_t<>>)
EXPORT_SERIALIZABLE_ARCHIVE(1, o, ::eightser::oarchive_t<::eightser::wrapper::ofile_stream_t<>>)

#endif // EIGHTSER_DEFAULT_DISABLE

#endif // EIGHTSER_CORE_HPP
