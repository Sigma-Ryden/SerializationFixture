#ifndef SIRAF_CORE_SERIALIZATION_HPP
#define SIRAF_CORE_SERIALIZATION_HPP

#include <Siraf/Core/ArchiveBase.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp> // SIWHEN

#define SERIALIZATION(mode, ...)                                                                        \
    template <>                                                                                         \
    struct Serialization::mode<__VA_ARGS__> {                                                           \
        template <class Archive>                                                                        \
        static void call(Archive& archive, __VA_ARGS__& self);                                          \
    };                                                                                                  \
    template <class Archive>                                                                            \
    void Serialization::mode<__VA_ARGS__>::call(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <typename T>                                                                               \
    struct Serialization::mode<T, SIWHEN(__VA_ARGS__)> {                                                \
        template <class Archive>                                                                        \
        static void call(Archive& archive, T& self);                                                    \
    };                                                                                                  \
    template <typename T> template <class Archive>                                                      \
    void Serialization::mode<T, SIWHEN(__VA_ARGS__)>::call(Archive& archive, T& self)

#define _SIRAF_HAS_PROPERTY_HELPER(extern_name, inner_name)                                             \
    template <typename C, typename = void>                                                              \
    struct has_##extern_name : std::false_type {};                                                      \
    template <typename C>                                                                               \
    struct has_##extern_name<C, ::siraf::meta::to_void<decltype(&C::inner_name)>>                       \
        : std::true_type {};

// should be in global namespace
class Serialization
{
private:
    // specificaly check for construction without arguments
    template <typename T, typename = void>
    struct has_implementation : std::false_type {};
    template <typename T>
    struct has_implementation<T, ::siraf::meta::to_void<decltype(T{})>> : std::true_type {};

public:
    template <class T>
    static constexpr bool is_save_class() noexcept
    {
        return has_implementation<Save<T>>::value;
    }

    template <class T>
    static constexpr bool is_load_class() noexcept
    {
        return has_implementation<Load<T>>::value;
    }

    template <class T>
    static constexpr bool is_saveload_class() noexcept
    {
        return has_implementation<SaveLoad<T>>::value;
    }

public:
    template <class T>
    static constexpr bool has_save_mode() noexcept
    {
        return is_save_class<T>() or is_saveload_class<T>();
    }

    template <class T>
    static constexpr bool has_load_mode() noexcept
    {
        return is_load_class<T>() or is_saveload_class<T>();
    }

public:
    template <typename T, typename enable = void> struct SaveLoad;
    template <typename T, typename enable = void> struct Save;
    template <typename T, typename enable = void> struct Load;

public:
    // default implementation
    template <class Archive, typename T>
    static void call(Archive& archive, T& self)
    {
        throw "The 'T' type cannot be saved/loaded.";
    }

private:
    template <typename T>
    struct SaveModeMeta
    {
        using scope = siraf::meta::scope<Save<T>, SaveLoad<T>, Serialization>;
        static constexpr auto index = siraf::meta::select(is_save_class<T>(), is_saveload_class<T>());

        using mode = typename scope::template type<index>;
    };

    template <typename T>
    struct LoadModeMeta
    {
        using scope = siraf::meta::scope<Load<T>, SaveLoad<T>, Serialization>;
        static constexpr auto index = siraf::meta::select(is_load_class<T>(), is_saveload_class<T>());

        using mode = typename scope::template type<index>;
    };

public:
    template <typename T> using SaveMode = typename SaveModeMeta<T>::mode;
    template <typename T> using LoadMode = typename LoadModeMeta<T>::mode;

private:
    _SIRAF_HAS_PROPERTY_HELPER(static_trait, __static_trait)
    _SIRAF_HAS_PROPERTY_HELPER(trait, __trait)

public:
    template <class T>
    static constexpr bool is_registered_class() noexcept
    {
        return has_static_trait<T>::value and has_trait<T>::value;
    }

private:
    template <typename From, typename To, typename = void>
    struct can_static_cast_impl : std::false_type {};

    template <typename From, typename To>
    struct can_static_cast_impl<From, To,
        siraf::meta::to_void<decltype( static_cast<To>(std::declval<From>()) )>>
    : std::true_type {};

    template <class, class> static std::false_type try_cast(...);
    template <class From, class To> static auto try_cast(int) -> decltype
    (
        (void) std::declval<void(&)(To)>()(std::declval<From>()),
        std::true_type{}
    );

public:
    template <class From, class To>
    static constexpr bool can_static_cast() noexcept
    {
        return can_static_cast_impl<From, To>::value;
    }

    template <class From, class To>
    static constexpr bool is_cast_allowed() noexcept
    {
        return (siraf::meta::is_returnable<To>() and decltype(try_cast<From, To>(0))::value)
            or (siraf::meta::is_same_all<void, From, To>());
    }

    template <class From, class To>
    static constexpr bool is_pointer_cast_allowed() noexcept
    {
        return is_cast_allowed<From*, To*>()
            or siraf::meta::is_void<From>();
    }

public:
    using ArchiveBase = siraf::core::ArchiveBase;
    using PolymorphicTraitBase = siraf::core::PolymorphicTraitBase;

public:
    template <typename Archive, typename T>
    static void save(Archive& archive, T& data)
    {
        SaveMode<T>::call(archive, data);
    }

    template <typename Archive, typename T>
    static void load(Archive& archive, T& data)
    {
        LoadMode<T>::call(archive, data);
    }

    template <typename Base, class Archive, typename Derived,
              SIREQUIRE(siraf::meta::is_archive<Archive>() and
                        siraf::meta::is_base_of<Base, Derived>())>
    static void serialize_base(Archive& archive, Derived& object)
    {
        archive & cast<Base&>(object);
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static auto trait(T& object) noexcept -> decltype(object.__trait())
    {
        return object.__trait();
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static constexpr PolymorphicTraitBase::key_type static_trait() noexcept
    {
        return T::__static_trait();
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
#ifdef SIRAF_EXPORT_POLYMORPHIC_DISABLE
    static constexpr PolymorphicTraitBase::key_type trait() noexcept
#else
    static PolymorphicTraitBase::key_type trait() noexcept
#endif // SIRAF_EXPORT_POLYMORPHIC_DISABLE
    {
        constexpr auto trait_key = siraf::core::PolymorphicTraitKey<T>::key;

        return trait_key == PolymorphicTraitBase::base_key
             ? static_trait<T>()
             : trait_key;
    }

public:
    template <class To, class From>
    static To cast(From& from) noexcept
    {
        return static_cast<To>(from);
    }

    template <class To, class From>
    static To runtime_cast(From& from) // maybe throw exception
    {
        return dynamic_cast<To>(from);
    }
};

// clean up
#undef _SIRAF_HAS_PROPERTY_HELPER

#endif // SIRAF_CORE_SERIALIZATION_HPP
