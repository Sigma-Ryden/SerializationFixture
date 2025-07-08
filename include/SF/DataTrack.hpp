#ifndef SF_DATA_TRACK_HPP
#define SF_DATA_TRACK_HPP

#include <memory> // addressof

#include <SF/Core/ArchiveBase.hpp>

#include <SF/Dynamic/InstantiableTraits.hpp>
#include <SF/Dynamic/InstantiableRegistry.hpp>

#include <SF/ApplyFunctor.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace detail
{

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       meta::is_pointer_to_any<PointerType>>::value)>
std::uintptr_t tracking_key(ArchiveType& archive, PointerType& pointer)
{
    auto const pure = memory::pure(pointer);
    auto key = reinterpret_cast<std::uintptr_t>(memory::raw(pure));

    archive & key;
    return key;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       meta::is_pointer_to_any<PointerType>>::value)>
std::uintptr_t tracking_key(ArchiveType& archive, PointerType& pointer)
{
#ifdef SF_DEBUG
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_DEBUG

    std::uintptr_t key{};
    archive & key;

    return key;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       meta::is_pointer_to_polymorphic<PointerType>>::value)>
static ::xxsf_instantiable_traits_key_type instantiable_key(ArchiveType& archive, PointerType& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer was not allocated.";

    auto const hash = SF_EXPRESSION_HASH(*pointer);

    auto key = dynamic::instantiable_registry.rtti_all.at(hash).key;
    archive & key;

    return key;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       meta::is_pointer_to_polymorphic<PointerType>>::value)>
static ::xxsf_instantiable_traits_key_type instantiable_key(ArchiveType& archive, PointerType& pointer)
{
#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    ::xxsf_instantiable_traits_key_type key{};
    archive & key;

    return key;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       meta::is_pointer_to_polymorphic<PointerType>>::value)>
void native_save(ArchiveType& archive, PointerType& pointer, ::xxsf_instantiable_traits_key_type track_key)
{
    instantiable_key(archive, pointer); // write class info
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       std::negation<meta::is_pointer_to_polymorphic<PointerType>>>::value)>
void native_save(ArchiveType&, PointerType&, ::xxsf_instantiable_traits_key_type) noexcept { /*pass*/ }

template <class ArchiveType, typename PointerType, typename VoidPointerType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       meta::is_pointer_to_polymorphic<PointerType>>::value)>
void native_load(ArchiveType& archive, PointerType& pointer, VoidPointerType& address)
{
    auto const key = instantiable_key(archive, pointer);
    dynamic::instantiable_registry.cast(pointer, address, key);
}

template <class ArchiveType, typename PointerType, typename VoidPointerType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       std::negation<meta::is_pointer_to_polymorphic<PointerType>>>::value)>
void native_load(ArchiveType&, PointerType& pointer, VoidPointerType const& address) noexcept
{
    pointer = memory::static_pointer_cast<typename memory::pointer_traits<PointerType>::element_type>(address);
}

} // namespace detail

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       meta::is_pointer_to_standard_layout<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer must be allocated.";

    archive & (*pointer);
}

template <class ArchiveType, typename PointerType, typename VoidPointerType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       meta::is_pointer_to_standard_layout<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer, VoidPointerType& cache)
{
#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    memory::allocate(pointer);
    cache = memory::pure(pointer);

    archive & (*pointer);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       meta::is_pointer_to_polymorphic<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    detail::instantiable_key(archive, pointer);
    dynamic::instantiable_registry.save(archive, pointer);
}

template <class ArchiveType, typename PointerType, typename VoidPointerType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       meta::is_pointer_to_polymorphic<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer, VoidPointerType& cache)
{
    auto const key = detail::instantiable_key(archive, pointer);
    dynamic::instantiable_registry.load(archive, pointer, key, cache);
}

// verison without cache using
template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       meta::is_pointer_to_any<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    typename memory::pointer_traits<PointerType>::template pointer_template<void> cache = nullptr; // mock
    strict(archive, pointer, cache);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       meta::is_pointer_to_any<PointerType>>::value)>
void track(ArchiveType& archive, PointerType& pointer)
{
    auto const key = detail::tracking_key(archive, pointer); // serialize refer info
    if (not key) return;

#ifdef SF_DEBUG
    if (archive.tracking().is_mixed(key, pointer))
        throw "Mixed pointer tracking is not allowed.";
#endif // SF_DEBUG

    auto& is_tracking = archive.tracking().pointer(pointer)[key];

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

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(std::conjunction<meta::is_oarchive<ArchiveType>,
                                       std::negation<meta::is_pointer_to_any<SerializableType>>>::value)>
void track(ArchiveType& archive, SerializableType& data)
{
    auto const pointer = std::addressof(data);
    auto const address = memory::pure(pointer);
    auto key = reinterpret_cast<std::uintptr_t>(address);

    auto& is_tracking = archive.tracking().pointer(pointer)[key];

    if (is_tracking)
        throw "The write tracking data is already tracked.";

    is_tracking = true;

    archive & key;
    archive & data;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       meta::is_pointer_to_any<PointerType>>::value)>
void track(ArchiveType& archive, PointerType& pointer)
{
#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read track pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    auto const key = detail::tracking_key(archive, pointer); // serialize refer info
    if (not key) return;

    auto& address = archive.tracking().pointer(pointer)[key];

    if (address == nullptr)
    {
        // call the strict serialization of not tracking pointer
        strict(archive, pointer, address);
    }
    else
    {
        detail::native_load(archive, pointer, address);
    }
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(std::conjunction<meta::is_iarchive<ArchiveType>,
                                       std::negation<meta::is_pointer_to_any<SerializableType>>>::value)>
void track(ArchiveType& archive, SerializableType& data)
{
    std::uintptr_t key{};
    archive & key;

    auto const pointer = std::addressof(data);
    auto& address = archive.tracking().pointer(pointer)[key];

    if (address != nullptr)
        throw  "The read tracking data is already tracked.";

    address = memory::pure(pointer);

    archive & data;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<PointerType>,
                                       meta::is_pointer_to_any<PointerType>>::value)>
void raw(ArchiveType& archive, PointerType& pointer)
{
    if (detail::tracking_key(archive, pointer)) // serialize refer info
        strict(archive, pointer);
}

namespace apply
{

template <typename SerializableType>
struct strict_functor_t : public apply_functor_t
{
    SerializableType& data;

    strict_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { strict(archive, data); }
};

template <typename SerializableType>
struct track_functor_t : apply_functor_t
{
    SerializableType& data;

    track_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { track(archive, data); }
};

template <typename SerializableType>
struct raw_functor_t : apply_functor_t
{
    SerializableType& data;

    raw_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { raw(archive, data); }
};

} // namespace apply

template <typename SerializableType>
apply::strict_functor_t<SerializableType> strict(SerializableType& data) noexcept { return { data }; }

template <typename SerializableType>
apply::track_functor_t<SerializableType> track(SerializableType& data) noexcept { return { data }; }

template <typename SerializableType>
apply::raw_functor_t<SerializableType> raw(SerializableType& data) noexcept { return { data }; }

} // namespace sf

#endif // SF_DATA_TRACK_HPP
