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
// TODO: simplify
enum class Word { x32, x64 }; // word size: x* - number of bits

template <Word word> struct word_type_impl;

template <> struct word_type_impl<Word::x32> { using type = let::u32; };
template <> struct word_type_impl<Word::x64> { using type = let::u64; };

template <Word word>
using word_t = typename detail::word_type_impl<word>::type;

template <typename T> struct word_traits { static constexpr auto value = Word::x64; };

template <> struct word_traits<let::u32> { static constexpr auto value = Word::x32; };
template <> struct word_traits<let::u64> { static constexpr auto value = Word::x64; };

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
constexpr HashType static_fnv_1a(const char* text, HashType hash = OffsetBasis) noexcept
{
    return (*text == '\0')
           ? hash
           : static_fnv_1a<HashType, FnvPrime, OffsetBasis>(text + 1, (hash ^ (*text)) * FnvPrime);
}

template <detail::Word word>
struct hash_t;

template <>
struct hash_t<detail::Word::x32>
{
private:
    // For 32 bit machines:
    static constexpr let::u32 fnv_prime        = 16777619ull;
    static constexpr let::u32 fnv_offset_basis = 2166136261ull;

public:
    static let::u32 run(const char* text)
    {
        return detail::fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u32 static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }
};

template <>
struct hash_t<detail::Word::x64>
{
private:
    // For 64 bit machines:
    static constexpr let::u64 fnv_prime        = 1099511628211ull;
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

template <typename KeyType = let::u64>
inline KeyType hash(const char* text)
{
    using detail::hash_t;
    using detail::word_traits;

    return hash_t<detail::word_traits<KeyType>::value>::run(text);
}

template <typename KeyType = let::u64>
constexpr KeyType static_hash(const char* text) noexcept
{
    using detail::hash_t;
    using detail::word_traits;

    return hash_t<detail::word_traits<KeyType>::value>::static_run(text);
}

namespace detail
{

template <typename HashType, typename T>
inline void hash_combine(HashType& seed, const T& object) noexcept
{
    auto hash = std::hash<T>{}(object) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<HashType>(hash);
}

} // namespace detail

} // namepace sf

#endif // SF_CORE_HASH_HPP
