#ifndef SF_BINARY_HPP
#define SF_BINARY_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/ApplyFunctor.hpp>

namespace sf
{

template <class Archive, typename T,
          SFREQUIRE(meta::is_ioarchive<Archive>::value)>
void binary(Archive& archive, T& data)
{
    archive.stream().call(std::addressof(data), sizeof(T));
}

namespace apply
{

template <typename T>
struct BinaryFunctor : ApplyFunctor
{
    T& data;

    BinaryFunctor(T& data) noexcept : data(data) {}

    template <class Archive>
    void operator() (Archive& archive) const { binary(archive, data); }
};

} // namespace apply

template <typename T> apply::BinaryFunctor<T> binary(T& object) noexcept { return { object }; }

} // namespace sf

#endif // SF_BINARY_HPP
