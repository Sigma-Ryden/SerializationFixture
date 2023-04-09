#ifndef SF_CORE_HASH_HPP
#define SF_CORE_HASH_HPP

#include <typeinfo> // type_info

#include <SF/Core/TypeCore.hpp>

#include <SF/Detail/Meta.hpp>

#ifndef SF_STATIC_HASH_KEY_TYPE
    #define SF_STATIC_HASH_KEY_TYPE ::sf::let::u64
#endif // SF_STATIC_HASH_KEY_TYPE

#ifndef SF_STATIC_HASH
    #define SF_STATIC_HASH(string) ::sf::static_hash<SF_STATIC_HASH_KEY_TYPE>((string))
#endif // SF_STATIC_HASH

#ifndef SF_TYPE_HASH
    #define SF_TYPE_HASH(type_info) ::sf::type_hash(type_info)
#endif // SF_TYPE_HASH

namespace sf
{

namespace detail
{

enum class Word { x32, x64 }; // word size: x* - number of bits

template <Word word> struct word_type_implementation;

template <> struct word_type_implementation<Word::x32> { using type = let::u32; };
template <> struct word_type_implementation<Word::x64> { using type = let::u64; };

template <Word word>
using WordType = typename detail::word_type_implementation<word>::type;

template <typename T> struct WordTrait { static constexpr auto value = Word::x64; };

template <> struct WordTrait<let::u32> { static constexpr auto value = Word::x32; };
template <> struct WordTrait<let::u64> { static constexpr auto value = Word::x64; };

template <let::u64 FnvPrime, let::u64 OffsetBasis>
let::u64 fnv_1a(const char* text)
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

template <let::u64 FnvPrime, let::u64 OffsetBasis>
constexpr let::u64 static_fnv_1a(const char* text, let::u64 hash = OffsetBasis) noexcept
{
    return (*text == '\0')
           ? hash
           : static_fnv_1a<FnvPrime, OffsetBasis>(text + 1, (hash ^ (*text)) * FnvPrime);
}

template <detail::Word word>
struct Hash;

template <>
struct Hash<detail::Word::x32>
{
private:
    // For 32 bit machines:
    static constexpr let::u32 fnv_prime        = 16777619ull;
    static constexpr let::u32 fnv_offset_basis = 2166136261ull;

public:
    static let::u32 run(const char* text)
    {
        return detail::fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u32 static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }
};

template <>
struct Hash<detail::Word::x64>
{
private:
    // For 64 bit machines:
    static constexpr let::u64 fnv_prime        = 1099511628211ull;
    static constexpr let::u64 fnv_offset_basis = 14695981039346656037ull;

public:
    static let::u64 run(const char* text)
    {
        return detail::fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u64 static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<fnv_prime, fnv_offset_basis>(text);
    }
};

} // namespace detail

template <typename key_type = let::u64>
inline key_type hash(const char* text)
{
    using detail::Hash;
    using detail::WordTrait;

    return Hash<detail::WordTrait<key_type>::value>::run(text);
}

template <typename key_type = let::u64>
constexpr key_type static_hash(const char* text) noexcept
{
    using detail::Hash;
    using detail::WordTrait;

    return Hash<detail::WordTrait<key_type>::value>::static_run(text);
}

inline let::u64 type_hash(const std::type_info& type) noexcept
{
    // not portable implementation - will be changed
    return type.hash_code();
}

inline let::u64 hash_combine(let::u64 lhs, let::u64 rhs) noexcept
{
    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

} // namepace sf

#endif // SF_CORE_HASH_HPP