#ifndef SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
#define SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP

#include <unordered_map> // unordered_map

#include <SerializationFixture/Core/Serialization.hpp>
#include <SerializationFixture/Core/Memory.hpp>

#include <SerializationFixture/Core/Hash.hpp>

#include <SerializationFixture/Core/PolymorphicArchive.hpp>

#include <SerializationFixture/Dynamic/Instantiable.hpp>
#include <SerializationFixture/Dynamic/InstantiableTraits.hpp>

namespace sf
{

namespace dynamic
{

struct instantiable_registry_t
{
public:
    using instantiable_type = INSTANTIABLE_TYPE;

public:
    struct instantiable_proxy_t
    {
        ::xxsf_instantiable_traits_key_type key = ::xxsf_instantiable_traits_base_key;

        std::shared_ptr<instantiable_type>(*shared)() = nullptr;
        std::shared_ptr<instantiable_type>(*cast_shared)(std::shared_ptr<void> const&) = nullptr;

        instantiable_type*(*raw)() = nullptr;
        instantiable_type*(*cast_raw)(void*) = nullptr;

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

        proxy.key = key;

        proxy.shared = []
        {
            std::shared_ptr<InstantiableType> pointer = nullptr;
            memory::allocate(pointer);
            return memory::static_pointer_cast<instantiable_type>(pointer);
        };

        proxy.cast_shared = [](std::shared_ptr<void> const& address)
        {
            return memory::static_pointer_cast<instantiable_type>(memory::static_pointer_cast<InstantiableType>(address));
        };

        proxy.raw = []
        {
            InstantiableType* pointer = nullptr;
            memory::allocate(pointer);
            return memory::static_pointer_cast<instantiable_type>(pointer);
        };

        proxy.cast_raw = [](void* address)
        {
            return memory::static_pointer_cast<instantiable_type>(memory::static_pointer_cast<InstantiableType>(address));
        };

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

    // raw ptr
    public:
    template <typename InstantiableType>
    void clone(InstantiableType*& pointer, ::xxsf_instantiable_traits_key_type key) const
    {
        pointer = memory::dynamic_pointer_cast<InstantiableType>(all.at(key).raw());
    }

    template <typename InstantiableType>
    void cast(InstantiableType*& pointer, void* address, ::xxsf_instantiable_traits_key_type key) const
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        pointer = memory::dynamic_pointer_cast<InstantiableType>(all.at(key).cast_raw(address));
    }

    // std shared ptr
    public:
    template <typename InstantiableType>
    void clone(std::shared_ptr<InstantiableType>& pointer, ::xxsf_instantiable_traits_key_type key) const
    {
        pointer = memory::dynamic_pointer_cast<InstantiableType>(all.at(key).shared());
    }

    template <typename InstantiableType>
    void cast(std::shared_ptr<InstantiableType>& pointer, std::shared_ptr<void> const& address, ::xxsf_instantiable_traits_key_type key) const
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        pointer = memory::dynamic_pointer_cast<InstantiableType>(all.at(key).cast_shared(address));
    }
};

extern instantiable_registry_t instantiable_registry;

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
