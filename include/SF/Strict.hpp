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

template <class WriteArchive, typename T,
          SFREQUIRE(meta::all<meta::is_write_archive<WriteArchive>,
                              meta::is_pointer_to_standard_layout<T>>::value)>
void strict(WriteArchive& archive, T& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer must be allocated.";

    archive & (*pointer);
}

template <class ReadArchive, typename T,
          SFREQUIRE(meta::all<meta::is_read_archive<ReadArchive>,
                              meta::is_pointer_to_standard_layout<T>>::value)>
void strict(ReadArchive& archive, T& pointer, Memory::void_ptr<T>& cache)
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

template <class WriteArchive, typename T,
          SFREQUIRE(meta::all<meta::is_write_archive<WriteArchive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void strict(WriteArchive& archive, T& pointer)
{
    auto& registry = archive.registry();

    auto id = registry.save_key(archive, pointer);
    registry.save(archive, pointer, id);
}

template <class ReadArchive, typename T,
          SFREQUIRE(meta::all<meta::is_read_archive<ReadArchive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void strict(ReadArchive& archive, T& pointer, Memory::void_ptr<T>& cache)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.load(archive, pointer, id, cache);
}

// verison without cache using
template <class ReadArchive, typename T,
          SFREQUIRE(meta::all<meta::is_read_archive<ReadArchive>,
                              meta::is_serializable_pointer<T>>::value)>
void strict(ReadArchive& archive, T& pointer)
{
    Memory::void_ptr<T> cache = nullptr;
    strict(archive, pointer, cache);
}

namespace detail
{

template <class WriteArchive, typename T,
          typename KeyType = typename WriteArchive::TrackingKeyType,
          SFREQUIRE(meta::all<meta::is_write_archive<WriteArchive>,
                              meta::is_serializable_pointer<T>>::value)>
KeyType refer_key(WriteArchive& archive, T& pointer)
{
    auto pure = Memory::pure(pointer);
    auto key = reinterpret_cast<KeyType>(Memory::raw(pure));

    archive & key;
    return key;
}

template <class ReadArchive, typename T,
          typename KeyType = typename ReadArchive::TrackingKeyType,
          SFREQUIRE(meta::all<meta::is_read_archive<ReadArchive>,
                              meta::is_serializable_pointer<T>>::value)>
KeyType refer_key(ReadArchive& archive, T& pointer)
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

#endif // SF_STRICT_HPP
