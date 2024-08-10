#ifndef SF_STRICT_HPP
#define SF_STRICT_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/ApplyFunctor.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer_to_standard_layout<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer must be allocated.";

    archive & (*pointer);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer_to_standard_layout<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr& cache)
{
    using item_type = typename memory::ptr_traits<PointerType>::item;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    memory::allocate<item_type>(pointer);
    cache = memory::pure(pointer);

    archive & (*pointer);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    auto& registry = archive.registry();

    const auto id = registry.save_key(archive, pointer);
    registry.save(archive, pointer, id);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr& cache)
{
    auto& registry = archive.registry();

    const auto id = registry.load_key(archive, pointer);
    registry.load(archive, pointer, id, cache);
}

// verison without cache using
template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_serializable_pointer<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    typename memory::ptr_traits<PointerType>::void_ptr cache = nullptr;
    strict(archive, pointer, cache);
}

namespace detail
{

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_serializable_pointer<PointerType>>::value)>
typename ArchiveType::TrackingKeyType refer_key(ArchiveType& archive, PointerType& pointer)
{
    auto pure = memory::pure(pointer);
    auto key = reinterpret_cast<typename ArchiveType::TrackingKeyType>(memory::raw(pure));

    archive & key;
    return key;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_serializable_pointer<PointerType>>::value)>
typename ArchiveType::TrackingKeyType refer_key(ArchiveType& archive, PointerType& pointer)
{
#ifdef SF_DEBUG
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_DEBUG

    typename ArchiveType::TrackingKeyType key{};
    archive & key;

    return key;
}

} // namespace detail

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

} // namespace apply

template <typename SerializableType>
apply::strict_functor_t<SerializableType>
strict(SerializableType& parameter) noexcept { return { parameter }; }

} // namespace sf

#endif // SF_STRICT_HPP
