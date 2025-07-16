#ifndef EIGHTSER_BINARY_HPP
#define EIGHTSER_BINARY_HPP

#include <memory> // addressof

#include <Eightser/Core/Serialization.hpp>
#include <Eightser/ApplyFunctor.hpp>

namespace eightser
{

template <class ArchiveType, typename SerializableType,
          EIGHTSER_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
void binary(ArchiveType& archive, SerializableType& data)
{
    archive.stream().call(std::addressof(data), sizeof(SerializableType));
}

namespace apply
{

template <typename SerializableType>
struct binary_functor_t : apply_functor_t
{
    SerializableType& data;

    binary_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { binary(archive, data); }
};

} // namespace apply

template <typename SerializableType>
apply::binary_functor_t<SerializableType> binary(SerializableType& object) noexcept { return { object }; }

} // namespace eightser

#endif // EIGHTSER_BINARY_HPP
