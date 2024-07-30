#ifndef SF_BUILT_IN_VECTOR_HPP
#define SF_BUILT_IN_VECTOR_HPP

#include <type_traits> // true_type, false_type

#include <vector> // vector

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_vector : std::false_type {};
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, vector, ::sf::meta::is_std_vector<T>::value)
{
    ::sf::let::u64 size = vector.size();
    archive & size;

    ::sf::compress::zip(archive, vector);
}

CONDITIONAL_SERIALIZATION(load, vector, ::sf::meta::is_std_vector<T>::value)
{
    ::sf::let::u64 size{};
    archive & size;

    vector.resize(size);
    ::sf::compress::zip(archive, vector);
}

// slow impl
SERIALIZATION(save, vector, std::vector<bool>)
{
    ::sf::let::u64 size = vector.size();
    archive & size;

    for(auto item:vector)
    {
        bool boolean = item;
        archive & boolean;
    }
}

SERIALIZATION(load, vector, std::vector<bool>)
{
    ::sf::let::u64 size{};
    archive & size;

    vector.resize(size);

    for(auto item/*bit_reference*/:vector)
    {
        bool boolean{};
        archive & boolean;
        item = boolean;
    }
}

#endif // SF_BUILT_IN_VECTOR_HPP
