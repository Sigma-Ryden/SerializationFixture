#ifndef SF_ALIAS_HPP
#define SF_ALIAS_HPP

#include <memory> // addressof

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/ExternSerialization.hpp>
#include <SF/DataTrack.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

template <typename T>
class alias
{
private:
    T* data_;

public:
    using type = T;

    // DONT use dereferencing of null data before rebinding
    alias() noexcept : data_(nullptr) {}

    template <typename dT,
              SFREQUIRE(::Serialization::is_pointer_cast_allowed<dT, T>::value)>
    alias(dT& data) noexcept : data_(std::addressof(data)) {}

    template <typename dT>
    alias(const alias<dT>& data) noexcept : alias(data.get()) {}

public:
    // rebinding data
    alias(const alias&) = default;
    alias& operator=(const alias&) = default;

    bool is_refer() const noexcept { return data_ == nullptr; }

    template <typename dT>
    bool is_refer(dT& data)  const noexcept { return data_ == std::addressof(data); }

    operator T&() const noexcept { return get(); }

    T& get() const noexcept { return *data_; }
    void set(T& data) noexcept { data_ = std::addressof(data); }
};

namespace meta
{

template <typename> struct is_alias : std::false_type {};
template <typename T>
struct is_alias<alias<T>> : std::true_type {};

} // namespace meta

// inline namespace common also used in namespace library
inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, alias, meta::is_alias<T>::value)
{
    using key_type = typename Archive::TrackingKeyType;

    if (alias.is_refer())
        throw "The write alias must be initialized.";

    auto pointer = std::addressof(alias.get());
    auto address = Memory::pure(pointer);

    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.template tracking<tracking::Raw>()[key];

    if (not is_tracking)
        throw "The write alias must be tracked before.";

    detail::native_save(archive, pointer, key);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, alias, meta::is_alias<T>::value)
{
    using key_type   = typename Archive::TrackingKeyType;
    using value_type = typename T::type;

    using track_type = tracking::Raw;

    if (not alias.is_refer())
        throw "The read alias must be null.";

    key_type key;
    archive & key;

    auto& item = archive.template tracking<tracking::Raw>()[key];

    if (item.address == nullptr)
        throw "The read alias must be tracked before.";

    value_type* pointer = nullptr;

    detail::native_load(archive, pointer, item.address);

    alias.set(*pointer); // pointer will never nullptr

    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_alias<T>::value)

#endif // SF_ALIAS_HPP
