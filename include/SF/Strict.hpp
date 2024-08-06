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

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_pointer_to_standard_layout<T>>::value)>
void strict(ArchiveType& archive, T& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer must be allocated.";

    archive & (*pointer);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_pointer_to_standard_layout<T>>::value)>
void strict(ArchiveType& archive, T& pointer, memory::void_ptr<T>& cache)
{
    using item_type = typename memory::ptr_traits<T>::item;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    memory::allocate<item_type>(pointer);
    cache = memory::pure(pointer);

    archive & (*pointer);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void strict(ArchiveType& archive, T& pointer)
{
    auto& registry = archive.registry();

    const auto id = registry.save_key(archive, pointer);
    registry.save(archive, pointer, id);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void strict(ArchiveType& archive, T& pointer, memory::void_ptr<T>& cache)
{
    auto& registry = archive.registry();

    const auto id = registry.load_key(archive, pointer);
    registry.load(archive, pointer, id, cache);
}

// verison without cache using
template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_serializable_pointer<T>>::value)>
void strict(ArchiveType& archive, T& pointer)
{
    memory::void_ptr<T> cache = nullptr;
    strict(archive, pointer, cache);
}

namespace detail
{

template <class ArchiveType, typename T,
          typename KeyType = typename ArchiveType::TrackingKeyType,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_serializable_pointer<T>>::value)>
KeyType refer_key(ArchiveType& archive, T& pointer)
{
    auto pure = memory::pure(pointer);
    auto key = reinterpret_cast<KeyType>(memory::raw(pure));

    archive & key;
    return key;
}

template <class ArchiveType, typename T,
          typename KeyType = typename ArchiveType::TrackingKeyType,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_serializable_pointer<T>>::value)>
KeyType refer_key(ArchiveType& archive, T& pointer)
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
struct strict_functor_t : public apply_functor_t
{
    T& data;

    strict_functor_t(T& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { strict(archive, data); }
};

} // namespace apply

template <typename T>
apply::strict_functor_t<T> strict(T& parameter) noexcept { return { parameter }; }

} // namespace sf

#endif // SF_STRICT_HPP
