#ifndef SERIALIZATION_HASH_HPP
#define SERIALIZATION_HASH_HPP

#include <string> // string

#include "Detail/StaticString.hpp"

#define STATIC_HASH(text) static_hash(STATIC_STRING(text))

namespace serialization
{

enum class Word
{
    x32,
    x64
};

namespace detail
{

template <std::size_t FnvPrime, std::size_t OffsetBasis>
std::size_t fnv_1a(const std::string& text)
{
    std::size_t hash = OffsetBasis;
    for(auto it = text.begin(), end = text.end();
            it != end; ++it)
    {
        hash ^= *it;
        hash *= FnvPrime;
    }

    return hash;
}

template <std::size_t FnvPrime, std::size_t OffsetBasis,
          std::size_t hash = OffsetBasis>
constexpr std::size_t static_fnv_1a()
{
    return hash;
}

template <std::size_t FnvPrime, std::size_t OffsetBasis,
          std::size_t hash = OffsetBasis,
          char s, char... sn>
constexpr std::size_t static_fnv_1a()
{
    return static_fnv_1a<FnvPrime, OffsetBasis, (hash ^ s) * FnvPrime, sn...>();
}

} // namespace detail

template <Word word>
struct Hash;

template <Word word>
struct StaticHash;

template <>
struct Hash<Word::x32>
{
private:
    // For 32 bit machines:
    static constexpr std::size_t fnv_prime        = 16777619u;
    static constexpr std::size_t fnv_offset_basis = 2166136261u;

public:
    static std::size_t run(const std::string& text)
    {
        return detail::fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }
};

template <>
struct Hash<Word::x64>
{
private:
    // For 64 bit machines:
    static constexpr std::size_t fnv_prime        = 1099511628211u;
    static constexpr std::size_t fnv_offset_basis = 14695981039346656037u;

public:
    static std::size_t run(const std::string& text)
    {
        return detail::fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }
};

template <>
struct StaticHash<Word::x32>
{
private:
    // For 32 bit machines:
    static constexpr std::size_t fnv_prime        = 16777619u;
    static constexpr std::size_t fnv_offset_basis = 2166136261u;

public:
    template <char s, char... sn>
    static constexpr std::size_t static_run(static_string<s, sn...> text)
    {
        return detail::static_fnv_1a<fnv_prime, fnv_offset_basis, s, sn...>(text);
    }
};

template <>
struct StaticHash<Word::x64>
{
private:
    // For 64 bit machines:
    static constexpr std::size_t fnv_prime        = 1099511628211u;
    static constexpr std::size_t fnv_offset_basis = 14695981039346656037u;

public:
    template <char s, char... sn>
    static constexpr std::size_t static_run(static_string<s, sn...> text)
    {
        return detail::static_fnv_1a<fnv_prime, fnv_offset_basis, s, sn...>();
    }
};

template <Word word = Word::x64>
std::size_t hash(const std::string& text)
{
    return Hash<word>::run(text);
}

template <Word word = Word::x64, char s, char... sn>
constexpr std::size_t static_hash(static_string<s, sn...> text)
{
    return StaticHash<word>::static_run(text);
}

} // namepace serialization

#endif // SERIALIZATION_HASH_HPP
