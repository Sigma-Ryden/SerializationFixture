#ifndef SF_CORE_HASH_HPP
#define SF_CORE_HASH_HPP

#include <typeinfo> // type_info
#include <functional> // hash

#include <SF/Core/TypeCore.hpp>

#include <SF/Detail/Meta.hpp>

#ifndef SF_STRING_HASH
    #define SF_STRING_HASH(string) ::sf::static_hash<::sf::let::u64>((string))
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
struct hash_t<let::u32>
{
private:
    static constexpr let::u32 fnv_prime = 16777619ull;
    static constexpr let::u32 fnv_offset_basis = 2166136261ull;

public:
    static let::u32 run(char const* text)
    {
        return detail::fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u32 static_run(char const* text) noexcept
    {
        return detail::static_fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }
};

template <>
struct hash_t<let::u64>
{
private:
    static constexpr let::u64 fnv_prime = 1099511628211ull;
    static constexpr let::u64 fnv_offset_basis = 14695981039346656037ull;

public:
    static let::u64 run(const char* text)
    {
        return detail::fnv_1a<let::u64, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u64 static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<let::u64, fnv_prime, fnv_offset_basis>(text);
    }
};

} // namespace detail

template <typename HashType = let::u64>
HashType hash(const char* text)
{
    return detail::hash_t<HashType>::run(text);
}

template <typename HashType = let::u64>
constexpr HashType static_hash(char const* text) noexcept
{
    return detail::hash_t<HashType>::static_run(text);
}

namespace detail
{

template <typename HashType, typename ObjectType>
void hash_combine(HashType& seed, ObjectType const& object) noexcept
{
    auto hash = std::hash<ObjectType>{}(object) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<HashType>(hash);
}

} // namespace detail

} // namepace sf

#endif // SF_CORE_HASH_HPP
