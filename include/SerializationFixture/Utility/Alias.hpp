#ifndef SF_UTILITY_ALIAS_HPP
#define SF_UTILITY_ALIAS_HPP

#include <memory> // addressof

#include <SerializationFixture/DataTrack.hpp>

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
{

template <typename ElementType>
class alias_t
{
private:
    ElementType* xxdata;

public:
    using element_type = ElementType;

    // DONT use dereferencing of null data before rebinding
    alias_t() noexcept : xxdata(nullptr) {}

    template <typename OtherElementType,
              SF_REQUIRES(meta::is_static_castable<OtherElementType*, element_type*>::value)>
    alias_t(OtherElementType& data) noexcept : xxdata(std::addressof(data)) {}

    template <typename OtherElementType>
    alias_t(alias_t<OtherElementType> const& data) noexcept : alias_t(data.get()) {}

public:
    // rebinding data
    alias_t(alias_t const&) = default;
    alias_t& operator=(alias_t const&) = default;

    bool is_refer() const noexcept { return xxdata != nullptr; }

    template <typename OtherElementType>
    bool is_refer(OtherElementType& data)  const noexcept { return xxdata == std::addressof(data); }

    operator element_type&() const noexcept { return get(); }

    element_type& get() const noexcept { return *xxdata; }
    void set(element_type& data) noexcept { xxdata = std::addressof(data); }
};

} // namespace sf

TEMPLATE_SERIALIZATION(save, alias, template <typename ElementType>, ::sf::alias_t<ElementType>)
{
    if (not alias.is_refer())
        throw "The write alias_t must be initialized.";

    auto pointer = std::addressof(alias.get());
    auto const key = ::sf::detail::refer_key(archive, pointer);

    auto& is_tracking = archive.tracking().template pointer<ElementType*>()[key];

    if (not is_tracking)
        throw "The write alias_t must be tracked before.";

    ::sf::detail::native_save(archive, pointer, key);
}

TEMPLATE_SERIALIZATION(load, alias, template <typename ElementType>, ::sf::alias_t<ElementType>)
{
#ifndef SF_GARBAGE_CHECK_DISABLE
    if (alias.is_refer())
        throw "The read alias_t must be null.";
#endif // SF_GARBAGE_CHECK_DISABLE

    std::uintptr_t key{};
    archive & key;

    auto& address = archive.tracking().template pointer<ElementType*>()[key];

    if (address == nullptr)
        throw "The read alias_t must be tracked before.";

    ElementType* pointer = nullptr;

    ::sf::detail::native_load(archive, pointer, address);

    alias.set(*pointer); // pointer will never nullptr
}

#endif // SF_UTILITY_ALIAS_HPP
