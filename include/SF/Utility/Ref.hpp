#ifndef SF_UTILITY_REF_HPP
#define SF_UTILITY_REF_HPP

#include <memory> // addressof

#include <SF/Core/TypeRegistry.hpp>

#include <SF/ExternSerialization.hpp>
#include <SF/DataTrack.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
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

    // DONT use dereferencing of null data before rebinding
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
struct is_ref<sf::utility::Ref<T>> : std::true_type {};

} // namespcae detail

template <class T> constexpr bool is_ref() noexcept
{
    return detail::is_ref<T>::value;
}

} // namespace meta

// inline namespace common also used in namespace library
inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, ref, meta::is_ref<T>())
{
    using key_type = typename Archive::TrackingKeyType;

    if (ref.is_null())
        throw "The write reference must be initialized.";

    auto pointer = std::addressof(ref.get());
    auto address = Memory::pure(pointer);

    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.tracking()[key];

    if (not is_tracking)
        throw "The write reference must be tracked before.";

    detail::native_save(archive, pointer, key);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, ref, meta::is_ref<T>())
{
    using key_type   = typename Archive::TrackingKeyType;
    using value_type = typename T::type;

    using track_type = tracking::Raw;

    if (not ref.is_null())
        throw "The read reference must be null.";

    key_type key;
    archive & key;

    auto& item = archive.template tracking<track_type>()[key];

    if (item.address == nullptr)
        throw "The read reference must be tracked before.";

    value_type* pointer = nullptr;

    detail::native_load(archive, pointer, item.address);

    ref.set(*pointer); // pointer will never nullptr

    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_ref<T>())

#endif // SF_UTILITY_REF_HPP
