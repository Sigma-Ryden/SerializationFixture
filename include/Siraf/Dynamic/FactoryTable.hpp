#ifndef SIRAF_FACTORY_TABLE_HPP
#define SIRAF_FACTORY_TABLE_HPP

#include <unordered_map> // unordered_map

#include <Siraf/Dynamic/PolymorphicTrait.hpp>

#include <Siraf/Access.hpp>
#include <Siraf/Utility.hpp>

#include <Siraf/Memory/Memory.hpp>

#define _CLONEABLE_KEY_CALL(...)                                                                        \
    { return SIRAF_STATIC_HASH(#__VA_ARGS__); }

#define _CLONEABLE_KEY_IMPLEMENTATION(...)                                                              \
    static constexpr key_type __static_trait() noexcept                                                 \
    _CLONEABLE_KEY_CALL(__VA_ARGS__)                                                                    \
    key_type __trait() const noexcept                                                                   \
    { return siraf::Access::static_trait<__VA_ARGS__>(); }

#define _CLONEABLE_IMPLEMENTATION(...)                                                                  \
    siraf::memory::shared_ptr<clone_type> __shared() const                                              \
    { return siraf::memory::allocate_shared<clone_type, __VA_ARGS__>(); }                               \
    siraf::memory::shared_ptr<clone_type> __cast(siraf::memory::shared_ptr<void> address) const         \
    { return siraf::memory::static_pointer_cast<clone_type, __VA_ARGS__>(address); }                    \
    siraf::memory::raw_ptr<clone_type> __raw() const                                                    \
    { return siraf::memory::allocate_raw<clone_type, __VA_ARGS__>(); }                                  \
    siraf::memory::raw_ptr<clone_type> __cast(siraf::memory::raw_ptr<void> address) const               \
    { return siraf::memory::static_pointer_cast<clone_type, __VA_ARGS__>(address); }

#define _CLONEABLE_BODY(...)                                                                            \
    private:                                                                                            \
    _CLONEABLE_IMPLEMENTATION(__VA_ARGS__)                                                              \
    _CLONEABLE_KEY_IMPLEMENTATION(__VA_ARGS__)                                                          \
    public:

namespace siraf
{

namespace dynamic
{

class Cloneable; // need for type alias

struct FactoryTableCore
{
    using key_type   = PolymorphicTraitCore::key_type;
    using clone_type = Cloneable;
};

class FactoryTable; // ned for factory access

class Cloneable
{
    friend class FactoryTable;

public:
    using key_type   = FactoryTableCore::key_type;
    using clone_type = FactoryTableCore::clone_type;

    template <typename T>
    using shared_ptr = memory::shared_ptr<T>;

    template <typename T>
    using raw_ptr    = memory::raw_ptr<T>;

public:
    virtual ~Cloneable() = default;

private:
    virtual key_type __trait() const noexcept = 0;

    virtual shared_ptr<clone_type> __shared() const = 0;
    virtual shared_ptr<clone_type> __cast(shared_ptr<void> address) const = 0;

    virtual raw_ptr<clone_type> __raw() const = 0;
    virtual raw_ptr<clone_type> __cast(raw_ptr<void> address) const = 0;
};

class FactoryTable
{
public:
    using key_type   = FactoryTableCore::key_type;
    using clone_type = FactoryTableCore::clone_type;

    using InnerTabel = std::unordered_map<key_type, clone_type*>;

private:
    InnerTabel factory_;

private:

    FactoryTable() : factory_() {}

    ~FactoryTable()
    {
        for (const auto& pair : factory_) delete pair.second;
    }

    FactoryTable(const FactoryTable&) = delete;
    FactoryTable& operator=(const FactoryTable&) = delete;

public:
    static FactoryTable& instance() noexcept
    {
        static FactoryTable self;
        return self;
    }

private:
    template <typename T> static constexpr bool is_cloneable() noexcept
    {
        return meta::is_convertible<T, Cloneable>();
    }

public:
    template <class T, SIREQUIRE(not is_cloneable<T>())>
    void update(key_type key)
    {
        throw "The polymorphic 'T' type is not convertible to 'siraf::dynamic::Cloneable'.";
    }

    template <class T, SIREQUIRE(is_cloneable<T>())>
    void update(key_type key)
    {
        if (not has_factory(key)) factory_[key] = memory::allocate_raw<T>();
    }

    template <typename TraitType,
              SIREQUIRE(meta::is_memory_shared<TraitType>())>
    memory::shared_ptr<clone_type> clone(key_type key)
    {
        return has_factory(key) ? factory_[key]->__shared() : nullptr;
    }

    template <typename TraitType,
              SIREQUIRE(meta::is_memory_raw<TraitType>())>
    memory::raw_ptr<clone_type> clone(key_type key)
    {
        return has_factory(key) ? factory_[key]->__raw() : nullptr;
    }

    bool has_factory(key_type key) const noexcept
    {
        return factory_.find(key) != factory_.end();
    }

    memory::shared_ptr<clone_type> cast(memory::shared_ptr<void> address, key_type key)
    {
        return cast<memory::shared_ptr>(address, key);
    }

    memory::raw_ptr<clone_type> cast(memory::raw_ptr<void> address, key_type key)
    {
        return cast<memory::raw_ptr>(address, key);
    }

private:
    template <template <typename...> class PointerWrapper>
    PointerWrapper<clone_type> cast(PointerWrapper<void> address, key_type key)
    {
        return has_factory(key) ? factory_[key]->__cast(address) : nullptr;
    }
};

template <class T>
class FactoryRegistry
{
private:
    static bool lock_;

public:
    FactoryRegistry()
    {
        call<T>();
    }

private:
    template <typename U = T, SIREQUIRE(meta::is_polymorphic<U>())>
    void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto key = Access::template static_trait<T>();

    #ifdef SIRAF_DEBUG
        if (FactoryTable::instance().has_factory(key))
            throw "The 'siraf::dynamic::FactoryTable' must contain clone instance with unique key.";
    #endif // SIRAF_DEBUG

        FactoryTable::instance().update<T>(key);
    }

    template <typename U = T, SIREQUIRE(not meta::is_polymorphic<U>())>
    void call() { /*pass*/ }
};

template <class T>
bool FactoryRegistry<T>::lock_ = false;

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_FACTORY_TABLE_HPP
