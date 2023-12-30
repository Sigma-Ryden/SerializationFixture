#ifndef SERIALIZATION_FIXTURE_HPP
#define SERIALIZATION_FIXTURE_HPP

namespace sf
{

namespace core
{

#ifndef SF_TYPE_PORTABLE_DISABLE

static_assert(sizeof(char) == 1, "Require character size.");

static_assert(sizeof(float) == 4, "Require floating point number size.");
static_assert(sizeof(double) == 8, "Require double precision floation point number size.");

static_assert(sizeof(int) == 4, "Require integer number size.");

#endif // SF_TYPE_PORTABLE_DISABLE

} // namespace core

} // namespace sf

#include <cstddef> // size_t

#include <type_traits>
// is_enum, is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#if __cplusplus >= 201703L
#include <any> // any
#endif // if

#include <memory> // shared_ptr
#include <array> // array

#include <tuple> // tuple_element

#define SFREQUIRE(...) ::sf::meta::require<(bool)(__VA_ARGS__)> = 0
#define SFWHEN(...) ::sf::meta::when<(bool)(__VA_ARGS__)>
#define SFVOID(...) ::sf::meta::to_void<decltype(__VA_ARGS__)>

namespace sf
{

namespace meta
{

template <typename... Tn> constexpr bool to_false() noexcept { return false; }

template <typename T, std::size_t I = 0>
auto declval() noexcept -> decltype(std::declval<T>()) { return std::declval<T>(); }

template <bool condition, typename T = void>
using when = typename std::enable_if<condition, T>::type;

template <bool condition>
using require = when<condition, int>;

template <std::size_t I>
using dispatch = std::integral_constant<std::size_t, I>;

template <bool condition>
using boolean = std::integral_constant<bool, condition>;

template <bool condition, typename if_true, typename if_false>
using conditional = typename std::conditional<condition, if_true, if_false>::type;

template <typename T>
using decay = typename std::decay<T>::type;

template <class...> struct all : std::true_type {};
template <class B1> struct all<B1> : B1 {};
template <class B1, class... Bn>
struct all<B1, Bn...> : conditional<bool(B1::value), all<Bn...>, B1> {};

template <class...> struct one : std::false_type {};
template <class B1> struct one<B1> : B1 {};
template <class B1, class... Bn>
struct one<B1, Bn...> : conditional<bool(B1::value), B1, one<Bn...>> {};

template <class B> struct negation : boolean<not bool(B::value)> {};

template <typename T, typename... Tn> struct is_same : all<std::is_same<T, Tn>...> {};

template <typename... Args>
using to_void = void;

namespace detail
{

template <typename T, std::size_t N>
struct remove_pointer_implementation
{
    using type = T;
};

template <typename T>
struct remove_pointer_implementation<T*, 1>
{
    using type = T;
};

template <typename T, std::size_t N>
struct remove_pointer_implementation<T*, N>
{
    using type = typename remove_pointer_implementation<T, N - 1>::type;
};

} // namespace detail

template <typename T, std::size_t N = 1>
using remove_pointer = typename detail::remove_pointer_implementation<T, N>::type;

namespace detail
{

template <typename T, typename = void> struct dereference_implementation { using type = T; };

template <typename T> struct dereference_implementation<T*> { using type = T; };
template <typename T> struct dereference_implementation<std::weak_ptr<T>> { using type = T; };
template <typename T> struct dereference_implementation<std::shared_ptr<T>> { using type = T; };
template <typename T> struct dereference_implementation<std::unique_ptr<T>> { using type = T; };

template <typename T> struct dereference_implementation<T, SFVOID(*std::declval<T>())>
    : std::remove_reference<decltype(*std::declval<T>())> {};

} // namespace detail

template <typename T>
using dereference = typename detail::dereference_implementation<T>::type;

namespace detail
{

template <typename T, std::size_t N>
struct pointer_implementation
{
    using type = typename pointer_implementation<T, N - 1>::type*;
};

template <typename T>
struct pointer_implementation<T, 0>
{
    using type = T;
};

} // namespace detail

template <typename T, std::size_t N = 1>
using pointer = typename detail::pointer_implementation<T, N>::type;

 // limited by template depth
template <std::size_t... I> struct index_sequence
{
    static constexpr std::size_t size() noexcept { return sizeof...(I); }
};

namespace detail
{

template <std::size_t I, std::size_t... In>
struct index_sequence_helper : index_sequence_helper<I - 1, I - 1, In...> {};

template <std::size_t... In>
struct index_sequence_helper<0, In...>
{
    using type = index_sequence<In...>;
};

} // namespace detail

template <std::size_t N>
using make_index_sequence = typename detail::index_sequence_helper<N>::type;

template <typename... Tn>
struct type_array
{
    template <std::size_t I>
    using type = typename std::tuple_element<I, std::tuple<Tn...>>::type;
};

// meta
template <typename T, typename enable = void>
struct is_complete : std::false_type {};

template <typename T>
struct is_complete<T, SFVOID(sizeof(T))> : std::true_type {};

template <typename From, typename To, typename enable = void>
struct can_static_cast : std::false_type {};

template <typename From, typename To>
struct can_static_cast<From, To,
    SFVOID( static_cast<To*>(std::declval<From*>()) )> : std::true_type {};

template <typename T> constexpr std::size_t pointer_count() noexcept
{
    return std::is_pointer<T>::value
         ? 1 + pointer_count<remove_pointer<T>>()
         : 0;
}

struct shared_type {};
struct raw_type {};

struct dummy_type
{
#if __cplusplus >= 201703L
    template <typename T, SFREQUIRE(not std::is_same<T, std::any>::value)> operator T();
#else
    template <typename T> operator T();
#endif // if
};

#if __cplusplus >= 201703L
namespace detail
{

template <class C, typename S = index_sequence<>, typename overload = void>
struct aggregate_size_implementation : S {};

template <class C, std::size_t... I>
struct aggregate_size_implementation<C, index_sequence<I...>,
    SFVOID(C{ declval<dummy_type>(), declval<dummy_type, I>()... })>
    : aggregate_size_implementation<C, index_sequence<I..., sizeof...(I)>> {};

} // namespace detail

template <class T, bool condition = std::is_aggregate<T>::value>
struct aggregate_size
{
    static constexpr auto value = detail::aggregate_size_implementation<T>::size();
};

template <class T>
struct aggregate_size<T, false>
{
    static constexpr auto value = 0;
};

#endif // if

namespace detail
{

template <class T, typename enable = void>
struct object_value_type
{
    using type = dummy_type;
};

template <class T>
struct object_value_type<T, to_void<typename T::value_type>>
{
    using type = typename T::value_type;
};

template <typename T>
struct array_value_type
{
    using type = dummy_type;
};

template <typename T>
struct array_value_type<T[]>
{
    using type = T;
};

template <typename T, std::size_t N>
struct array_value_type<T[N]>
{
    using type = T;
};

template <typename T, typename enable = void>
struct value_type_implementation
{
    using type = dummy_type;
};

template <typename T>
struct value_type_implementation<T, when<std::is_class<T>::value>>
{
    using type = typename object_value_type<T>::type;
};

template <typename T>
struct value_type_implementation<T, when<std::is_array<T>::value>>
{
    using type = typename array_value_type<T>::type;
};

} // namespace detail

template <typename T>
using value_type = typename detail::value_type_implementation<T>::type;

namespace detail
{

template <std::size_t I, typename... Bn> struct with_implementation : dispatch<I> {};

template <std::size_t I, typename B, typename... Bn> struct with_implementation<I, B, Bn...>
    : conditional<bool(B::value), with_implementation<I>, with_implementation<I + 1, Bn...>> {};

} // namespace detail

template <typename... Bn> struct with : detail::with_implementation<0, Bn...> {};

template <typename From, typename To> struct is_cast_allowed
    : one<can_static_cast<From, To>, std::is_convertible<From, To>> {};

namespace detail
{

template <typename> struct is_character_implementation : std::false_type {};

template <> struct is_character_implementation<char> : std::true_type {};
template <> struct is_character_implementation<signed char> : std::true_type {};
template <> struct is_character_implementation<unsigned char> : std::true_type {};

template <> struct is_character_implementation<wchar_t> : std::true_type {};
template <> struct is_character_implementation<char16_t> : std::true_type {};
template <> struct is_character_implementation<char32_t> : std::true_type {};

} // namespace detail

template <typename T>
struct is_character : detail::is_character_implementation<typename std::remove_cv<T>::type> {};

template <class T> struct is_compressible : std::is_arithmetic<value_type<T>> {};

template <class Derived, class Base, class... Base_n> struct is_derived_of
    : all<std::is_base_of<Base, Derived>,
          std::is_base_of<Base_n, Derived>...> {};

template <typename T> struct is_void : std::is_same<T, void> {};

template <typename> struct is_std_shared_ptr : std::false_type {};
template <typename T>
struct is_std_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <typename> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename T> struct is_shared_pointer : is_std_shared_ptr<T> {};
template <typename T> struct is_raw_pointer : std::is_pointer<T> {};

template <typename T> struct is_pointer : one<is_shared_pointer<T>, is_raw_pointer<T>> {};

template <typename T> struct is_pointer_to_polymorphic
    : all<is_pointer<T>, std::is_polymorphic<dereference<T>>> {};

template <typename T> struct is_void_pointer : all<is_pointer<T>, is_void<dereference<T>>> {};
template <typename T> struct is_null_pointer : std::is_same<T, std::nullptr_t> {};

template <typename> struct is_function_pointer : std::false_type {};
template <typename Ret, typename... Args>
struct is_function_pointer<Ret (*)(Args...)> : std::true_type {};

template <typename T> struct is_pointer_to_standard_layout :
    all<is_pointer<T>,
        negation<is_void_pointer<T>>,
        negation<is_pointer_to_polymorphic<T>>,
        negation<is_function_pointer<T>>,
        negation<is_null_pointer<T>>,
        negation<std::is_member_pointer<T>>> {};

template <typename T> struct is_serializable_pointer
    : one<is_pointer_to_standard_layout<T>, is_pointer_to_polymorphic<T>> {};

template <typename T> struct is_serializable_raw_pointer
    : all<is_raw_pointer<T>, is_serializable_pointer<T>> {};

template <typename T> struct is_serializable_shared_pointer
    : all<is_shared_pointer<T>, is_serializable_pointer<T>> {};

#if __cplusplus >= 201703L
template <typename T> struct is_aggregate
    : all<std::is_aggregate<T>, negation<is_std_array<T>>, negation<std::is_array<T>>> {};
#endif // if

template <typename T> struct is_unsupported :
    one<is_void_pointer<T>,
        is_function_pointer<T>,
        is_null_pointer<T>,
        std::is_function<T>,
        std::is_member_function_pointer<T>,
        std::is_member_object_pointer<T>,
        std::is_reference<T>> {};

} // namespace meta

} // namespace sf

#define CONDITIONAL_TYPE_REGISTRY(...)                                                                  \
    namespace sf { namespace core {                                                                     \
        template <typename T> struct TypeRegistry<T, SFWHEN(__VA_ARGS__)> : std::true_type {};          \
    }}

// you should use TYPE_REGISTRY before using EXTERN_SERIALIZATION e.t.
#define TYPE_REGISTRY(...)                                                                              \
    namespace sf { namespace core {                                                                     \
        template <> struct TypeRegistry<__VA_ARGS__> : std::true_type {};                               \
    }}

namespace sf
{

namespace core
{

template <typename T, typename enable = void>
struct TypeRegistry : std::false_type {};

} // namespace core

namespace meta
{

template <typename T> struct is_registered : core::TypeRegistry<T> {};

// use this function only for extern type registry check
template <typename T> struct is_registered_extern
#ifdef SF_TYPE_REGISTRY_DISABLE
    : std::true_type {};
#else
    : is_registered<T> {};
#endif // SF_TYPE_REGISTRY_DISABLE

template <typename T> struct is_serializable : all<negation<is_unsupported<T>>, is_registered<T>> {};

} // namespace meta

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_unsupported<T>::value)

namespace sf
{

namespace core
{

class IOArchive;

struct IArchiveType {};
struct OArchiveType {};

} // namespace core

namespace meta
{

template <class T> struct is_iarchive : std::is_base_of<core::IArchiveType, T> {};
template <class T> struct is_oarchive : std::is_base_of<core::OArchiveType, T> {};

template <class T> struct is_ioarchive : one<is_iarchive<T>, is_oarchive<T>> {};

template <class T> struct is_archive : std::is_base_of<core::IOArchive, T> {};

} // namespace meta

} // namespace sf

#include <cstdint> // int8_t, uint8_t, int32_t, uint32_t, int64_t, uint64_t

namespace sf
{

namespace let
{

using i8 = std::int8_t;
using u8 = std::uint8_t;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i64 = std::int64_t;
using u64 = std::uint64_t;

} // namespace let

} // namespace sf

#ifndef SF_ARCHIVE_TRAIT_MAX_KEY_SIZE
    #define SF_ARCHIVE_TRAIT_MAX_KEY_SIZE 4
#endif // SF_ARCHIVE_MAX_TRAIT_KEY

#define SERIALIZATION_ARCHIVE(...)                                                                      \
    virtual key_type trait() const noexcept override {                                                  \
        return ::sf::core::ArchiveTraitKey<__VA_ARGS__>::key;                                           \
    }

namespace sf
{

namespace core
{

struct ArchiveTrait
{
    using key_type = let::u8;

    static constexpr auto base_key = key_type(-1);
    static constexpr auto max_key = key_type(SF_ARCHIVE_TRAIT_MAX_KEY_SIZE);
};

template <class Archive> struct ArchiveTraitKey
{
    static constexpr auto key = ArchiveTrait::base_key;
};

class IOArchive
{
public:
    using key_type = ArchiveTrait::key_type;

protected:
    virtual ~IOArchive() = default;

public:
    virtual key_type trait() const noexcept
    {
        return ArchiveTraitKey<IOArchive>::key;
    }
};

} // namespace core

} // namespace sf

#include <typeinfo> // type_info

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
        return detail::fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u32 static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
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
        return detail::fnv_1a<let::u64, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u64 static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<let::u64, fnv_prime, fnv_offset_basis>(text);
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

inline std::size_t type_hash(const std::type_info& type) noexcept
{
    // not portable implementation - will be changed
    return type.hash_code();
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

#define EXPORT_INSTANTIABLE_KEY(unique_name, ...)                                                       \
    namespace sf { namespace core {                                                                     \
        template <> struct InstantiableTraitKey<__VA_ARGS__> {                                          \
            static constexpr auto key = SF_STATIC_HASH(unique_name);                                    \
        };                                                                                              \
    }}

#define EXPORT_INSTANTIABLE(...)                                                                        \
    EXPORT_INSTANTIABLE_KEY(#__VA_ARGS__, __VA_ARGS__)

namespace sf
{

namespace core
{

struct InstantiableTrait
{
    using key_type = SF_STATIC_HASH_KEY_TYPE;
    
    static constexpr auto base_key = key_type(-1);
};

template <class T>
struct InstantiableTraitKey
{
    static constexpr auto key = InstantiableTrait::base_key;
};

} // namespace core

} // namespace sf

#define SERIALIZATION(mode, ...)                                                                        \
    template <>                                                                                         \
    struct Serialization::mode<__VA_ARGS__> {                                                           \
        template <class Archive> mode(Archive&, __VA_ARGS__&);                                          \
    };                                                                                                  \
    template <class Archive>                                                                            \
    Serialization::mode<__VA_ARGS__>::mode(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <class T>                                                                                  \
    struct Serialization::mode<T, SFWHEN(__VA_ARGS__)> {                                                \
        template <class Archive> mode(Archive&, T&);                                                    \
    };                                                                                                  \
    template <class T> template <class Archive>                                                         \
    Serialization::mode<T, SFWHEN(__VA_ARGS__)>::mode(Archive& archive, T& self)

// Allow to hide implementationementation to translation unit, and declare interface in header
#define SERIALIZATION_INTERFACE(mode, ...)                                                              \
    template <>                                                                                         \
    struct Serialization::mode<__VA_ARGS__> {                                                           \
        mode(::sf::core::IOArchive&, __VA_ARGS__&);                                                     \
    };

#define SERIALIZATION_IMPLEMENTATION(mode, ...)                                                         \
    Serialization::mode<__VA_ARGS__>::mode(::sf::core::IOArchive& archive, __VA_ARGS__& self)

// should be in global namespace
class Serialization
{
public:
    template <class T, typename enable = void> struct SaveLoad;
    template <class T, typename enable = void> struct Save;
    template <class T, typename enable = void> struct Load;

public:
    template <class T> struct is_save_class : sf::meta::is_complete<Save<T>> {};
    template <class T> struct is_load_class : sf::meta::is_complete<Load<T>> {};
    template <class T> struct is_saveload_class : sf::meta::is_complete<SaveLoad<T>> {};

public:
    template <class T> struct has_save_mode : sf::meta::one<is_save_class<T>, is_saveload_class<T>> {};
    template <class T> struct has_load_mode : sf::meta::one<is_load_class<T>, is_saveload_class<T>> {};

public:
    template <class Archive, class T> static void call(Archive& archive, T& self)
    {
        throw "The 'T' type cannot be saved/loaded."; // default implementation
    }

private:
    template <class T>
    struct SaveModeMeta
    {
        static constexpr auto index = sf::meta::with<is_save_class<T>, is_saveload_class<T>>::value;

        using mode_array = sf::meta::type_array<Save<T>, SaveLoad<T>, Serialization>;
        using mode = typename mode_array::template type<index>;
    };

    template <class T>
    struct LoadModeMeta
    {
        static constexpr auto index = sf::meta::with<is_load_class<T>, is_saveload_class<T>>::value;

        using mode_array = sf::meta::type_array<Load<T>, SaveLoad<T>, Serialization>;
        using mode = typename mode_array::template type<index>;
    };

public:
    template <class T> using SaveMode = typename SaveModeMeta<T>::mode;
    template <class T> using LoadMode = typename LoadModeMeta<T>::mode;

public:
    template <class T, typename = void> struct has_static_trait : std::false_type {};
    template <class T> struct has_static_trait<T, SFVOID(&T::__static_trait)> : std::true_type {};

    template <class T, typename = void> struct has_trait : std::false_type {};
    template <class T> struct has_trait<T, SFVOID(&T::__trait)> : std::true_type {};

public:
    template <class T> struct has_inner_trait
    {
        static constexpr bool value = has_static_trait<T>::value and has_trait<T>::value; // delay access
    };

private:
    template <typename From, typename To, typename = void> struct can_static_cast : std::false_type {};
    template <typename From, typename To>
    struct can_static_cast<From, To, SFVOID(static_cast<To>(std::declval<From>()))> : std::true_type {};

    template <typename From, typename To, typename = void> struct can_cast : std::false_type {};
    template <typename From, typename To>
    struct can_cast<From, To, SFVOID(std::declval<void(&)(To)>()(std::declval<From>()))> : std::true_type {};

    template <typename T, typename = void> struct is_returnable : std::false_type {};
    template <typename T>
    struct is_returnable<T, SFVOID(static_cast<T(*)()>(nullptr))> : std::true_type {};

public:
    template <class From, class To> struct is_cast_allowed
        : sf::meta::one<sf::meta::all<is_returnable<To>, can_cast<From, To>>,
                        sf::meta::is_same<void, From, To>> {};

    template <class From, class To> struct is_pointer_cast_allowed
        : sf::meta::one<is_cast_allowed<From*, To*>, sf::meta::is_void<From>> {};

public:
    template <class Base, class Derived> struct is_virtual_base_of
        : sf::meta::all<std::is_base_of<Base, Derived>,
                        sf::meta::negation<can_static_cast<Base*, Derived*>>> {};

public:
    using IOArchive = sf::core::IOArchive;
    using InstantiableTrait = sf::core::InstantiableTrait;

public:
    template <typename Archive, class T>
    static void save(Archive& archive, T& data)
    {
        SaveMode<T>(archive, data);
    }

    template <typename Archive, class T>
    static void load(Archive& archive, T& data)
    {
        LoadMode<T>(archive, data);
    }

    template <typename Base, class Archive, class Derived,
              SFREQUIRE(sf::meta::all<sf::meta::is_ioarchive<Archive>,
                                      std::is_base_of<Base, Derived>>::value)>
    static void serialize_base(Archive& archive, Derived& object)
    {
        archive & static_cast<Base&>(object);
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>::value)>
    static InstantiableTrait::key_type trait(T& object)
    {
    #ifdef SF_EXTERN_RUNTIME_TRAIT
        return SF_EXTERN_RUNTIME_TRAIT(object);
    #else
        return SF_TYPE_HASH(typeid(object));
    #endif // SF_EXTERN_RUNTIME_TRAIT
    }

    template <class T, SFREQUIRE(has_inner_trait<T>::value)>
    static InstantiableTrait::key_type trait(T& object) noexcept
    {
        return object.__trait();
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>::value)>
    static InstantiableTrait::key_type static_trait() noexcept
    {
    #ifdef SF_EXTERN_TRAIT
        return SF_EXTERN_TRAIT(T);
    #else
        return SF_TYPE_HASH(typeid(T));
    #endif // SF_EXTERN_TRAIT
    }

    template <class T, SFREQUIRE(has_inner_trait<T>::value)>
    static constexpr InstantiableTrait::key_type static_trait() noexcept
    {
        return T::__static_trait();
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>::value)>
    static InstantiableTrait::key_type trait() noexcept
    {
        constexpr auto trait_key = sf::core::InstantiableTraitKey<T>::key;

        static_assert(trait_key == InstantiableTrait::base_key,
            "Export instantiable trait is not allowed using typeid.");

        return static_trait<T>();
    }

    template <class T, SFREQUIRE(has_inner_trait<T>::value)>
#ifdef SF_EXPORT_INSTANTIABLE_DISABLE
    static constexpr InstantiableTraitBase::key_type trait() noexcept
#else
    static InstantiableTrait::key_type trait() noexcept
#endif // SF_EXPORT_INSTANTIABLE_DISABLE
    {
        constexpr auto trait_key = sf::core::InstantiableTraitKey<T>::key;

        return trait_key == InstantiableTrait::base_key
             ? static_trait<T>()
             : trait_key;
    }
};

#include <unordered_map> // unordered_map

// You can include given file to other dir level

namespace sf
{

namespace meta
{

template <typename T> struct is_memory_shared : std::is_same<T, shared_type> {};
template <typename T> struct is_memory_raw : std::is_same<T, raw_type> {};

template <typename T> struct is_memory : one<is_memory_shared<T>, is_memory_raw<T>> {};

} // namespace meta

class Memory
{
public:
    using Shared = meta::shared_type;
    using Raw = meta::raw_type;

    template <typename T>
    using shared_ptr = std::shared_ptr<T>;

    template <typename T>
    using raw_ptr = T*;

public:
    template <typename T>
    struct ptr_trait
    {
        using item = std::nullptr_t;
    };

    template <typename T>
    struct ptr_trait<std::shared_ptr<T>>
    {
        using trait    = Shared;

        template <typename U>
        using wrapper  = shared_ptr<U>;

        using type     = std::shared_ptr<T>;
        using item     = T;

        using void_ptr = std::shared_ptr<void>;
    };

    template <typename T>
    struct ptr_trait<T*>
    {
        using trait    = Raw;

        template <typename U>
        using wrapper  = raw_ptr<U>;

        using type     = T*;
        using item     = T;

        using void_ptr = void*;
    };

    template <typename T>
    struct ptr_trait<std::weak_ptr<T>>
    {
        using type     = std::weak_ptr<T>;
        using item     = T;
    };

    template <typename T>
    struct ptr_trait<std::unique_ptr<T>>
    {
        using type     = std::unique_ptr<T>;
        using item     = T;
    };

public:
    template <typename T>
    using void_ptr = typename ptr_trait<T>::void_ptr;

public:
    template <typename T>
    struct Factory
    {
        static std::shared_ptr<T> shared()
        {
            return std::make_shared<T>();
        }

        static T* raw()
        {
            return new T{};
        }
    };

private:
    template <class From, class To> struct is_pointer_cast_allowed
        : ::Serialization::is_pointer_cast_allowed<From, To> {};

public:
    template <typename To, typename T,
              SFREQUIRE(meta::is_shared_pointer<T>::value)>
    static shared_ptr<To> dynamic_pointer_cast(const T& pointer)
    {
        auto address = dynamic_pointer_cast<To>(pointer.get());
        return address == nullptr ? shared_ptr<To>() : shared_ptr<To>(pointer, address);
    }

    template <typename To, typename T,
              SFREQUIRE(meta::is_raw_pointer<T>::value)>
    static raw_ptr<To> dynamic_pointer_cast(const T& pointer)
    {
        return dynamic_cast<raw_ptr<To>>(pointer);
    }

    template <typename To, typename T,
              typename Trait = ptr_trait<T>,
              SFREQUIRE(meta::one<meta::is_null_pointer<T>,
                                  meta::all<meta::is_pointer<T>,
                                            meta::negation<is_pointer_cast_allowed<typename Trait::item, To>>>>::value)>
    static typename Trait::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
    {
        return nullptr;
    }

    template <typename To, typename T,
              SFREQUIRE(meta::all<meta::is_shared_pointer<T>,
                                  is_pointer_cast_allowed<typename ptr_trait<T>::item, To>>::value)>
    static shared_ptr<To> static_pointer_cast(const T& pointer) noexcept
    {
        auto address = static_pointer_cast<To>(pointer.get());
        return shared_ptr<To>(pointer, address);
    }

    template <typename To, typename T,
              SFREQUIRE(meta::all<meta::is_raw_pointer<T>,
                                  is_pointer_cast_allowed<typename ptr_trait<T>::item, To>>::value)>
    static raw_ptr<To> static_pointer_cast(const T& pointer) noexcept
    {
        return static_cast<raw_ptr<To>>(pointer);
    }

    template <typename To, typename From, typename T,
              typename Trait = ptr_trait<T>,
              SFREQUIRE(meta::all<meta::is_pointer<T>,
                                  meta::negation<is_pointer_cast_allowed<From, To>>>::value)>
    static typename Trait::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
    {
        return nullptr;
    }

    template <typename To, typename From, typename T,
              typename Trait = ptr_trait<T>,
              SFREQUIRE(meta::all<meta::is_pointer<T>,
                                  is_pointer_cast_allowed<typename Trait::item, From>,
                                  is_pointer_cast_allowed<From, To>>::value)>
    static typename Trait::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
    {
        return static_pointer_cast<To>(static_pointer_cast<From>(pointer));
    }

public:
    template <typename T,
              SFREQUIRE(meta::all<meta::is_pointer<T>,
                                  meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
    static void_ptr<T> pure(const T& pointer) noexcept
    {
        return static_pointer_cast<void>(pointer);
    }

    template <typename T,
              SFREQUIRE(meta::all<meta::is_pointer<T>,
                                  meta::is_pointer_to_polymorphic<T>>::value)>
    static void_ptr<T> pure(const T& pointer_to_polymorphic)
    {
        return dynamic_pointer_cast<void>(pointer_to_polymorphic);
    }

    static raw_ptr<void> pure(std::nullptr_t pointer) noexcept { return nullptr; }

    template <typename dT, typename T,
              SFREQUIRE(meta::is_pointer<T>::value)>
    static void assign(T& pointer, const void_ptr<T>& address) noexcept
    {
        pointer = static_pointer_cast<dT>(address);
    }

public:
    template <typename To, typename From = To, typename TraitType,
              SFREQUIRE(meta::all<meta::is_memory<TraitType>,
                                  meta::one<meta::negation<is_pointer_cast_allowed<From, To>>,
                                            std::is_abstract<From>>>::value)>
    static std::nullptr_t allocate() noexcept
    {
        return nullptr;
    }

    template <typename To, typename From = To, typename TraitType,
              SFREQUIRE(meta::all<meta::negation<std::is_abstract<From>>,
                                  meta::is_memory_shared<TraitType>,
                                  is_pointer_cast_allowed<From, To>>::value)>
    static shared_ptr<To> allocate()
    {
        auto instance = Factory<From>::shared();
        return static_pointer_cast<To>(instance);
    }

    template <typename To, typename From = To, typename TraitType,
              SFREQUIRE(meta::all<meta::negation<std::is_abstract<From>>,
                                  meta::is_memory_raw<TraitType>,
                                  is_pointer_cast_allowed<From, To>>::value)>
    static raw_ptr<To> allocate()
    {
        auto instance = Factory<From>::raw();
        return static_pointer_cast<To>(instance);
    }

    template <typename To, typename From = To>
    static shared_ptr<To> allocate_shared()
    {
        return allocate<To, From, Shared>();
    }

    template <typename To, typename From = To>
    static raw_ptr<To> allocate_raw()
    {
        return allocate<To, From, Raw>();
    }

    template <typename To, typename From = To, typename T,
              SFREQUIRE(meta::is_pointer<T>::value)>
    static void allocate(T& pointer)
    {
        pointer = allocate<To, From, typename ptr_trait<T>::trait>();
    }

public:
    template <typename T, typename dT = typename ptr_trait<T>::item,
              SFREQUIRE(meta::is_raw_pointer<T>::value)>
    static raw_ptr<dT> raw(const T& pointer) { return pointer; }

    template <typename T, typename dT = typename ptr_trait<T>::item,
              SFREQUIRE(meta::is_shared_pointer<T>::value)>
    static raw_ptr<dT> raw(const T& pointer) { return pointer.get(); }

public:
    template <typename ByteType = char, typename T>
    static const ByteType* const_byte_cast(T* data) noexcept
    {
        return reinterpret_cast<const ByteType*>(data);
    }

    template <typename ByteType = char, typename T>
    static ByteType* byte_cast(T* data) noexcept
    {
        return reinterpret_cast<ByteType*>(data);
    }
};

} // namespace sf

#define EXPORT_SERIALIZATION_ARCHIVE(archive_key, archive_type, ...)                                    \
    namespace sf { namespace core {                                                                     \
        template <> struct ArchiveTraitKey<__VA_ARGS__> {                                               \
            static constexpr IOArchive::key_type key = archive_key;                                     \
        };                                                                                              \
        template <> struct archive_type##ArchiveTrait<ArchiveTraitKey<__VA_ARGS__>::key> {              \
            using type = __VA_ARGS__;                                                                   \
        };                                                                                              \
    }}

namespace sf
{

namespace core
{

template <IOArchive::key_type I> struct IArchiveTrait { using type = IOArchive; };
template <IOArchive::key_type I> struct OArchiveTrait { using type = IOArchive; };

} // namespace core

} // namespace sf

namespace sf
{

namespace core
{

class PolymorphicArchive
{
public:
    using Archive  = IOArchive;
    using key_type = IOArchive::key_type;

    static constexpr key_type max_key = ArchiveTrait::max_key;

public:
    template <class T> static void save(Archive& archive, T& data)
    {
        call<OArchiveTrait>(archive, data);
    }

    template <class T> static void load(Archive& archive, T& data)
    {
        call<IArchiveTrait>(archive, data);
    }

private:
    template <class Archive> struct is_valid_archive
        : meta::boolean<ArchiveTraitKey<Archive>::key != ArchiveTrait::base_key> {};

private:
    template <template <key_type> class ArchiveTrait,
              key_type Key, class T, SFREQUIRE(Key == max_key)>
    static void call(Archive& archive, T& data)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <key_type> class ArchiveTrait,
              key_type Key = 0, class T, SFREQUIRE(Key < max_key)>
    static void call(Archive& archive, T& data)
    {
        using DerivedArchive = typename ArchiveTrait<Key>::type;

        if (ArchiveTraitKey<DerivedArchive>::key == archive.trait())
            return try_call<DerivedArchive>(archive, data);

        call<ArchiveTrait, Key + 1>(archive, data);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(not is_valid_archive<DerivedArchive>::value)>
    static void try_call(Archive& archive, T& data)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(is_valid_archive<DerivedArchive>::value)>
    static void try_call(Archive& archive, T& data)
    {
        auto derived_archive = dynamic_cast<DerivedArchive*>(&archive);

    #ifdef SF_DEBUG
        if (derived_archive == nullptr)
            throw "The read/write archive was registered incorrect.";
    #endif // SF_DEBUG

        proccess(*derived_archive, data);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(meta::all<meta::is_oarchive<DerivedArchive>,
                                  ::Serialization::has_save_mode<T>>::value)>
    static void proccess(DerivedArchive& archive, T& object)
    {
        ::Serialization::save(archive, object);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(meta::all<meta::is_iarchive<DerivedArchive>,
                                  ::Serialization::has_load_mode<T>>::value)>
    static void proccess(DerivedArchive& archive, T& object)
    {
        ::Serialization::load(archive, object);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(meta::all<meta::is_ioarchive<DerivedArchive>,
                                  meta::negation<::Serialization::has_save_mode<T>>,
                                  meta::negation<::Serialization::has_load_mode<T>>>::value)>
    static void proccess(DerivedArchive& archive, T& data)
    {
        process_data(archive, data);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(meta::is_registered_extern<T>::value)>
    static void process_data(DerivedArchive& archive, T& data)
    {
        archive & data;
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(not meta::is_registered_extern<T>::value)>
    static void process_data(DerivedArchive& archive, T& data)
    {
        throw "The 'T' type is unregistered.";
    }
};

template <class Archive, typename T,
          SFREQUIRE(meta::is_archive<Archive>::value)>
Archive& operator<< (Archive& archive, T&& data)
{
    PolymorphicArchive::save(archive, data);
    return archive;
}

template <class Archive, typename T,
          SFREQUIRE(meta::is_archive<Archive>::value)>
Archive& operator>> (Archive& archive, T&& data)
{
    PolymorphicArchive::load(archive, data);
    return archive;
}

} // namespace core

} // namespace sf

// By default library will use Instantiable type for general instancing,
// if you want to specify behaviour, just define own INSTANTIABLE_TYPE
// Note that: any defined INSTANTIABLE_TYPE must have virtual destructor
#ifndef INSTANTIABLE_TYPE
    #define INSTANTIABLE_TYPE ::sf::Instantiable
#endif // INSTANTIABLE_TYPE

namespace sf
{

struct Instantiable { virtual ~Instantiable() = default; };

} // namespace sf

// Auto instantiable type registration
#define SERIALIZATION_FIXTURE(...)                                                                      \
    private:                                                                                            \
    ::sf::dynamic::InstantiableFixture<__VA_ARGS__> __fixture;                                          \
    public:

#define SERIALIZATION_TRAIT(...)                                                                        \
    private:                                                                                            \
    using __key_type = ::sf::core::InstantiableTrait::key_type;                                         \
    static constexpr __key_type __static_trait() noexcept { return SF_STATIC_HASH(#__VA_ARGS__); }      \
    virtual __key_type __trait() const noexcept { return ::Serialization::trait<__VA_ARGS__>(); }       \
    public:

namespace sf
{

namespace dynamic
{

class InstantiableRegistry
{
public:
    using key_type          = core::InstantiableTrait::key_type;
    using instantiable_type = INSTANTIABLE_TYPE;
    using archive_type      = core::IOArchive;

private:
    template <typename ItemType>
    using InnerTable = std::unordered_map<key_type, ItemType>;

private:
    struct InstantiableProxy
    {
        instantiable_type* __instance = nullptr;

        std::shared_ptr<instantiable_type>(*__shared)() = nullptr;
        std::shared_ptr<instantiable_type>(*__cast_shared)(std::shared_ptr<void>) = nullptr;

        instantiable_type*(*__raw)() = nullptr;
        instantiable_type*(*__cast_raw)(void*) = nullptr;

        void(*__save)(archive_type&, instantiable_type*) = nullptr;
        void(*__load)(archive_type&, instantiable_type*) = nullptr;
    };

private:
    InnerTable<InstantiableProxy> registry_;

private:

    InstantiableRegistry() : registry_() {}

    ~InstantiableRegistry()
    {
        for (const auto& pair : registry_) delete pair.second.__instance;
    }

    InstantiableRegistry(const InstantiableRegistry&) = delete;
    InstantiableRegistry& operator=(const InstantiableRegistry&) = delete;

public:
    static InstantiableRegistry& instance() noexcept
    {
        static InstantiableRegistry self;
        return self;
    }

public:
    // we should use external check
    template <typename dT> struct is_instantiable : meta::is_cast_allowed<dT*, instantiable_type*> {};

public:
    template <class T, SFREQUIRE(not is_instantiable<T>::value)>
    void update(key_type key)
    {
        throw "The polymorphic 'T' type is not convertible to 'Instantiable'.";
    }

    template <class T, SFREQUIRE(is_instantiable<T>::value)>
    void update(key_type key)
    {
        if (is_registered(key)) return;

        InstantiableProxy proxy;

        proxy.__instance = Memory::allocate_raw<instantiable_type, T>();

        proxy.__shared = [] { return Memory::allocate_shared<instantiable_type, T>(); };

        proxy.__cast_shared = [](std::shared_ptr<void> address)
        {
            return Memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.__raw = [] { return Memory::allocate_raw<instantiable_type, T>(); };

        proxy.__cast_raw = [](void* address)
        {
            return Memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.__save = [](archive_type& archive, instantiable_type* instance)
        {
            archive << *Memory::dynamic_pointer_cast<T>(instance);
        };

        proxy.__load = [](archive_type& archive, instantiable_type* instance)
        {
            archive >> *Memory::dynamic_pointer_cast<T>(instance);
        };

        registry_.emplace(key, proxy);
    }

    template <typename TraitType,
              SFREQUIRE(meta::is_memory_shared<TraitType>::value)>
    std::shared_ptr<instantiable_type> clone(key_type key)
    {
        return registry(key).__shared();
    }

    template <typename TraitType,
              SFREQUIRE(meta::is_memory_raw<TraitType>::value)>
    instantiable_type* clone(key_type key)
    {
        return registry(key).__raw();
    }

    std::shared_ptr<instantiable_type> cast(std::shared_ptr<void> address, key_type key)
    {
        return registry(key).__cast_shared(address);
    }

    instantiable_type* cast(void* address, key_type key)
    {
        return registry(key).__cast_raw(address);
    }

    template <typename Pointer, typename T = sf::meta::dereference<Pointer>,
              SFREQUIRE(::Serialization::has_save_mode<T>::value)>
    void save(archive_type& archive, Pointer& pointer)
    {
        auto key = ::Serialization::trait(*pointer);
        registry(key).__save(archive, pointer);
    }

    template <typename Pointer, typename T = sf::meta::dereference<Pointer>,
              SFREQUIRE(::Serialization::has_load_mode<T>::value)>
    void load(archive_type& archive, Pointer& pointer)
    {
        auto key = ::Serialization::trait(*pointer);
        registry(key).__load(archive, pointer);
    }

public:
    bool is_registered(key_type key)
    {
        return registry_.find(key) != registry_.end();
    }

#ifndef SF_REGISTRY_ACCESS
private:
#endif // SF_REGISTRY_ACCESS
    InstantiableProxy& registry(key_type key)
    {
        // It happens if the class with the given key has not beed public inherited
        // from the instantiable class or not registered with fixture object.
        auto it = registry_.find(key);
        if (it == registry_.end())
            throw "The 'sf::dynamic::InstantiableRegistry' does not has instance with input key.";

        return it->second;
    }
};

template <class T>
class InstantiableFixture
{
private:
    static bool lock_;

public:
    InstantiableFixture() { call<T>(); }

public:
    template <typename dT = T,
              SFREQUIRE(InstantiableRegistry::is_instantiable<dT>::value)>
    static void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = InstantiableRegistry::instance();

        auto key = ::Serialization::template trait<T>();
    #ifdef SF_DEBUG
        if (registry.is_registered(key))
            throw "The 'sf::dynamic::InstantiableRegistry' must contains instance with unique key.";
    #endif // SF_DEBUG

        registry.update<T>(key);
    }

    template <typename dT = T,
              SFREQUIRE(not InstantiableRegistry::is_instantiable<dT>::value)>
    static void call() noexcept { /*pass*/ }
};

template <class T>
bool InstantiableFixture<T>::lock_ = false;

} // namespace dynamic

} // namespace sf

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

namespace sf
{

namespace dynamic
{

class AnyRegistry
{
public:
    using archive_type = core::IOArchive;

private:
    struct AnyProxy
    {
        // we use raw function ptr instead std::function to reach perfomance
        void(*__save)(archive_type&, std::any&) = nullptr;
        void(*__load)(archive_type&, std::any&) = nullptr;
    };

private:
    using InnerTable = std::unordered_map<let::u64, AnyProxy>;

private:
    InnerTable registry_;

private:
    AnyRegistry() : registry_() {}

    AnyRegistry(const AnyRegistry&) = delete;
    AnyRegistry& operator=(const AnyRegistry&) = delete;

public:
    static AnyRegistry& instance() noexcept
    {
        static AnyRegistry self;
        return self;
    }

    template <typename T> void update(let::u64 hash)
    {
        if (is_registered(hash)) return;

        AnyProxy proxy;

        proxy.__save = [](archive_type& archive, std::any& any)
        {
            archive << std::any_cast<T&>(any);
        };

        proxy.__load = [](archive_type& archive, std::any& any)
        {
            any.emplace<T>();
            archive >> std::any_cast<T&>(any);
        };

        registry_.emplace(hash, proxy);
    }

public:
    void save(archive_type& archive, std::any& any, let::u64 hash)
    {
        registry(hash).__save(archive, any);
    }

    void load(archive_type& archive, std::any& any, let::u64 hash)
    {
        registry(hash).__load(archive, any);
    }

public:
    bool is_registered(let::u64 hash)
    {
        return registry_.find(hash) != registry_.end();
    }

#ifndef SF_REGISTRY_ACCESS
private:
#endif // SF_REGISTRY_ACCESS
    const AnyProxy& registry(let::u64 hash)
    {
        // It happens if the type not registered with fixture object.
        auto it = registry_.find(hash);
        if (it == registry_.end())
            throw "The 'sf::AnyRegistry' must registry type with specify hash code.";

        return it->second;
    }
};

template <typename T>
class AnyFixture
{
private:
    static bool lock_;

public:
    AnyFixture() { call(); }

public:
    static void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = AnyRegistry::instance();

        auto hash = SF_TYPE_HASH(typeid(T));
    #ifdef SF_DEBUG
        if (registry.is_registered(hash))
            throw "The 'sf::dynamic::AnyRegistry' must contains unique hashes.";
    #endif // SF_DEBUG

        registry.update<T>(hash);
    }
};

template <typename T>
bool AnyFixture<T>::lock_ = false;

} // namespace dynamic

} // namespace sf

#endif // if

namespace sf
{

namespace dynamic
{

class ExternRegistry
{
public:
    using key_type = core::InstantiableTrait::key_type;

public:
    template <class Archive, typename T,
              SFREQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static key_type save_key(Archive& archive, T& pointer)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto key = ::Serialization::trait(*pointer);
        archive & key;

        return key;
    }

    template <class Archive, typename T,
              SFREQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static key_type load_key(Archive& archive, T& pointer)
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        key_type key{};
        archive & key;

        return key;
    }

private:
    template <typename T> struct is_pointer_to_instantiable
        : meta::all<InstantiableRegistry::is_instantiable<meta::dereference<T>>,
                    meta::is_pointer_to_polymorphic<T>> {};

public:
    template <typename T, SFREQUIRE(is_pointer_to_instantiable<T>::value)>
    static void save(core::IOArchive& archive, T& pointer, key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto raw_pointer = Memory::raw(pointer);
        InstantiableRegistry::instance().save(archive, raw_pointer);
    }

    template <typename T, SFREQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static void load(core::IOArchive& archive, T& pointer, key_type key, Memory::void_ptr<T>& cache)
    {
        using TraitType = typename Memory::ptr_trait<T>::trait;
        using dT = meta::dereference<T>;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto& registry = InstantiableRegistry::instance();

        auto cloned = registry.clone<TraitType>(key);

        pointer = Memory::dynamic_pointer_cast<dT>(cloned);
        cache = Memory::pure(pointer);

        auto raw_pointer = Memory::raw(pointer);
        registry.load(archive, raw_pointer);
    }

    template <typename T, SFREQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static void assign(T& pointer, const Memory::void_ptr<T>& address, key_type key)
    {
        using dT = meta::dereference<T>;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto casted = InstantiableRegistry::instance().cast(address, key);
        pointer = Memory::dynamic_pointer_cast<dT>(casted);
    }
};

} // namespace dynamic

} // namespace sf

#include <utility> // forward

namespace sf
{

namespace tracking
{

using Shared = meta::shared_type;
using Raw = meta::raw_type;

template <typename T>
struct track_trait;

template <typename T>
struct track_trait<Memory::shared_ptr<T>>
{
    using type = tracking::Shared;
};

template <typename T>
struct track_trait<Memory::raw_ptr<T>>
{
    using type = tracking::Raw;
};

template <typename T>
struct reverse_trait;

template <>
struct reverse_trait<Shared>
{
    using type = Raw;
};

template <>
struct reverse_trait<Raw>
{
    using type = Shared;
};

} // namespace tracking

namespace meta
{

template <typename T> struct is_track_shared : std::is_same<T, tracking::Shared> {};
template <typename T> struct is_track_raw : std::is_same<T, tracking::Raw> {};

} // namespace meta

} // namespace sf

namespace sf
{

namespace detail
{

template <typename HashType = let::u64>
struct PairHash
{
    template <typename T1, typename T2>
    HashType operator() (const std::pair<T1, T2>& pair) const noexcept
    {
        HashType seed{};

        detail::hash_combine(seed, pair.first);
        detail::hash_combine(seed, pair.second);

        return seed;
    }
};

} // namespace detail

namespace tracking
{

struct Hierarchy {};

template <typename KeyType, typename TraitType = core::InstantiableTrait::key_type>
using HierarchyTrack = std::unordered_map<std::pair<KeyType, TraitType>, bool, detail::PairHash<TraitType>>;

} // namespace tracking

namespace meta
{

template <typename T> struct is_track_hierarchy : std::is_same<T, tracking::Hierarchy> {};

} // namespace meta

} // namespace sf

#include <vector> // vector

#ifndef SF_BYTE_STREAM_RESERVE_SIZE
    #define SF_BYTE_STREAM_RESERVE_SIZE std::size_t(4096)
#endif // SF_BYTE_STREAM_RESERVE_SIZE

namespace sf
{

namespace wrapper
{

using ByteContainer = std::vector<unsigned char>;

template <typename OutStream = ByteContainer>
class OByteStream
{
protected:
    using item_type = typename OutStream::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::OByteStream'.");

public:
    OutStream& storage;

public:
    OByteStream(OutStream& stream) noexcept : storage(stream)
    {
        storage.reserve(SF_BYTE_STREAM_RESERVE_SIZE); // default reserve memory
    }

    template <typename T>
    void call(const T* data, std::size_t size)
    {
        auto it = Memory::const_byte_cast<item_type>(data);
        while (size > 0)
        {
            storage.emplace_back(*it++);
            --size;
        }
    }
};

template <typename InStream = ByteContainer>
struct IByteStream
{
protected:
    using item_type = typename InStream::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::IByteStream'.");

public:
    InStream& storage;
    std::size_t offset;

    IByteStream(InStream& stream) noexcept : storage(stream), offset() {}

    template <typename T>
    void call(T* data, std::size_t size)
    {
        auto it = Memory::byte_cast<item_type>(data);
        while (size > 0)
        {
            *it++ = storage[offset++];
            --size;
        }
    }
};

template <typename OutStream>
class OFileStream
{
public:
    OutStream& file;

public:
    OFileStream(OutStream& stream) noexcept : file(stream) {}

    template <typename T>
    void call(const T* data, std::size_t memory_size)
    {
        file.write(Memory::const_byte_cast(data), memory_size);
    }
};

template <typename InStream>
class IFileStream
{
public:
    InStream& file;

public:
    IFileStream(InStream& stream) noexcept : file(stream) {}

    template <typename T>
    void call(T* data, std::size_t memory_size)
    {
        file.read(Memory::byte_cast(data), memory_size);
    }
};

} // namespace wrapper

} // namespace sf

namespace sf
{

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry>
class OArchive : public core::IOArchive, public core::OArchiveType
{
    SERIALIZATION_ARCHIVE(OArchive)

public:
    using TrackingKeyType = std::uintptr_t;
    using TrackingTable = std::unordered_map<TrackingKeyType, bool>;
    using HierarchyTrackingTable = tracking::HierarchyTrack<TrackingKeyType>;

private:
    StreamWrapper archive_;

    TrackingTable track_shared_;
    TrackingTable track_raw_;

    HierarchyTrackingTable track_hierarchy_;

    Registry registry_;

public:
    template <typename OutStream> OArchive(OutStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable& { return track_shared_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable& { return track_raw_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> HierarchyTrackingTable& { return track_hierarchy_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator<< (T&& data) -> OArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> OArchive&;

    auto operator() () noexcept -> OArchive& { return *this; }
};

// create default OArchive with wrapper::OByteStream<>
template <typename OutStream>
OArchive<wrapper::OByteStream<OutStream>> oarchive(OutStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          typename OutStream>
OArchive<StreamWrapper<OutStream>, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          typename OutStream>
OArchive<StreamWrapper, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper, class Registry>
template <typename OutStream>
OArchive<StreamWrapper, Registry>::OArchive(OutStream& stream)
    : archive_{stream}, track_shared_(), track_raw_(), track_hierarchy_(), registry_()
{
}

template <class StreamWrapper, class Registry>
template <typename T>
auto OArchive<StreamWrapper, Registry>::operator<< (T&& data) -> OArchive&
{
    return (*this) & std::forward<T>(data);
}

template <class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto OArchive<StreamWrapper, Registry>::operator() (T& data, Tn&... data_n) -> OArchive&
{
    (*this) & data;
    return operator()(data_n...);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::is_unsupported<T>>::value)>
Archive& operator& (Archive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unsupported type for the 'sf::OArchive'.");

    return archive;
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::negation<meta::is_registered_extern<T>>>::value)>
Archive& operator& (Archive& archive, T& unregistered)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unregistered type for the 'sf::OArchive'.");

    return archive;
}

} // namespace sf

namespace sf
{

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry>
class IArchive : public core::IOArchive, public core::IArchiveType
{
    SERIALIZATION_ARCHIVE(IArchive)

private:
    template <typename VoidPointer>
    struct TrackData { VoidPointer address = nullptr; };

public:
    using Shared = TrackData<Memory::shared_ptr<void>>;
    using Raw = TrackData<Memory::raw_ptr<void>>;

public:
    using TrackingKeyType = std::uintptr_t;

    template <typename TrackData>
    using TrackingTable = std::unordered_map<TrackingKeyType, TrackData>;

    using HierarchyTrackingTable = tracking::HierarchyTrack<TrackingKeyType>;

private:
    StreamWrapper archive_;

    TrackingTable<Shared> track_shared_;
    TrackingTable<Raw> track_raw_;

    HierarchyTrackingTable track_hierarchy_;

    Registry registry_;

public:
    template <typename InStream> IArchive(InStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable<Shared>& { return track_shared_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable<Raw>& { return track_raw_; }

    template <typename TrackType,
              SFREQUIRE(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> HierarchyTrackingTable& { return track_hierarchy_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator>> (T&& data) -> IArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> IArchive&;

    auto operator() () -> IArchive& { return *this; }
};

// create default IArchive with wrapper::IByteStream<>
template <typename InStream>
IArchive<wrapper::IByteStream<InStream>> iarchive(InStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          typename InStream>
IArchive<StreamWrapper<InStream>, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          typename InStream>
IArchive<StreamWrapper, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class StreamWrapper, class Registry>
template <typename InStream>
IArchive<StreamWrapper, Registry>::IArchive(InStream& stream)
    : archive_{stream}, track_shared_(), track_raw_(), track_hierarchy_(), registry_()
{
}

template <class StreamWrapper, class Registry>
template <typename T>
auto IArchive<StreamWrapper, Registry>::operator>> (T&& data) -> IArchive&
{
    return (*this) & std::forward<T>(data);
}

template <class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto IArchive<StreamWrapper, Registry>::operator() (T& data, Tn&... data_n) -> IArchive&
{
    (*this) & data;
    return operator()(data_n...);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::is_unsupported<T>>::value)>
Archive& operator& (Archive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unsupported type for the 'sf::IArchive'.");

    return archive;
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::negation<meta::is_registered_extern<T>>>::value)>
Archive& operator& (Archive& archive, T& unregistered)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unregistered type for the 'sf::IArchive'.");

    return archive;
}

} // namespace sf

#define EXTERN_CONDITIONAL_SERIALIZATION(mode, parameter, ...)                                          \
    template <class Archive, typename T,                                                                \
              SFREQUIRE(::sf::meta::all<::sf::meta::is_##mode<Archive>,                                 \
                                        ::sf::meta::is_registered_extern<T>,                            \
                                        ::sf::meta::boolean<bool(__VA_ARGS__)>>::value)>                \
    Archive& operator& (Archive& archive, T& parameter)

// require TYPE_REGISTRY before use if not def SF_TYPE_REGISTRY_DISABLE
#define EXTERN_SERIALIZATION(mode, parameter, ...)                                                      \
    template <class Archive,                                                                            \
              SFREQUIRE(::sf::meta::all<::sf::meta::is_##mode<Archive>,                                 \
                                        ::sf::meta::is_registered_extern<__VA_ARGS__>>::value)>         \
    Archive& operator& (Archive& archive, __VA_ARGS__& parameter)

namespace sf
{

namespace meta
{

template <class T> struct is_Save : is_oarchive<T> {};
template <class T> struct is_Load : is_iarchive<T> {};

template <class T> struct is_SaveLoad : is_ioarchive<T> {};

} // namespace meta

} // namespace sf

namespace sf
{

namespace apply
{

struct ApplyFunctor {};

} // namespace apply

namespace meta
{

template <typename T> struct is_apply_functor : std::is_base_of<apply::ApplyFunctor, T> {};

} // namespace meta

// inline namespace common also used in namespace library
inline namespace common
{

template <typename Archive, typename T,
          typename dT = meta::decay<T>, // T can be lvalue
          SFREQUIRE(meta::all<meta::is_archive<Archive>,
                              meta::is_registered_extern<dT>,
                              meta::is_apply_functor<dT>>::value)>
Archive& operator& (Archive& archive, T&& apply_functor)
{
    apply_functor(archive);
    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_apply_functor<T>::value)

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

namespace sf
{

namespace detail
{

template <class Archive, typename T, typename KeyType,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_save(Archive& archive, T& pointer, KeyType track_key) noexcept { /*pass*/ }

template <class Archive, typename T, typename KeyType,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void native_save(Archive& archive, T& pointer, KeyType track_key)
{
    archive.registry().save_key(archive, pointer); // write class info
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_load(Archive& archive, T& pointer, Memory::void_ptr<T>& address) noexcept
{
    Memory::assign<meta::dereference<T>>(pointer, address);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void native_load(Archive& archive, T& pointer, Memory::void_ptr<T>& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

namespace sf
{

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::is_pointer_to_standard_layout<T>>::value)>
void strict(Archive& archive, T& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer must be allocated.";

    archive & (*pointer);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::is_pointer_to_standard_layout<T>>::value)>
void strict(Archive& archive, T& pointer, Memory::void_ptr<T>& cache)
{
    using item_type = typename Memory::ptr_trait<T>::item;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    Memory::allocate<item_type>(pointer);
    cache = Memory::pure(pointer);

    archive & (*pointer);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void strict(Archive& archive, T& pointer)
{
    auto& registry = archive.registry();

    auto id = registry.save_key(archive, pointer);
    registry.save(archive, pointer, id);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void strict(Archive& archive, T& pointer, Memory::void_ptr<T>& cache)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.load(archive, pointer, id, cache);
}

// verison without cache using
template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::is_serializable_pointer<T>>::value)>
void strict(Archive& archive, T& pointer)
{
    Memory::void_ptr<T> cache = nullptr;
    strict(archive, pointer, cache);
}

namespace detail
{

template <class Archive, typename T,
          typename KeyType = typename Archive::TrackingKeyType,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::is_serializable_pointer<T>>::value)>
KeyType refer_key(Archive& archive, T& pointer)
{
    auto pure = Memory::pure(pointer);
    auto key = reinterpret_cast<KeyType>(Memory::raw(pure));

    archive & key;
    return key;
}

template <class Archive, typename T,
          typename KeyType = typename Archive::TrackingKeyType,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::is_serializable_pointer<T>>::value)>
KeyType refer_key(Archive& archive, T& pointer)
{
#ifdef SF_DEBUG
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_DEBUG

    KeyType key{};
    archive & key;

    return key;
}

} // namespace detail

namespace apply
{

template <typename T>
struct StrictFunctor : public ApplyFunctor
{
    T& data;

    StrictFunctor(T& data) noexcept : data(data) {}

    template <class Archive>
    void operator() (Archive& archive) const { strict(archive, data); }
};

} // namespace apply

template <typename T>
apply::StrictFunctor<T> strict(T& parameter) noexcept { return { parameter }; }

} // namespace sf

namespace sf
{

namespace tracking
{

template <typename TrackType, class Archive, typename KeyType,
          SFREQUIRE(meta::is_ioarchive<Archive>::value)>
bool is_track(Archive& archive, KeyType key)
{
    auto& item = archive.template tracking<TrackType>();
    return item.find(key) != item.end();
}

template <typename TrackType, class Archive, typename KeyType,
          SFREQUIRE(meta::is_ioarchive<Archive>::value)>
bool is_mixed(Archive& archive, KeyType key)
{
    using reverse_track_type = typename reverse_trait<TrackType>::type;
    return is_track<reverse_track_type>(archive, key);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::is_pointer<T>>::value)>
void track(Archive& archive, T& pointer)
{
    using track_type = typename tracking::track_trait<T>::type;

    auto key = detail::refer_key(archive, pointer); // serialize refer info
    if (not key) return;

#ifdef SF_DEBUG
    if (is_mixed<track_type>(archive, key))
        throw "Mixed pointer tracking is not allowed.";
#endif // SF_DEBUG

    auto& is_tracking = archive.template tracking<track_type>()[key];

    if (not is_tracking)
    {
        is_tracking = true;
        strict(archive, pointer); // call the strict serialization of not tracking pointer
    }
    else
    {
        detail::native_save(archive, pointer, key);
    }
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<Archive>,
                              meta::negation<meta::is_pointer<T>>>::value)>
void track(Archive& archive, T& data)
{
    using key_type = typename Archive::TrackingKeyType;

    auto address = Memory::pure(std::addressof(data));
    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.template tracking<tracking::Raw>()[key];

    if (is_tracking)
        throw "The write tracking data is already tracked.";

    is_tracking = true;

    archive & key;
    archive & data;
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::is_pointer<T>>::value)>
void track(Archive& archive, T& pointer)
{
    using track_type = typename tracking::track_trait<T>::type;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read track pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    auto key = detail::refer_key(archive, pointer); // serialize refer info
    if (not key) return;

    auto& item = archive.template tracking<track_type>()[key];

    if (item.address == nullptr)
    {
        // call the strict serialization of not tracking pointer
        strict(archive, pointer, item.address);
    }
    else
    {
        detail::native_load(archive, pointer, item.address);
    }
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<Archive>,
                              meta::negation<meta::is_pointer<T>>>::value)>
void track(Archive& archive, T& data)
{
    using key_type = typename Archive::TrackingKeyType;

    key_type key{};
    archive & key;

    auto& item = archive.template tracking<tracking::Raw>()[key];

    if (item.address != nullptr)
        throw  "The read tracking data is already tracked.";

    item.address = Memory::pure(std::addressof(data));

    archive & data;
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_ioarchive<T>,
                              meta::is_serializable_raw_pointer<T>>::value)>
void raw(Archive& archive, T& pointer)
{
    if (detail::refer_key(archive, pointer)) // serialize refer info
        strict(archive, pointer);
}

} // namespace tracking

namespace apply
{

template <typename T>
struct TrackFunctor : ApplyFunctor
{
    T& data;

    TrackFunctor(T& data) noexcept : data(data) {}

    template <class Archive>
    void operator() (Archive& archive) const { tracking::track(archive, data); }
};

template <typename T>
struct RawFunctor : ApplyFunctor
{
    T& data;

    RawFunctor(T& data) noexcept : data(data) {}

    template <class Archive>
    void operator() (Archive& archive) const { tracking::raw(archive, data); }
};

} // namespace apply

namespace tracking
{

template <typename T> apply::TrackFunctor<T> track(T& data) noexcept { return { data }; }
template <typename T> apply::RawFunctor<T> raw(T& data) noexcept { return { data }; }

} // namespace tracking

} // namespace sf

#include <valarray> // valarray

namespace sf
{

namespace utility
{

template <class Container>
inline const typename Container::value_type* data(const Container& c) noexcept
{
    // if begin return iterator - we should deref it and take address again
    return std::addressof(*std::begin(c));
}

template <typename T>
inline const T* data(const std::valarray<T>& c) noexcept
{
    return std::begin(c);
}

template <class T, std::size_t N>
inline const T* data(const T (&array)[N]) noexcept
{
    return array;
}

template <class Container>
inline auto size(const Container& c) noexcept -> decltype(c.size())
{
    return c.size();
}

template <class T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept
{
    return N;
}

template <typename CharType,
          SFREQUIRE(meta::is_character<CharType>::value)>
std::size_t size(const CharType* str) noexcept
{
    std::size_t count{};
    while (*str++ != CharType{}) ++count;

    return count;
}

} // namespace utility

} // namespace sf

namespace sf
{

namespace compress
{

// always require compressible type for fast compression
template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              meta::is_compressible<T>>::value)>
void fast(Archive& archive, T& object)
{
    using item_type = meta::value_type<T>;

    archive.stream().call
    (
        const_cast<item_type*>(utility::data(object)),
        utility::size(object) * sizeof(item_type)
    );
}

template <class Archive, typename T,
          SFREQUIRE(meta::is_ioarchive<Archive>::value)>
void slow(Archive& archive, T& object)
{
    for (auto&& item : object)
        archive & item;
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              meta::is_compressible<T>>::value)>
void zip(Archive& archive, T& object)
{
    fast(archive, object);
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              meta::negation<meta::is_compressible<T>>>::value)>
void zip(Archive& archive, T& object)
{
    slow(archive, object);
}

} // namespace compress

} // namespace sf

namespace sf
{

// inline namespace common also used in namespace library
inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, object, ::Serialization::has_save_mode<T>::value)
{
    ::Serialization::save(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, object, ::Serialization::has_load_mode<T>::value)
{
    ::Serialization::load(archive, object);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, number, std::is_arithmetic<T>::value)
{
    binary(archive, number);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Save, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    return archive & value;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff{};
    archive & buff;

    enumerator = static_cast<T>(buff);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, array, std::is_array<T>::value)
{
    compress::zip(archive, array);
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, pointer, meta::is_serializable_raw_pointer<T>::value)
{
#ifdef SF_PTRTRACK_DISABLE
    tracking::raw(archive, pointer);
#else
    tracking::track(archive, pointer);
#endif // SF_PTRTRACK_DISABLE

    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::all<::Serialization::has_save_mode<T>, ::Serialization::has_load_mode<T>>::value)

CONDITIONAL_TYPE_REGISTRY(std::is_arithmetic<T>::value)
CONDITIONAL_TYPE_REGISTRY(std::is_enum<T>::value)
CONDITIONAL_TYPE_REGISTRY(std::is_array<T>::value)
CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_raw_pointer<T>::value)

#if __cplusplus >= 201703L

#define _SF_CONCAT_IMPL(a, b) a##b

// concatenation of two macro arguments
#define SFCONCAT(a, b) _SF_CONCAT_IMPL(a, b)

// return first argument from two
#define SFFIRST_ARGUMENT(first, ...) first

#define _SFVA_ARGS_SIZE_IMPL(                                                                           \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, _11, _12, _13, _14, _15, _16,                     \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,                     \
    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,                     \
    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N

#define _REVERSE_INTEGER_SEQUENCE                                                                       \
    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48,                                     \
    47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32,                                     \
    31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,                                     \
    15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define _SFVA_ARGS_SIZE(...) _SFVA_ARGS_SIZE_IMPL(__VA_ARGS__)

// return __VA_ARGS__ arguments count, for empty __VA_ARGS__ return 1
#define SFVA_ARGS_SIZE(...) _SFVA_ARGS_SIZE(__VA_ARGS__, _REVERSE_INTEGER_SEQUENCE)

// generate repeat sequance macro(1) macro(2) ... macro(count)
#define SFREPEAT(macro, count) SFCONCAT(_REPEAT_, count)(macro)

#define _REPEAT_0(macro)

#define _REPEAT_1(macro) _REPEAT_0(macro) macro(1)
#define _REPEAT_2(macro) _REPEAT_1(macro) macro(2)
#define _REPEAT_3(macro) _REPEAT_2(macro) macro(3)
#define _REPEAT_4(macro) _REPEAT_3(macro) macro(4)
#define _REPEAT_5(macro) _REPEAT_4(macro) macro(5)
#define _REPEAT_6(macro) _REPEAT_5(macro) macro(6)
#define _REPEAT_7(macro) _REPEAT_6(macro) macro(7)
#define _REPEAT_8(macro) _REPEAT_7(macro) macro(8)

#define _REPEAT_9(macro) _REPEAT_8(macro) macro(9)
#define _REPEAT_10(macro) _REPEAT_9(macro) macro(10)
#define _REPEAT_11(macro) _REPEAT_10(macro) macro(11)
#define _REPEAT_12(macro) _REPEAT_11(macro) macro(12)
#define _REPEAT_13(macro) _REPEAT_12(macro) macro(13)
#define _REPEAT_14(macro) _REPEAT_13(macro) macro(14)
#define _REPEAT_15(macro) _REPEAT_14(macro) macro(15)
#define _REPEAT_16(macro) _REPEAT_15(macro) macro(16)

#define _REPEAT_17(macro) _REPEAT_16(macro) macro(17)
#define _REPEAT_18(macro) _REPEAT_17(macro) macro(18)
#define _REPEAT_19(macro) _REPEAT_18(macro) macro(19)
#define _REPEAT_20(macro) _REPEAT_19(macro) macro(20)
#define _REPEAT_21(macro) _REPEAT_20(macro) macro(21)
#define _REPEAT_22(macro) _REPEAT_21(macro) macro(22)
#define _REPEAT_23(macro) _REPEAT_22(macro) macro(23)
#define _REPEAT_24(macro) _REPEAT_23(macro) macro(24)

#define _REPEAT_25(macro) _REPEAT_24(macro) macro(25)
#define _REPEAT_26(macro) _REPEAT_25(macro) macro(26)
#define _REPEAT_27(macro) _REPEAT_26(macro) macro(27)
#define _REPEAT_28(macro) _REPEAT_27(macro) macro(28)
#define _REPEAT_29(macro) _REPEAT_28(macro) macro(29)
#define _REPEAT_30(macro) _REPEAT_29(macro) macro(30)
#define _REPEAT_31(macro) _REPEAT_30(macro) macro(31)
#define _REPEAT_32(macro) _REPEAT_31(macro) macro(32)

#define _REPEAT_33(macro) _REPEAT_32(macro) macro(33)
#define _REPEAT_34(macro) _REPEAT_33(macro) macro(34)
#define _REPEAT_35(macro) _REPEAT_34(macro) macro(35)
#define _REPEAT_36(macro) _REPEAT_35(macro) macro(36)
#define _REPEAT_37(macro) _REPEAT_36(macro) macro(37)
#define _REPEAT_38(macro) _REPEAT_37(macro) macro(38)
#define _REPEAT_39(macro) _REPEAT_38(macro) macro(39)
#define _REPEAT_40(macro) _REPEAT_39(macro) macro(40)

#define _REPEAT_41(macro) _REPEAT_40(macro) macro(41)
#define _REPEAT_42(macro) _REPEAT_41(macro) macro(42)
#define _REPEAT_43(macro) _REPEAT_42(macro) macro(43)
#define _REPEAT_44(macro) _REPEAT_43(macro) macro(44)
#define _REPEAT_45(macro) _REPEAT_44(macro) macro(45)
#define _REPEAT_46(macro) _REPEAT_45(macro) macro(46)
#define _REPEAT_47(macro) _REPEAT_46(macro) macro(47)
#define _REPEAT_48(macro) _REPEAT_47(macro) macro(48)

#define _REPEAT_49(macro) _REPEAT_48(macro) macro(49)
#define _REPEAT_50(macro) _REPEAT_49(macro) macro(50)
#define _REPEAT_51(macro) _REPEAT_50(macro) macro(51)
#define _REPEAT_52(macro) _REPEAT_51(macro) macro(52)
#define _REPEAT_53(macro) _REPEAT_52(macro) macro(53)
#define _REPEAT_54(macro) _REPEAT_53(macro) macro(54)
#define _REPEAT_55(macro) _REPEAT_54(macro) macro(55)
#define _REPEAT_56(macro) _REPEAT_55(macro) macro(56)

#define _REPEAT_57(macro) _REPEAT_56(macro) macro(57)
#define _REPEAT_58(macro) _REPEAT_57(macro) macro(58)
#define _REPEAT_59(macro) _REPEAT_58(macro) macro(59)
#define _REPEAT_60(macro) _REPEAT_59(macro) macro(60)
#define _REPEAT_61(macro) _REPEAT_60(macro) macro(61)
#define _REPEAT_62(macro) _REPEAT_61(macro) macro(62)
#define _REPEAT_63(macro) _REPEAT_62(macro) macro(63)
#define _REPEAT_64(macro) _REPEAT_63(macro) macro(64)
// and etc.

// generate placeholder sequance _0, _1, _2, ..., _count
#define SFPLACEHOLDERS(count) SFCONCAT(_PLACEHOLDER_, count)()

#define _PLACEHOLDER_1() _0
#define _PLACEHOLDER_2() _PLACEHOLDER_1(), _1
#define _PLACEHOLDER_3() _PLACEHOLDER_2(), _2
#define _PLACEHOLDER_4() _PLACEHOLDER_3(), _3
#define _PLACEHOLDER_5() _PLACEHOLDER_4(), _4
#define _PLACEHOLDER_6() _PLACEHOLDER_5(), _5
#define _PLACEHOLDER_7() _PLACEHOLDER_6(), _6
#define _PLACEHOLDER_8() _PLACEHOLDER_7(), _7

#define _PLACEHOLDER_9() _PLACEHOLDER_8(), _8
#define _PLACEHOLDER_10() _PLACEHOLDER_9(), _9
#define _PLACEHOLDER_11() _PLACEHOLDER_10(), _10
#define _PLACEHOLDER_12() _PLACEHOLDER_11(), _11
#define _PLACEHOLDER_13() _PLACEHOLDER_12(), _12
#define _PLACEHOLDER_14() _PLACEHOLDER_13(), _13
#define _PLACEHOLDER_15() _PLACEHOLDER_14(), _14
#define _PLACEHOLDER_16() _PLACEHOLDER_15(), _15

#define _PLACEHOLDER_17() _PLACEHOLDER_16(), _16
#define _PLACEHOLDER_18() _PLACEHOLDER_17(), _17
#define _PLACEHOLDER_19() _PLACEHOLDER_18(), _18
#define _PLACEHOLDER_20() _PLACEHOLDER_19(), _19
#define _PLACEHOLDER_21() _PLACEHOLDER_20(), _20
#define _PLACEHOLDER_22() _PLACEHOLDER_21(), _21
#define _PLACEHOLDER_23() _PLACEHOLDER_22(), _22
#define _PLACEHOLDER_24() _PLACEHOLDER_23(), _23

#define _PLACEHOLDER_25() _PLACEHOLDER_24(), _24
#define _PLACEHOLDER_26() _PLACEHOLDER_25(), _25
#define _PLACEHOLDER_27() _PLACEHOLDER_26(), _26
#define _PLACEHOLDER_28() _PLACEHOLDER_27(), _27
#define _PLACEHOLDER_29() _PLACEHOLDER_28(), _28
#define _PLACEHOLDER_30() _PLACEHOLDER_29(), _29
#define _PLACEHOLDER_31() _PLACEHOLDER_30(), _30
#define _PLACEHOLDER_32() _PLACEHOLDER_31(), _31

#define _PLACEHOLDER_33() _PLACEHOLDER_32(), _32
#define _PLACEHOLDER_34() _PLACEHOLDER_33(), _33
#define _PLACEHOLDER_35() _PLACEHOLDER_34(), _34
#define _PLACEHOLDER_36() _PLACEHOLDER_35(), _35
#define _PLACEHOLDER_37() _PLACEHOLDER_36(), _36
#define _PLACEHOLDER_38() _PLACEHOLDER_37(), _37
#define _PLACEHOLDER_39() _PLACEHOLDER_38(), _38
#define _PLACEHOLDER_40() _PLACEHOLDER_39(), _39

#define _PLACEHOLDER_41() _PLACEHOLDER_40(), _40
#define _PLACEHOLDER_42() _PLACEHOLDER_41(), _41
#define _PLACEHOLDER_43() _PLACEHOLDER_42(), _42
#define _PLACEHOLDER_44() _PLACEHOLDER_43(), _43
#define _PLACEHOLDER_45() _PLACEHOLDER_44(), _44
#define _PLACEHOLDER_46() _PLACEHOLDER_45(), _45
#define _PLACEHOLDER_47() _PLACEHOLDER_46(), _46
#define _PLACEHOLDER_48() _PLACEHOLDER_47(), _47

#define _PLACEHOLDER_49() _PLACEHOLDER_48(), _48
#define _PLACEHOLDER_50() _PLACEHOLDER_49(), _49
#define _PLACEHOLDER_51() _PLACEHOLDER_50(), _50
#define _PLACEHOLDER_52() _PLACEHOLDER_51(), _51
#define _PLACEHOLDER_53() _PLACEHOLDER_52(), _52
#define _PLACEHOLDER_54() _PLACEHOLDER_53(), _53
#define _PLACEHOLDER_55() _PLACEHOLDER_54(), _54
#define _PLACEHOLDER_56() _PLACEHOLDER_55(), _55

#define _PLACEHOLDER_57() _PLACEHOLDER_56(), _56
#define _PLACEHOLDER_58() _PLACEHOLDER_57(), _57
#define _PLACEHOLDER_59() _PLACEHOLDER_58(), _58
#define _PLACEHOLDER_60() _PLACEHOLDER_59(), _59
#define _PLACEHOLDER_61() _PLACEHOLDER_60(), _60
#define _PLACEHOLDER_62() _PLACEHOLDER_61(), _61
#define _PLACEHOLDER_63() _PLACEHOLDER_62(), _62
#define _PLACEHOLDER_64() _PLACEHOLDER_63(), _63
// and etc.

#define _SF_AGGREGATE_IMPLEMENTATION_GENERIC(count)                                                     \
    template <class Archive, typename T>                                                                \
    void aggregate_implementation(Archive& archive, T& object, meta::dispatch<count>) {                 \
        auto& [SFPLACEHOLDERS(count)] = object;                                                         \
        archive(SFPLACEHOLDERS(count));                                                                 \
    }

namespace sf
{

namespace meta
{

template <typename T> struct is_serializable_aggregate
    : all<is_aggregate<T>,
          negation<std::is_union<T>>,
          negation<::Serialization::has_save_mode<T>>,
          negation<::Serialization::has_save_mode<T>>> {};

} // namespace meta

namespace detail
{

template <class Archive, typename T>
void aggregate_implementation(Archive& archive, T& object, meta::dispatch<0>) noexcept { /*pass*/ }

SFREPEAT(_SF_AGGREGATE_IMPLEMENTATION_GENERIC, 64)

} // namespace detail

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              meta::is_aggregate<T>>::value)>
void aggregate(Archive& archive, T& object)
{
    constexpr auto size = meta::aggregate_size<T>::value;
    detail::aggregate_implementation(archive, object, meta::dispatch<size>{});
}

inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, object, meta::is_serializable_aggregate<T>::value)
{
    aggregate(archive, object);
    return archive;
}

} // inline namespace common

namespace apply
{

template <typename T>
struct AggregateFunctor : ApplyFunctor
{
    T& object;

    AggregateFunctor(T& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { aggregate(archive, object); }
};

} // namespace apply

template <typename T> apply::AggregateFunctor<T> aggregate(T& object) noexcept { return { object }; }

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_aggregate<T>::value)

// clean up
#undef _SF_AGGREGATE_IMPLEMENTATION_GENERIC

#endif // if

namespace sf
{

namespace meta
{

template <typename T> struct is_serializable_union
    : all<std::is_union<T>,
          negation<::Serialization::has_save_mode<T>>,
          negation<::Serialization::has_save_mode<T>>> {};

} // namespace meta

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, data, meta::is_serializable_union<T>::value)
{
    binary(archive, data);
    return archive;
}

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_union<T>::value)

namespace sf
{

template <class Base, class Archive, class Derived,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              std::is_base_of<Base, Derived>>::value)>
void base(Archive& archive, Derived& object)
{
    ::Serialization::serialize_base<Base>(archive, object);
}

template <class Base, class Archive, class Derived,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              std::is_base_of<Base, Derived>>::value)>
void virtual_base(Archive& archive, Derived& object)
{
#ifdef SF_PTRTRACK_DISABLE
    if (::Serialization::trait(object) == ::Serialization::template trait<Derived>())
        base<Base>(archive, object);
#else
    using key_type = typename Archive::TrackingKeyType;

    auto address = Memory::pure(std::addressof(object));

    auto key = reinterpret_cast<key_type>(address);
    auto trait = ::Serialization::trait<Base>();

    auto& hierarchy_tracking = archive.template tracking<tracking::Hierarchy>();

    auto& is_tracking = hierarchy_tracking[{key, trait}];
    if (not is_tracking)
    {
        is_tracking = true;
        base<Base>(archive, object);
    }
#endif // SF_PTRTRACK_DISABLE
}

namespace detail
{

template <class Base, class Archive, class Derived,
          SFREQUIRE(not ::Serialization::is_virtual_base_of<Base, Derived>::value)>
void native_base(Archive& archive, Derived& object_with_base)
{
    base<Base>(archive, object_with_base);
}

template <class Base, class Archive, class Derived,
          SFREQUIRE(::Serialization::is_virtual_base_of<Base, Derived>::value)>
void native_base(Archive& archive, Derived& object_with_virtual_base)
{
    virtual_base<Base>(archive, object_with_virtual_base);
}

} // namespace detail

namespace apply
{

template <class Derived, class Base>
struct BaseFunctor : ApplyFunctor
{
    Derived& object;

    BaseFunctor(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { base<Base>(archive, object); }
};

template <class Derived, class Base>
struct VirtualBaseFunctor : ApplyFunctor
{
    Derived& object;

    VirtualBaseFunctor(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { virtual_base<Base>(archive, object); }
};

} // namespace apply

template <class Base, class Derived,
          SFREQUIRE(std::is_base_of<Base, Derived>::value)>
apply::BaseFunctor<Derived, Base> base(Derived& object) noexcept { return { object }; }

template <class Base, class Derived,
          SFREQUIRE(std::is_base_of<Base, Derived>::value)>
apply::VirtualBaseFunctor<Derived, Base> virtual_base(Derived& object) noexcept { return { object }; }

// default empty implementation
template <class Archive, class Derived>
void hierarchy(Archive& archive, Derived& object) noexcept { /*pass*/ }

// Variadic native_base function
template <class Base, class... Base_n, class Archive, class Derived,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              meta::is_derived_of<Derived, Base, Base_n...>>::value)>
void hierarchy(Archive& archive, Derived& object)
{
    detail::native_base<Base>(archive, object);
    hierarchy<Base_n...>(archive, object);
}

namespace apply
{

template <class Derived, class Base, class... Base_n>
struct HierarchyFunctor : ApplyFunctor
{
    Derived& object;

    HierarchyFunctor(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { hierarchy<Base, Base_n...>(archive, object); }
};

} // namespace apply

template <class Base, class... Base_n, class Derived,
          SFREQUIRE(meta::is_derived_of<Derived, Base, Base_n...>::value)>
apply::HierarchyFunctor<Derived, Base, Base_n...> hierarchy(Derived& object) noexcept
{
    return { object };
}

} // namespace sf

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend class ::Serialization;

// Alternative instantiable registration with library trait no-rtti
#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        SERIALIZATION_FIXTURE(__VA_ARGS__)                                                              \
        SERIALIZATION_TRAIT(__VA_ARGS__)
#endif // SERIALIZABLE

namespace sf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename T> void serializable()
{
    static_assert(not meta::is_unsupported<T>::value, "The 'T' is an unsupported type for serialization.");
    static_assert(meta::is_registered_extern<T>::value, "The 'T' is an unregistered type for serialization.");

    dynamic::InstantiableFixture<T>::call();

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)
    dynamic::AnyFixture<T>::call();
#endif // if
}

template <typename T> T&& serializable(T&& object)
{
    serializable<meta::decay<T>>();
    return std::forward<T>(object);
}

} // namepsace sf

EXPORT_SERIALIZATION_ARCHIVE(0, I, IArchive<wrapper::IByteStream<wrapper::ByteContainer>>)
EXPORT_SERIALIZATION_ARCHIVE(0, O, OArchive<wrapper::OByteStream<wrapper::ByteContainer>>)

#ifndef SF_DEFAULT_DISABLE

#include <fstream> // ifstream, ofstream

EXPORT_SERIALIZATION_ARCHIVE(1, I, IArchive<wrapper::IFileStream<std::ifstream>>)
EXPORT_SERIALIZATION_ARCHIVE(1, O, OArchive<wrapper::OFileStream<std::ofstream>>)

#endif // SF_DEFAULT_DISABLE

#include <initializer_list> // initializer_list

namespace sf
{

template <typename T>
class alias
{
private:
    T* data_;

public:
    using type = T;

    // DONT use dereferencing of null data before rebinding
    alias() noexcept : data_(nullptr) {}

    template <typename dT,
              SFREQUIRE(::Serialization::is_pointer_cast_allowed<dT, T>::value)>
    alias(dT& data) noexcept
        : data_(std::addressof(data)) {}

    template <typename dT>
    alias(const alias<dT>& data) noexcept : alias(data.get()) {}

public:
    // rebinding data
    alias(const alias&) = default;
    alias& operator=(const alias&) = default;

    bool is_refer() const noexcept { return data_ != nullptr; }

    template <typename dT>
    bool is_refer(dT& data)  const noexcept { return data_ == std::addressof(data); }

    operator T&() const noexcept { return get(); }

    T& get() const noexcept { return *data_; }
    void set(T& data) noexcept { data_ = std::addressof(data); }
};

namespace meta
{

template <typename> struct is_alias : std::false_type {};
template <typename T>
struct is_alias<alias<T>> : std::true_type {};

} // namespace meta

// inline namespace common also used in namespace library
inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, alias, meta::is_alias<T>::value)
{
    using key_type = typename Archive::TrackingKeyType;

    if (not alias.is_refer())
        throw "The write alias must be initialized.";

    auto pointer = std::addressof(alias.get());
    auto key = detail::refer_key(archive, pointer);

    auto& is_tracking = archive.template tracking<tracking::Raw>()[key];

    if (not is_tracking)
        throw "The write alias must be tracked before.";

    detail::native_save(archive, pointer, key);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, alias, meta::is_alias<T>::value)
{
    using key_type   = typename Archive::TrackingKeyType;
    using value_type = typename T::type;

    using track_type = tracking::Raw;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (alias.is_refer())
        throw "The read alias must be null.";
#endif // SF_GARBAGE_CHECK_DISABLE

    key_type key{};
    archive & key;

    auto& item = archive.template tracking<tracking::Raw>()[key];

    if (item.address == nullptr)
        throw "The read alias must be tracked before.";

    value_type* pointer = nullptr;

    detail::native_load(archive, pointer, item.address);

    alias.set(*pointer); // pointer will never nullptr

    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_alias<T>::value)

namespace sf
{

namespace utility
{

template <typename T, std::size_t N>
class Span;

template <typename T, std::size_t N>
class SpanBase
{
public:
    using size_type         = std::size_t;
    using value_type        = Span<T, N - 1>;

    using pointer           = meta::pointer<T, N>;
    using const_pointer     = const meta::pointer<T, N>;

protected:
    using Dimension         = size_type[N];

protected:
    alias<pointer> data_;
    Dimension dim_;

protected:
    SpanBase(pointer& data) noexcept
        : data_(data), dim_() {}

    template <typename D, typename... Dn>
    SpanBase(pointer& data, D d, Dn... dn) noexcept
        : data_(data), dim_{d, dn...} {}

public:
    void init(pointer data) noexcept { data_.get() = data; }
    void data(alias<pointer> data) noexcept { data_ = data; }

    pointer& data() noexcept { return data_; }
    size_type size() const noexcept { return dim_[0]; }
    Dimension& dim() noexcept { return dim_; }
};

template <typename T, std::size_t N>
class Span : public SpanBase<T, N>
{
protected:
    using Core = SpanBase<T, N>;

public:
    using typename Core::size_type;
    using typename Core::pointer;

public:
    using value_type        = Span<T, N - 1>;
    using dereference_type  = meta::pointer<T, N - 1>;

    using reference         = value_type&;
    using const_reference   = const value_type&;

protected:
    using typename Core::Dimension;

private:
    mutable value_type child_scope_;

public:
    Span(pointer& data, Dimension dim) noexcept
        : Core(data), child_scope_(data[0], dim + 1)
    {
        for (size_type i = 0; i < N; ++i)
            this->dim_[i] = dim;
    }

    template <typename D, typename... Dn,
              SFREQUIRE(not std::is_array<D>::value)>
    Span(pointer& data, D d, Dn... dn) noexcept
        : Core(data, d, dn...), child_scope_(data[0], dn...) {}

    void size(size_type value) noexcept { this->dim_[0] = value; }

    reference operator[] (size_type i) noexcept
    {
        child_scope_.data(this->data_[i]);
        return child_scope_;
    }

public:
    using Core::size; // prevent Core function hiding
};

template <typename T>
class Span<T, 1> : public SpanBase<T, 1>
{
protected:
    using Core = SpanBase<T, 1>;

public:
    using typename Core::size_type;
    using typename Core::pointer;

public:
    using value_type        = T;
    using dereference_type  = T;

    using reference         = T&;
    using const_reference   = const T&;

protected:
    using typename Core::Dimension;

public:
    Span(pointer& data, Dimension size) noexcept
        : Core(data)
    {
        this->dim_[0] = size[0];
    }

    Span(pointer& data, size_type size) noexcept
        : Core(data, size) {}

    reference operator[] (size_type i) noexcept { return this->data_[i]; }
};

} // namespace utility

namespace meta
{

template <typename> struct is_span : std::false_type {};
template <typename T, std::size_t N>
struct is_span<utility::Span<T, N>> : std::true_type {};

template <typename Pointer, typename D, typename... Dn>
struct is_span_set
    : meta::all<meta::boolean<meta::pointer_count<Pointer>() >= sizeof...(Dn) + 1>,
                meta::all<std::is_arithmetic<D>,
                          std::is_arithmetic<Dn>...>> {};

} // namespace meta

namespace utility
{

template <typename Pointer, typename D, typename... Dn,
          std::size_t N = sizeof...(Dn) + 1,
          typename Type = meta::remove_pointer<Pointer, N>,
          SFREQUIRE(meta::is_span_set<Pointer, D, Dn...>::value)>
Span<Type, N> make_span(Pointer& data, D d, Dn... dn)
{
    using size_type = typename Span<Type, N>::size_type;
    return { data, static_cast<size_type>(d), static_cast<size_type>(dn)... };
}

} // namespace utility

namespace detail
{

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_archive<Archive>,
                              meta::negation<meta::is_span<T>>>::value)>
void span_implementation(Archive& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class OArchive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<OArchive>,
                              meta::is_span<T>>::value)>
void span_implementation(OArchive& archive, T& array)
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < array.size(); ++i)
        span_implementation(archive, array[i]);
}

template <class IArchive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<IArchive>,
                              meta::is_span<T>>::value)>
void span_implementation(IArchive& archive, T& array)
{
    using size_type        = typename T::size_type;
    using dereference_type = typename T::dereference_type;

    using pointer          = typename T::pointer;

    auto ptr = new dereference_type [array.size()] {};
    array.init(ptr);

    for (size_type i = 0; i < array.size(); ++i)
        span_implementation(archive, array[i]);
}

} // namespace detail

// inline namespace common also used in namespace library
inline namespace common
{

template <class Archive, typename T,
          typename D, typename... Dn,
          SFREQUIRE(meta::all<meta::is_archive<Archive>,
                              meta::is_span_set<T, D, Dn...>>::value)>
void span(Archive& archive, T& pointer, D& dimension, Dn&... dimension_n)
{
    if (not detail::refer_key(archive, pointer)) return; // serialize refer info
    archive(dimension, dimension_n...);

    auto span_data = utility::make_span(pointer, dimension, dimension_n...);
    detail::span_implementation(archive, span_data);
}

} // inline namespace common

namespace apply
{

template <typename T, typename D, typename... Dn>
struct SpanFunctor : ApplyFunctor
{
    using Pack = std::tuple<T&, D&, Dn&...>;

    Pack pack;

    SpanFunctor(T& pointer, D& d, Dn&... dn) noexcept : pack(pointer, d, dn...) {}

    template <class Archive>
    void operator() (Archive& archive) const
    {
        invoke(archive, meta::make_index_sequence<std::tuple_size<Pack>::value>{});
    }

private:
    template <class Archive, std::size_t... I>
    void invoke(Archive& archive, meta::index_sequence<I...>) const
    {
        span(archive, std::get<I>(pack)...);
    }
};

} // namespace apply

// inline namespace common also used in namespace library
inline namespace common
{

template <typename T, typename D, typename... Dn,
          SFREQUIRE(meta::is_span_set<T, D, Dn...>::value)>
apply::SpanFunctor<T, D, Dn...> span(T& pointer, D& dimension, Dn&... dimension_n) noexcept
{
    return { pointer, dimension, dimension_n... };
}

} // inline namespace common

} // namespace sf

#define _BITPACK_N(...) SFCONCAT(_BITPACK_, SFVA_ARGS_SIZE(__VA_ARGS__))(__VA_ARGS__)
#define _BITPACK_BODY(archive, ...) archive); _BITPACK_N(__VA_ARGS__) }
#define _BITPACK_IMPLEMENTATION(...) { auto __bitpack = ::sf::bitpack<__VA_ARGS__>( _BITPACK_BODY

#define _BITFIELD(field_and_bits) SFFIRST_ARGUMENT field_and_bits = __bitpack field_and_bits;

#define _BITPACK_1(field_and_bits) _BITFIELD(field_and_bits)
#define _BITPACK_2(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_1(__VA_ARGS__)
#define _BITPACK_3(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_2(__VA_ARGS__)
#define _BITPACK_4(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_3(__VA_ARGS__)
#define _BITPACK_5(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_4(__VA_ARGS__)
#define _BITPACK_6(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_5(__VA_ARGS__)
#define _BITPACK_7(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_6(__VA_ARGS__)
#define _BITPACK_8(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_7(__VA_ARGS__)

#define _BITPACK_9(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_8(__VA_ARGS__)
#define _BITPACK_10(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_9(__VA_ARGS__)
#define _BITPACK_11(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_10(__VA_ARGS__)
#define _BITPACK_12(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_11(__VA_ARGS__)
#define _BITPACK_13(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_12(__VA_ARGS__)
#define _BITPACK_14(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_13(__VA_ARGS__)
#define _BITPACK_15(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_14(__VA_ARGS__)
#define _BITPACK_16(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_15(__VA_ARGS__)

#define _BITPACK_17(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_16(__VA_ARGS__)
#define _BITPACK_18(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_17(__VA_ARGS__)
#define _BITPACK_19(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_18(__VA_ARGS__)
#define _BITPACK_20(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_19(__VA_ARGS__)
#define _BITPACK_21(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_20(__VA_ARGS__)
#define _BITPACK_22(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_21(__VA_ARGS__)
#define _BITPACK_23(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_22(__VA_ARGS__)
#define _BITPACK_24(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_23(__VA_ARGS__)

#define _BITPACK_25(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_24(__VA_ARGS__)
#define _BITPACK_26(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_25(__VA_ARGS__)
#define _BITPACK_27(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_26(__VA_ARGS__)
#define _BITPACK_28(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_27(__VA_ARGS__)
#define _BITPACK_29(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_28(__VA_ARGS__)
#define _BITPACK_30(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_29(__VA_ARGS__)
#define _BITPACK_31(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_30(__VA_ARGS__)
#define _BITPACK_32(field_and_bits, ...) _BITFIELD(field_and_bits) _BITPACK_31(__VA_ARGS__)
// and etc.

// Signature:
// BITPACK(common_fields_type)(archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
#define BITPACK(...) _BITPACK_IMPLEMENTATION(__VA_ARGS__)

// Signature:
// FBITPACK(archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
#define FBITPACK(...) BITPACK()(__VA_ARGS__)

// FBITPACK use fixed common_fields_type size version.
// BITPACK macro will generate code:
// {
//     auto __bitpack = ::sf::bitpack<common_fields_type>(archive);
//     object.field0 = __bitpack(object.field0, field0_bits);
//     object.field1 = __bitpack(object.field1, field1_bits);
//     and etc.
// }

namespace sf
{

namespace detail
{

template <class Archive, typename T, typename enable = void>
struct BitPack;

template <class Archive, typename T>
struct BitPack<Archive, T, SFWHEN(sf::meta::is_oarchive<Archive>::value)>
{
    Archive& archive;
    T data{};
    std::size_t offset{};

    BitPack(Archive& archive) : archive(archive) {}
    ~BitPack() { archive & data; }

    T operator()(T field, std::size_t bits) noexcept
    {
        // same as data = data | (field << offset);
        data |= field << offset;
        offset += bits;

        return field;
    }
};

template <class Archive, typename T>
struct BitPack<Archive, T, SFWHEN(sf::meta::is_iarchive<Archive>::value)>
{
    Archive& archive;
    T data{};

    BitPack(Archive& archive) : archive(archive) { archive & data; }

    T operator()(T field, std::size_t bits) noexcept
    {
        //same as field = data & ~(0xf...f << bits)
        field = data & ~(~T{} << bits);
        data >>= bits;

        return field;
    }
};

} // namespace detail

template <typename PackType = let::u32, class Archive>
detail::BitPack<Archive, PackType> bitpack(Archive& archive) noexcept { return { archive }; }

} // namespace sf

namespace sf
{

namespace meta
{

template <typename> struct is_std_vector : std::false_type {};
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, vector, meta::is_std_vector<T>::value)
{
    let::u64 size = vector.size();
    archive & size;

    compress::zip(archive, vector);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, vector, meta::is_std_vector<T>::value)
{
    let::u64 size{};
    archive & size;

    vector.resize(size);
    compress::zip(archive, vector);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_vector<T>::value)
TYPE_REGISTRY(std::vector<bool>)

namespace sf
{

inline namespace library
{

// slow implementation
EXTERN_SERIALIZATION(Save, vector, std::vector<bool>)
{
    let::u64 size = vector.size();
    archive & size;

    for(auto item:vector)
    {
        bool boolean = item;
        archive & boolean;
    }

    return archive;
}

EXTERN_SERIALIZATION(Load, vector, std::vector<bool>)
{
    let::u64 size{};
    archive & size;

    vector.resize(size);

    for(auto item/*bit_reference*/:vector)
    {
        bool boolean{};
        archive & boolean;
        item = boolean;
    }

    return archive;
}

} // inline namespace library

} // namespace sf

namespace sf
{

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, array, meta::is_std_array<T>::value)
{
    compress::zip(archive, array);
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_array<T>::value)

#include <string> // basic_string

namespace sf
{

namespace meta
{

template <typename>
struct is_std_basic_string : std::false_type {};

template <typename Char, typename Traits, typename Alloc>
struct is_std_basic_string<std::basic_string<Char, Traits, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, string, meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    let::u64 size = string.size();
    archive & size;

    compress::zip(archive, string);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, string, meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    let::u64 size{};
    archive & size;

    string.resize(size);
    compress::zip(archive, string);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_basic_string<T>::value)

namespace sf
{

namespace meta
{

template <typename> struct is_std_pair : std::false_type {};
template <typename T1, typename T2>
struct is_std_pair<std::pair<T1, T2>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, pair, meta::is_std_pair<T>::value)
{
    archive & pair.first & pair.second;
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_pair<T>::value)

namespace sf
{

namespace meta
{

template <typename> struct is_std_tuple : std::false_type {};
template <typename... Tn>
struct is_std_tuple<std::tuple<Tn...>> : std::true_type {};

} // namespace meta

namespace detail
{

template <class Archive, class T, std::size_t... I,
          SFREQUIRE(meta::is_std_tuple<T>::value)>
void expand(Archive& archive, T& tuple, meta::index_sequence<I...>)
{
    archive(std::get<I>(tuple)...);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, tuple, meta::is_std_tuple<T>::value)
{
    constexpr auto size = std::tuple_size<T>::value;
    detail::expand(archive, tuple, meta::make_index_sequence<size>{});

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_tuple<T>::value)

#include <list> // list

namespace sf
{

namespace meta
{

template <typename> struct is_std_list : std::false_type {};
template <typename T, typename Alloc>
struct is_std_list<std::list<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, list, meta::is_std_list<T>::value)
{
    let::u64 size = list.size();
    archive & size;

    compress::slow(archive, list);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, list, meta::is_std_list<T>::value)
{
    let::u64 size{};
    archive & size;

    list.resize(size);
    compress::slow(archive, list);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_list<T>::value)

#include <forward_list> // forward_list

namespace sf
{

namespace meta
{

template <typename> struct is_std_forward_list : std::false_type {};
template <typename T, typename Alloc>
struct is_std_forward_list<std::forward_list<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, forward_list, meta::is_std_forward_list<T>::value)
{
    let::u64 size = std::distance(forward_list.begin(), forward_list.end());
    archive & size;

    compress::slow(archive, forward_list);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, forward_list, meta::is_std_forward_list<T>::value)
{
    let::u64 size{};
    archive & size;

    forward_list.resize(size);
    compress::slow(archive, forward_list);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_forward_list<T>::value)

#include <set> // set, multiset
#include <unordered_set> // unordered_set, unordered_multiset

#define _SF_IS_STD_SET_TYPE_META_GENERIC(set_type)                                                      \
    template <typename> struct is_std_##set_type : std::false_type {};                                  \
    template <typename Key, typename Compare, typename Alloc>                                           \
    struct is_std_##set_type<std::set_type<Key, Compare, Alloc>> : std::true_type {};

namespace sf
{

namespace meta
{

_SF_IS_STD_SET_TYPE_META_GENERIC(set)
_SF_IS_STD_SET_TYPE_META_GENERIC(unordered_set)
_SF_IS_STD_SET_TYPE_META_GENERIC(multiset)
_SF_IS_STD_SET_TYPE_META_GENERIC(unordered_multiset)

template <class T> struct is_std_any_unordered_set
    : one<is_std_unordered_set<T>,
          is_std_unordered_multiset<T>> {};

template <class T> struct is_std_any_set
    : one<is_std_set<T>,
          is_std_multiset<T>,
          is_std_any_unordered_set<T>> {};

} // namespace meta

namespace detail
{

template <class T,
          SFREQUIRE(not meta::is_std_any_unordered_set<T>::value)>
void reserve_unordered(T& ordered, std::size_t size) noexcept { /*pass*/ }

template <class T,
          SFREQUIRE(meta::is_std_any_unordered_set<T>::value)>
void reserve_unordered(T& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, set, meta::is_std_any_set<T>::value)
{
    let::u64 size = set.size();
    archive & size;

    compress::slow(archive, set);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, set, meta::is_std_any_set<T>::value)
{
    using value_type = typename T::value_type;

    let::u64 size{};
    archive & size;

    set.clear();
    detail::reserve_unordered(set, size);

    auto hint = set.begin();
    for (let::u64 i = 0; i < size; ++i)
    {
        value_type item{}; // temp
        archive & item;

        hint = set.emplace_hint(hint, std::move(item));
    }

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any_set<T>::value)

// clean up
#undef _SF_IS_STD_SET_TYPE_META_GENERIC

namespace sf
{

namespace meta
{

template <typename> struct is_std_unique_ptr : std::false_type {};
template <typename T, typename Deleter>
struct is_std_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, unique_ptr, meta::is_std_unique_ptr<T>::value)
{
    auto data = unique_ptr.get();
    archive & data;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, unique_ptr, meta::is_std_unique_ptr<T>::value)
{
    using item_type = typename Memory::ptr_trait<T>::item;

    item_type* data = nullptr;
    archive & data;

    unique_ptr.reset(data);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_unique_ptr<T>::value)

namespace sf
{

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, shared_ptr, meta::is_std_shared_ptr<T>::value)
{
    tracking::track(archive, shared_ptr);
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_shared_ptr<T>::value)

// serialization of shared_ptr

namespace sf
{

namespace meta
{

template <typename> struct is_std_weak_ptr : std::false_type {};
template <typename T> struct is_std_weak_ptr<std::weak_ptr<T>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, weak_ptr, meta::is_std_weak_ptr<T>::value)
{
    auto sptr = weak_ptr.lock();
    archive & sptr;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, weak_ptr, meta::is_std_weak_ptr<T>::value)
{
    using item_type = typename Memory::ptr_trait<T>::item;

    std::shared_ptr<item_type> sptr;
    archive & sptr;

    weak_ptr = sptr;

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_weak_ptr<T>::value)

#include <map> // map, multimap

// serialization of core map value_type

#define _SF_IS_STD_MAP_TYPE_META_GENERIC(map_type)                                                      \
    template <typename> struct is_std_##map_type : std::false_type {};                                  \
    template <typename Key, typename Type, typename Compare, typename Alloc>                            \
    struct is_std_##map_type<std::map_type<Key, Type, Compare, Alloc>> : std::true_type {};

namespace sf
{

namespace meta
{

_SF_IS_STD_MAP_TYPE_META_GENERIC(map)
_SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_map)
_SF_IS_STD_MAP_TYPE_META_GENERIC(multimap)
_SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_multimap)

template <class T> struct is_std_any_unordered_map
    : one<is_std_unordered_map<T>,
          is_std_unordered_multimap<T>> {};

template <class T> struct is_std_any_map
    : one<is_std_map<T>,
          is_std_multimap<T>,
          is_std_any_unordered_map<T>> {};

} // namespace meta

namespace detail
{

template <class T,
          SFREQUIRE(not meta::is_std_any_unordered_map<T>::value)>
void reserve_unordered(T& ordered, std::size_t size) noexcept { /*pass*/ }

template <class T,
          SFREQUIRE(meta::is_std_any_unordered_map<T>::value)>
void reserve_unordered(T& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, map, meta::is_std_any_map<T>::value)
{
    let::u64 size = map.size();
    archive & size;

    compress::slow(archive, map);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, map, meta::is_std_any_map<T>::value)
{
    using key_type   = typename T::key_type;
    using value_type = typename T::mapped_type;

    let::u64 size{};
    archive & size;

    map.clear();
    detail::reserve_unordered(map, size);

    auto hint = map.begin();
    for (let::u64 i = 0; i < size; ++i)
    {
        key_type key{};
        value_type value{};

        archive & key & value;

        hint = map.emplace_hint(hint, std::move(key), std::move(value));
    }

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any_map<T>::value)

//clear
#undef _SF_IS_STD_MAP_TYPE_META_GENERIC

#include <deque> // deque

namespace sf
{

namespace meta
{

template <typename> struct is_std_deque : std::false_type {};
template <typename T, typename Alloc>
struct is_std_deque<std::deque<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, deque, meta::is_std_deque<T>::value)
{
    let::u64 size = deque.size();
    archive & size;

    compress::slow(archive, deque);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, deque, meta::is_std_deque<T>::value)
{
    let::u64 size{};
    archive & size;

    deque.resize(size);
    compress::slow(archive, deque);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_deque<T>::value)

#include <stack> // stack

namespace sf
{

namespace meta
{

template <template <typename...> class Adapter,
          typename Type, class Container, typename... Args>
Container& underlying(Adapter<Type, Container, Args...>& adapter) noexcept
{
    using Core = Adapter<Type, Container, Args...>;

    struct base_inner : public Core
    {
        static Container& underlying(Core& core)
        {
            auto underlying_container = &base_inner::c;
            return core.*underlying_container;
        }
    };

    return base_inner::underlying(adapter);
}

} // namespace meta

} // namespace sf

// default container for stack

namespace sf
{

namespace meta
{

template <typename> struct is_std_stack : std::false_type {};
template <typename T, class Container>
struct is_std_stack<std::stack<T, Container>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, stack, meta::is_std_stack<T>::value)
{
    archive & meta::underlying(stack);
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_stack<T>::value)

#include <queue> // queue

// default container for queue

// default container for priority_queue

namespace sf
{

namespace meta
{

template <typename> struct is_std_queue : std::false_type {};
template <typename T, class Container>
struct is_std_queue<std::queue<T, Container>> : std::true_type {};

template <typename> struct is_std_priority_queue : std::false_type {};
template <typename T, class Container, class Compare>
struct is_std_priority_queue<std::priority_queue<T, Container, Compare>> : std::true_type {};

template <class T> struct is_std_any_queue
    : one<is_std_queue<T>,
          is_std_priority_queue<T>> {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, queue, meta::is_std_any_queue<T>::value)
{
    archive & meta::underlying(queue);
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any_queue<T>::value)

namespace sf
{

namespace meta
{

template <typename> struct is_std_valarray : std::false_type {};
template <typename T> struct is_std_valarray<std::valarray<T>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, valarray, meta::is_std_valarray<T>::value)
{
    let::u64 size = valarray.size();
    archive & size;

    compress::zip(archive, valarray);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, valarray, meta::is_std_valarray<T>::value)
{
    let::u64 size{};
    archive & size;

    valarray.resize(size);
    compress::zip(archive, valarray);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_valarray<T>::value)

#include <bitset> // bitset

// default array for bitset convertion

namespace sf
{

namespace meta
{

template <typename> struct is_std_bitset : std::false_type {};
template <std::size_t N> struct is_std_bitset<std::bitset<N>> : std::true_type {};

} // namespace meta

inline namespace library
{

// slow implementation
EXTERN_CONDITIONAL_SERIALIZATION(Save, bitset, meta::is_std_bitset<T>::value)
{
    auto data = bitset.to_string();
    archive & data;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, bitset, meta::is_std_bitset<T>::value)
{
    std::string data;
    archive & data;

    bitset = T(data);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_bitset<T>::value)

#include <atomic> // atomic

namespace sf
{

namespace meta
{

template <typename> struct is_std_atomic : std::false_type {};
template <typename T> struct is_std_atomic<std::atomic<T>> : std::true_type {};

template <typename T> struct atomic_trait;
template <typename T> struct atomic_trait<std::atomic<T>> { using value_type = T; };

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, atomic, meta::is_std_atomic<T>::value)
{
    auto object = atomic.load();
    archive & object;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, atomic, meta::is_std_atomic<T>::value)
{
    using object_type = typename meta::atomic_trait<T>::value_type;

    object_type object{};
    archive & object;

    atomic.store(object);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_atomic<T>::value)

#include <complex> // complex

namespace sf
{

namespace meta
{

template <typename> struct is_std_complex : std::false_type {};
template <typename T> struct is_std_complex<std::complex<T>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, complex, meta::is_std_complex<T>::value)
{
    auto re = complex.real();
    auto im = complex.imag();

    archive & re & im;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, complex, meta::is_std_complex<T>::value)
{
    using integral_type = typename T::value_type;

    integral_type re{};
    integral_type im{};

    archive & re & im;

    complex.real(re);
    complex.imag(im);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_complex<T>::value)

#if __cplusplus >= 201703L

#include <optional> // optional

namespace sf
{

namespace meta
{

template <typename> struct is_std_optional : std::false_type {};
template <typename T> struct is_std_optional<std::optional<T>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, optional, meta::is_std_optional<T>::value)
{
    auto is_init = optional.has_value();
    archive & is_init;

    if (is_init) archive & optional.value();

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, optional, meta::is_std_optional<T>::value)
{
    auto is_init = false;
    archive & is_init;

    if (is_init)
    {
        optional.emplace();
        archive & optional.value();
    }

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_optional<T>::value)

#endif // if

#if __cplusplus >= 201703L

#include <variant> // variant

// serialization of std::monostate

namespace sf
{

namespace meta
{

template <typename> struct is_std_variant : std::false_type {};
template <typename... Tn> struct is_std_variant<std::variant<Tn...>> : std::true_type {};

} // namespace meta

namespace detail
{

template <let::u64 I, class Archive, class Variant,
          SFREQUIRE(I == std::variant_size<Variant>::value)>
void variant_save(Archive& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class Archive, class Variant,
          SFREQUIRE(I < std::variant_size<Variant>::value)>
void variant_save(Archive& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_save<I + 1>(archive, variant, index);
    archive & std::get<I>(variant);
}

template <typename Type, class Archive, class Variant,
          SFREQUIRE(not std::is_constructible<Type>::value)>
void variant_load_implementation(Archive& archive, Variant& variant)
{
    throw "Require default constructor for specify type.";
}

template <typename Type, class Archive, class Variant,
          SFREQUIRE(std::is_constructible<Type>::value)>
void variant_load_implementation(Archive& archive, Variant& variant)
{
    archive & variant.template emplace<Type>();
}

template <let::u64 I, class Archive, class Variant,
          SFREQUIRE(I == std::variant_size<Variant>::value)>
void variant_load(Archive& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class Archive, class Variant,
          SFREQUIRE(I < std::variant_size<Variant>::value)>
void variant_load(Archive& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_load<I + 1>(archive, variant, index);

    using type = typename std::variant_alternative<I, Variant>::type;
    variant_load_implementation<type>(archive, variant);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, variant, meta::is_std_variant<T>::value)
{
    let::u64 index = variant.index();
    archive & index;

    if (index != std::variant_npos)
        detail::variant_save(archive, variant, index);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, variant, meta::is_std_variant<T>::value)
{
    let::u64 index{};
    archive & index;

    if (index != std::variant_npos)
        detail::variant_load(archive, variant, index);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_variant<T>::value)

#endif // if

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

namespace sf
{

namespace meta
{

template <typename> struct is_std_any : std::false_type {};
template <> struct is_std_any<std::any> : std::true_type {};

} // namespace meta

inline namespace library
{

// please, use 'sf::serializable' for type any registry before std::any serialization
EXTERN_CONDITIONAL_SERIALIZATION(Save, any, meta::is_std_any<T>::value)
{
    let::u64 hash = SF_TYPE_HASH(any.type());
    archive & hash;

    dynamic::AnyRegistry::instance().save(archive, any, hash);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, any, meta::is_std_any<T>::value)
{
    let::u64 hash{};
    archive & hash;

    dynamic::AnyRegistry::instance().load(archive, any, hash);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any<T>::value)

#endif // if

#endif // SERIALIZATION_FIXTURE_HPP
