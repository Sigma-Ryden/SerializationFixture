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

    template <typename dT,
              SF_REQUIRE(meta::is_static_castable<dT*, element_type*>::value)>
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
    const auto key = ::sf::detail::refer_key(archive, pointer);

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
