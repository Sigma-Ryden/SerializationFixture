#ifndef SIFAR_REF_HPP
#define SIFAR_REF_HPP

#include <memory> // addressof

#include <Sifar/ReadArchive.hpp>
#include <Sifar/WriteArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/DataTrack.hpp>

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace utility
{

template <typename T>
class Ref
{
private:
    T* data_;

public:
    using type = T;

    // DONT use dereferencing of null data before rebining
    Ref() noexcept : data_(nullptr) {}
    Ref(T& data) noexcept : data_(std::addressof(data)) {}

    // rebinding data
    Ref(const Ref&) = default;
    Ref& operator=(const Ref&) = default;

    bool is_null() const noexcept { return data_ == nullptr; }

    operator T&() const noexcept { return get(); }

    T& get() const noexcept { return *data_; }
    void set(T& data) noexcept { data_ = std::addressof(data); }
};

} // namespace utility

template <typename T>
utility::Ref<T> ref() noexcept { return {}; }

template <typename To, typename From>
utility::Ref<To> ref(From& data) noexcept { return { data }; }

template <typename T>
utility::Ref<T> ref(T& data) noexcept { return { data }; }

namespace meta
{

namespace detail
{

template <typename> struct is_ref : std::false_type {};
template <typename T>
struct is_ref<sifar::utility::Ref<T>> : std::true_type {};

} // namespcae detail

template <class T> constexpr bool is_ref() noexcept
{
    return detail::is_ref<T>::value;
}

} // namespace meta

// inline namespace common also used in namespace library
inline namespace common
{

SERIALIZATION_SAVE_DATA(ref, meta::is_ref<T>())
{
    using key_type = typename WriteArchive::TrackingTable::key_type;

    auto pointer = std::addressof(ref.get());
    auto address = utility::pure(pointer);

    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.tracking()[key];

    if (not is_tracking)
        throw "the write reference must be tracked before.";

    detail::native_save(archive, pointer, key);

    return archive;
}

SERIALIZATION_LOAD_DATA(ref, meta::is_ref<T>())
{
    using key_type   = typename ReadArchive::TrackingTable::key_type;
    using value_type = typename T::type;

    key_type key;
    archive & key;

    auto& track_data = archive.tracking()[key];

    if (not track_data.is_tracking)
        throw "the read reference must be tracked before.";

    value_type* pointer = nullptr;

    detail::native_load(archive, pointer, track_data.address);

    ref.set(*pointer); // pointer will never nullptr

    return archive;
}

} // inline namespace common

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_ref<T>())

#endif // SIFAR_REF_HPP
