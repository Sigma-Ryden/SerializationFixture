#ifndef SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
#define SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP

#include <unordered_map> // unordered_map

#include <SerializationFixture/Core/Serialization.hpp>
#include <SerializationFixture/Core/Memory.hpp>

#include <SerializationFixture/Core/Hash.hpp>

#include <SerializationFixture/Core/PolymorphicArchive.hpp>

#include <SerializationFixture/Dynamic/Instantiable.hpp>
#include <SerializationFixture/Dynamic/InstantiableTraits.hpp>

// By default library will use 'void*' and 'std::shared_ptr<void>' types for general instancing,
// if you want to specify behaviour, just define own INSTANTIABLE_VOID_POINTER_TYPES
// Note that: any used ptr type should be provided in '::sf::memory' namespace
#ifndef INSTANTIABLE_VOID_POINTER_TYPES
    #define INSTANTIABLE_VOID_POINTER_TYPES void*, std::shared_ptr<void>
#endif // INSTANTIABLE_VOID_POINTER_TYPES

namespace sf
{

namespace dynamic
{

template <typename... VoidPointerTypes>
struct instantiable_registry_t
{
public:
    using instantiable_type = INSTANTIABLE_TYPE;

public:
    template <typename VoidPointerType>
    struct instantiable_proxy_overload_t
    {
        using instantiable_pointer_type = typename memory::pointer_traits<VoidPointerType>::template pointer_template<instantiable_type>;

        instantiable_pointer_type(*xxclone)() = nullptr;
        instantiable_pointer_type(*xxcast)(VoidPointerType const&) = nullptr;
    };

    struct instantiable_proxy_t : instantiable_proxy_overload_t<VoidPointerTypes>...
    {
        ::xxsf_instantiable_traits_key_type key = ::xxsf_instantiable_traits_base_key;

        void(*save)(ioarchive_t&, instantiable_type*) = nullptr;
        void(*load)(ioarchive_t&, instantiable_type*) = nullptr;
    };

public:
    std::unordered_map<::xxsf_instantiable_traits_key_type, instantiable_proxy_t> all;
    std::unordered_map<std::uint64_t, instantiable_proxy_t> rtti_all;

public:
    template <typename InstantiableType> struct is_instantiable
        : meta::one<meta::is_static_castable<InstantiableType*, instantiable_type*>,
                    std::is_convertible<InstantiableType*, instantiable_type*>> {};

private:
    template <class InstantiableType>
    void add_impl(instantiable_proxy_t&) { /*pass*/ }

    template <class InstantiableType, typename OtherVoidPointerType, typename... OtherVoidPointerTypes>
    void add_impl(instantiable_proxy_t& proxy)
    {
        proxy.instantiable_proxy_overload_t<OtherVoidPointerType>::xxclone = []
        {
            typename memory::pointer_traits<OtherVoidPointerType>::template pointer_template<InstantiableType> pointer = nullptr;
            memory::allocate(pointer);
            return memory::static_pointer_cast<instantiable_type>(pointer);
        };

        proxy.instantiable_proxy_overload_t<OtherVoidPointerType>::xxcast = [](OtherVoidPointerType const& address)
        {
            return memory::static_pointer_cast<instantiable_type>(memory::static_pointer_cast<InstantiableType>(address));
        };

        add_impl<InstantiableType, OtherVoidPointerTypes...>(proxy);
    }

public:
    template <class InstantiableType, SF_REQUIRES(meta::negation<is_instantiable<InstantiableType>>::value)>
    void add() { /*pass*/ }

    template <class InstantiableType, SF_REQUIRES(is_instantiable<InstantiableType>::value)>
    void add()
    {
        static auto lock = false; if (lock) return;
        lock = true;

        auto const key = ::xxsf_instantiable_traits<InstantiableType>::key();
    #ifdef SF_DEBUG
        if (key == ::xxsf_instantiable_traits_base_key)
            throw "The 'sf::dynamic::instantiable_registry_t' must contains instance with valid key.";
    #endif // SF_DEBUG
        instantiable_proxy_t proxy;

        add_impl<InstantiableType, VoidPointerTypes...>(proxy);

        proxy.key = key;

        proxy.save = [](ioarchive_t& archive, instantiable_type* instance)
        {
            archive << *memory::dynamic_pointer_cast<InstantiableType>(instance);
        };

        proxy.load = [](ioarchive_t& archive, instantiable_type* instance)
        {
            archive >> *memory::dynamic_pointer_cast<InstantiableType>(instance);
        };

        all.emplace(key, proxy);

        auto const hash = SF_TYPE_HASH(InstantiableType);
        rtti_all.emplace(hash, proxy);
    }

    template <class InstantiableType>
    bool fixture()
    {
    #ifdef SF_DEBUG
        if (meta::negation<is_instantiable<InstantiableType>>::value)
            throw "The polymorphic 'InstantiableType' type is not convertible to 'instantiable_t'.";
    #endif // SF_DEBUG

        if (all.find(::xxsf_instantiable_traits<InstantiableType>::key()) != all.end())
        #ifdef SF_DEBUG
            throw "The 'sf::dynamic::instantiable_registry_t' must contains instance with unique key.";
        #else
            return false;
        #endif // SF_DEBUG

        add<InstantiableType>();
        return true;
    }

    template <typename PointerType>
    void save(ioarchive_t& archive, PointerType& pointer) const
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto raw_pointer = memory::raw(pointer);
        auto const hash = SF_EXPRESSION_HASH(*raw_pointer);

        rtti_all.at(hash).save(archive, raw_pointer);
    }

    template <typename PointerType, typename VoidPointerType>
    void load(ioarchive_t& archive, PointerType& pointer, ::xxsf_instantiable_traits_key_type key, VoidPointerType& cache) const
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        clone(pointer, key);
        cache = memory::pure(pointer);

        auto raw_pointer = memory::raw(pointer);
        auto const hash = SF_EXPRESSION_HASH(*raw_pointer);

        rtti_all.at(hash).load(archive, raw_pointer);
    }

    public:
    template <typename PointerType>
    void clone(PointerType& pointer, ::xxsf_instantiable_traits_key_type key) const
    {
        using pointer_traits = memory::pointer_traits<PointerType>;
        using serializable_type = typename pointer_traits::element_type;
        using void_pointer_type = typename pointer_traits::template pointer_template<void>;

        pointer = memory::dynamic_pointer_cast<serializable_type>
        (
            all.at(key).instantiable_proxy_overload_t<void_pointer_type>::xxclone()
        );
    }

    template <typename PointerType, typename VoidPointerType>
    void cast(PointerType& pointer, VoidPointerType const& address, ::xxsf_instantiable_traits_key_type key) const
    {
        using pointer_traits = memory::pointer_traits<PointerType>;
        using serializable_type = typename pointer_traits::element_type;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        pointer = memory::dynamic_pointer_cast<serializable_type>
        (
            all.at(key).instantiable_proxy_overload_t<VoidPointerType>::xxcast(address)
        );
    }
};

extern instantiable_registry_t<INSTANTIABLE_VOID_POINTER_TYPES> instantiable_registry;

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
