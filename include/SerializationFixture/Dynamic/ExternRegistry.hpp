#ifndef SF_DYNAMIC_REGISTRY_HPP
#define SF_DYNAMIC_REGISTRY_HPP

#include <SerializationFixture/Core/Serialization.hpp>
#include <SerializationFixture/Core/Memory.hpp>

#include <SerializationFixture/Dynamic/InstantiableRegistry.hpp>
#include <SerializationFixture/Dynamic/InstantiableTraits.hpp>
#include <SerializationFixture/Dynamic/AnyRegistry.hpp>

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
{

namespace dynamic
{

class extern_registry_t
{
public:
    template <class ArchiveType, typename PointerType,
              SF_REQUIRES(meta::is_pointer_to_polymorphic<PointerType>::value)>
    static ::xxsf_instantiable_traits_key_type save_key(ArchiveType& archive, PointerType& pointer)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto const hash = SF_EXPRESSION_HASH(*pointer);

        auto key = instantiable_registry.rtti_all.at(hash).key;
        archive & key;

        return key;
    }

    template <class ArchiveType, typename PointerType,
              SF_REQUIRES(meta::is_pointer_to_polymorphic<PointerType>::value)>
    static ::xxsf_instantiable_traits_key_type load_key(ArchiveType& archive, PointerType& pointer)
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        ::xxsf_instantiable_traits_key_type key{};
        archive & key;

        return key;
    }

private:
    template <typename PointerType> struct is_pointer_to_instantiable
        : meta::all<instantiable_registry_t::is_instantiable<typename memory::ptr_traits<PointerType>::item>,
                    meta::is_pointer_to_polymorphic<PointerType>> {};

public:
    template <typename PointerType,
              SF_REQUIRES(is_pointer_to_instantiable<PointerType>::value)>
    static void save(core::ioarchive_t& archive, PointerType& pointer, ::xxsf_instantiable_traits_key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto raw_pointer = memory::raw(pointer);
        instantiable_registry.save(archive, raw_pointer);
    }

    template <typename PointerType,
              SF_REQUIRES(meta::is_pointer_to_polymorphic<PointerType>::value)>
    static void load(core::ioarchive_t& archive, PointerType& pointer,
                     ::xxsf_instantiable_traits_key_type key, typename memory::ptr_traits<PointerType>::void_ptr& cache)
    {
        using PointerTraitsType = memory::ptr_traits<PointerType>;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto cloned = instantiable_registry.clone<typename PointerTraitsType::traits>(key);

        pointer = memory::dynamic_pointer_cast<typename PointerTraitsType::item>(cloned);
        cache = memory::pure(pointer);

        auto raw_pointer = memory::raw(pointer);
        instantiable_registry.load(archive, raw_pointer);
    }

    template <typename PointerType,
              SF_REQUIRES(meta::is_pointer_to_polymorphic<PointerType>::value)>
    static void assign(PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr const& address,
                       ::xxsf_instantiable_traits_key_type key)
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto casted = instantiable_registry.cast(address, key);
        pointer = memory::dynamic_pointer_cast<typename memory::ptr_traits<PointerType>::item>(casted);
    }
};

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_REGISTRY_HPP

