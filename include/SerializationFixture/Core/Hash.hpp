#ifndef SF_CORE_HASH_HPP
#define SF_CORE_HASH_HPP

#include <cstdint> // uint32_t, uint64_t

#if !defined(SF_TYPE_HASH) || !defined(SF_EXPRESSION_HASH)
#include <typeinfo> // type_info
#endif // if

#include <functional> // hash

#ifndef SF_STRING_HASH
    #define SF_STRING_HASH(string) ::sf::static_hash<std::uint64_t>((string))
#endif // SF_STRING_HASH

#ifndef SF_TYPE_HASH
    #define SF_TYPE_HASH(type) (typeid(type).hash_code())
#endif // SF_TYPE_HASH

#ifndef SF_EXPRESSION_HASH
    #define SF_EXPRESSION_HASH(expression) (typeid(expression).hash_code())
#endif // SF_EXPRESSION_HASH

namespace sf
{

namespace detail
{

template <typename HashType, HashType FnvPrime, HashType OffsetBasis>
HashType fnv_1a(const char* text)
{
    auto hash = OffsetBasis;
    while (*text != '\0')
    {
        hash ^= *text;
        hash *= FnvPrime;

        ++text;
    }

    return hash;
}

template <typename HashType, HashType FnvPrime, HashType OffsetBasis>
constexpr HashType static_fnv_1a(char const* text, HashType hash = OffsetBasis) noexcept
{
    return (*text == '\0')
           ? hash
           : static_fnv_1a<HashType, FnvPrime, OffsetBasis>(text + 1, (hash ^ (*text)) * FnvPrime);
}

template <typename HashType>
struct hash_t;

template <>
struct hash_t<std::uint32_t>
{
private:
    static constexpr std::uint32_t fnv_prime = 16777619ull;
    static constexpr std::uint32_t fnv_offset_basis = 2166136261ull;

public:
    static std::uint32_t run(char const* text)
    {
        return detail::fnv_1a<std::uint32_t, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr std::uint32_t static_run(char const* text) noexcept
    {
        return detail::static_fnv_1a<std::uint32_t, fnv_prime, fnv_offset_basis>(text);
    }
};

template <>
struct hash_t<std::uint64_t>
{
private:
    static constexpr std::uint64_t fnv_prime = 1099511628211ull;
    static constexpr std::uint64_t fnv_offset_basis = 14695981039346656037ull;

public:
    static std::uint64_t run(const char* text)
    {
        return detail::fnv_1a<std::uint64_t, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr std::uint64_t static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<std::uint64_t, fnv_prime, fnv_offset_basis>(text);
    }
};

} // namespace detail

template <typename HashType = std::uint64_t>
HashType hash(const char* text)
{
    return detail::hash_t<HashType>::run(text);
}

template <typename HashType = std::uint64_t>
constexpr HashType static_hash(char const* text) noexcept
{
    return detail::hash_t<HashType>::static_run(text);
}

} // namepace sf

#endif // SF_CORE_HASH_HPP
