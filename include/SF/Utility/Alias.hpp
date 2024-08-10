#ifndef SF_UTILITY_ALIAS_HPP
#define SF_UTILITY_ALIAS_HPP

#include <memory> // addressof

#include <SF/Core/SerializatonBase.hpp>

#include <SF/DataTrack.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

template <typename ElementType>
class alias_t
{
private:
    ElementType* data_;

public:
    using element_type = ElementType;

    // DONT use dereferencing of null data before rebinding
    alias_t() noexcept : data_(nullptr) {}

    template <typename OtherElementType,
              SF_REQUIRES(meta::is_static_castable<OtherElementType*, element_type*>::value)>
    alias_t(OtherElementType& data) noexcept
        : data_(std::addressof(data)) {}

    template <typename OtherElementType>
    alias_t(alias_t<OtherElementType> const& data) noexcept : alias_t(data.get()) {}

public:
    // rebinding data
    alias_t(alias_t const&) = default;
    alias_t& operator=(alias_t const&) = default;

    bool is_refer() const noexcept { return data_ != nullptr; }

    template <typename OtherElementType>
    bool is_refer(OtherElementType& data)  const noexcept { return data_ == std::addressof(data); }

    operator element_type&() const noexcept { return get(); }

    element_type& get() const noexcept { return *data_; }
    void set(element_type& data) noexcept { data_ = std::addressof(data); }
};

} // namespace sf

TEMPLATE_SERIALIZATION(save, alias, template <typename ElementType>, ::sf::alias_t<ElementType>)
{
    using key_type = typename ArchiveType::TrackingKeyType;

    if (not alias.is_refer())
        throw "The write alias_t must be initialized.";

    auto pointer = std::addressof(alias.get());
    auto const key = ::sf::detail::refer_key(archive, pointer);

    auto& is_tracking = archive.template tracking<::sf::tracking::raw_t>()[key];

    if (not is_tracking)
        throw "The write alias_t must be tracked before.";

    ::sf::detail::native_save(archive, pointer, key);
}

TEMPLATE_SERIALIZATION(load, alias, template <typename ElementType>, ::sf::alias_t<ElementType>)
{
    using key_type = typename ArchiveType::TrackingKeyType;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (alias.is_refer())
        throw "The read alias_t must be null.";
#endif // SF_GARBAGE_CHECK_DISABLE

    key_type key{};
    archive & key;

    auto& item = archive.template tracking<::sf::tracking::raw_t>()[key];

    if (item.address == nullptr)
        throw "The read alias_t must be tracked before.";

    ElementType* pointer = nullptr;

    ::sf::detail::native_load(archive, pointer, item.address);

    alias.set(*pointer); // pointer will never nullptr
}

#endif // SF_UTILITY_ALIAS_HPP
