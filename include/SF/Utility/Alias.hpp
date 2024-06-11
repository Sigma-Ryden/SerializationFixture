#ifndef SF_UTILITY_ALIAS_HPP
#define SF_UTILITY_ALIAS_HPP

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
class alias_t
{
private:
    T* data_;

public:
    using type = T;

    // DONT use dereferencing of null data before rebinding
    alias_t() noexcept : data_(nullptr) {}

    template <typename dT,
              SF_REQUIRE(::__sf::is_pointer_cast_allowed<dT, T>::value)>
    alias_t(dT& data) noexcept
        : data_(std::addressof(data)) {}

    template <typename dT>
    alias_t(const alias_t<dT>& data) noexcept : alias_t(data.get()) {}

public:
    // rebinding data
    alias_t(const alias_t&) = default;
    alias_t& operator=(const alias_t&) = default;

    bool is_refer() const noexcept { return data_ != nullptr; }

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
struct is_alias<alias_t<T>> : std::true_type {};

} // namespace meta

// inline namespace common also used in namespace library
inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, alias_t, meta::is_alias<T>::value)
{
    using key_type = typename Archive::TrackingKeyType;

    if (not alias_t.is_refer())
        throw "The write alias_t must be initialized.";

    auto pointer = std::addressof(alias_t.get());
    auto key = detail::refer_key(archive, pointer);

    auto& is_tracking = archive.template tracking<tracking::Raw>()[key];

    if (not is_tracking)
        throw "The write alias_t must be tracked before.";

    detail::native_save(archive, pointer, key);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, alias_t, meta::is_alias<T>::value)
{
    using key_type   = typename Archive::TrackingKeyType;
    using value_type = typename T::type;

    using track_type = tracking::Raw;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (alias_t.is_refer())
        throw "The read alias_t must be null.";
#endif // SF_GARBAGE_CHECK_DISABLE

    key_type key{};
    archive & key;

    auto& item = archive.template tracking<tracking::Raw>()[key];

    if (item.address == nullptr)
        throw "The read alias_t must be tracked before.";

    value_type* pointer = nullptr;

    detail::native_load(archive, pointer, item.address);

    alias_t.set(*pointer); // pointer will never nullptr

    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_alias<T>::value)

#endif // SF_UTILITY_ALIAS_HPP
