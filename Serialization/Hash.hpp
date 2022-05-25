#ifndef SERIALIZATION_HASH_HPP
#define SERIALIZATION_HASH_HPP

#include <string> // string

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
std::size_t fnv_1a(const char* text)
{
    std::size_t hash = OffsetBasis;
    while(*text != '\0')
    {
        hash ^= *text;
        hash *= FnvPrime;

        ++text;
    }

    return hash;
}

template <std::size_t FnvPrime, std::size_t OffsetBasis>
constexpr std::size_t static_fnv_1a(const char* text, std::size_t hash = OffsetBasis)
{
    return (*text == '\0')
           ? hash
           : static_fnv_1a<FnvPrime, OffsetBasis>(text + 1, (hash ^ (*text)) * FnvPrime);
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
    static std::size_t run(const char* text)
    {
        return detail::fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr std::size_t static_run(const char* text)
    {
        return detail::static_fnv_1a<fnv_prime, fnv_offset_basis>(text);
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
    static std::size_t run(const char* text)
    {
        return detail::fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr std::size_t static_run(const char* text)
    {
        return detail::static_fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }
};

template <Word word = Word::x64>
std::size_t hash(const char* text)
{
    return Hash<word>::run(text);
}

template <Word word = Word::x64>
constexpr std::size_t static_hash(const char* text)
{
    return Hash<word>::static_run(text);
}

} // namepace serialization

#endif // SERIALIZATION_HASH_HPP
