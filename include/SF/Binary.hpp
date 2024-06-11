#ifndef SF_BINARY_HPP
#define SF_BINARY_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/ApplyFunctor.hpp>

namespace sf
{

template <class Archive, typename T,
          SF_REQUIRE(meta::is_ioarchive<Archive>::value)>
void binary(Archive& archive, T& data)
{
    archive.stream().call(std::addressof(data), sizeof(T));
}

namespace apply
{

template <typename T>
struct binary_functor_t : apply_functor_t
{
    T& data;

    binary_functor_t(T& data) noexcept : data(data) {}

    template <class Archive>
    void operator() (Archive& archive) const { binary(archive, data); }
};

} // namespace apply

template <typename T> apply::binary_functor_t<T> binary(T& object) noexcept { return { object }; }

} // namespace sf

#endif // SF_BINARY_HPP
