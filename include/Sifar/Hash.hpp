#ifndef SIFAR_HASH_HPP
#define SIFAR_HASH_HPP

#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace utility
{

enum class Word { x32, x64 };

namespace detail
{

template <Word word> struct word_type_impl;

template <> struct word_type_impl<Word::x32> { using type = let::u32; };
template <> struct word_type_impl<Word::x64> { using type = let::u64; };

} // namespace detail

template <Word word>
using WordType = typename detail::word_type_impl<word>::type;

template <typename T> struct WordTrait { static constexpr auto value = Word::x64; };

template <> struct WordTrait<let::u32> { static constexpr auto value = Word::x32; };
template <> struct WordTrait<let::u64> { static constexpr auto value = Word::x64; };

namespace detail
{

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

} // namespace detail

template <Word word>
struct Hash;

template <>
struct Hash<Word::x32>
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
struct Hash<Word::x64>
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

} // namespace utility

template <typename key_type = let::u64>
key_type hash(const char* text)
{
    using utility::Hash;
    using utility::WordTrait;

    return Hash<WordTrait<key_type>::value>::run(text);
}

template <typename key_type = let::u64>
constexpr key_type static_hash(const char* text) noexcept
{
    using utility::Hash;
    using utility::WordTrait;

    return Hash<WordTrait<key_type>::value>::static_run(text);
}

} // namepace sifar

#endif // SIFAR_HASH_HPP
