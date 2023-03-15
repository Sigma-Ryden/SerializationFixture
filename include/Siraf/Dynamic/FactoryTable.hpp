#ifndef SIRAF_DYNAMIC_FACTORY_TABLE_HPP
#define SIRAF_DYNAMIC_FACTORY_TABLE_HPP

#include <unordered_map> // unordered_map

#include <Siraf/Core/Access.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>
#include <Siraf/Core/TypeCore.hpp>

#include <Siraf/Memory/Memory.hpp>

#define CLONEABLE_BODY(...)                                                                             \
    friend class ::siraf::dynamic::FactoryTable;                                                        \
    private:                                                                                            \
    siraf::dynamic::FactoryRegistry<__VA_ARGS__> __registry;                                            \
    static constexpr key_type __static_trait() noexcept                                                 \
    { return SIRAF_STATIC_HASH(#__VA_ARGS__); }                                                         \
    key_type __trait() const noexcept                                                                   \
    { return siraf::core::Access::trait<__VA_ARGS__>(); }                                               \
    std::shared_ptr<clone_type> __shared() const                                                        \
    { return siraf::memory::allocate_shared<clone_type, __VA_ARGS__>(); }                               \
    std::shared_ptr<clone_type> __cast(std::shared_ptr<void> address) const                             \
    { return siraf::memory::static_pointer_cast<clone_type, __VA_ARGS__>(address); }                    \
    clone_type* __raw() const                                                                           \
    { return siraf::memory::allocate_raw<clone_type, __VA_ARGS__>(); }                                  \
    clone_type* __cast(void* address) const                                                             \
    { return siraf::memory::static_pointer_cast<clone_type, __VA_ARGS__>(address); }                    \
    public:

#ifndef CLONEABLE
    #define CLONEABLE(...)                                                                              \
        public siraf::dynamic::Cloneable
#endif // CLONEABLE

namespace siraf
{

namespace dynamic
{

class Cloneable; // need for type alias

struct FactoryTableBase
{
    using key_type   = core::PolymorphicTraitBase::key_type;
    using clone_type = Cloneable;
};

class FactoryTable; // ned for factory access

class Cloneable
{
    friend class FactoryTable;

public:
    using key_type   = FactoryTableBase::key_type;
    using clone_type = FactoryTableBase::clone_type;

public:
    virtual ~Cloneable() = default;

private:
    virtual key_type __trait() const noexcept = 0;

    virtual std::shared_ptr<clone_type> __shared() const = 0;
    virtual std::shared_ptr<clone_type> __cast(std::shared_ptr<void> address) const = 0;

    virtual clone_type* __raw() const = 0;
    virtual clone_type* __cast(void* address) const = 0;
};

class FactoryTable
{
public:
    using key_type   = FactoryTableBase::key_type;
    using clone_type = FactoryTableBase::clone_type;

    using InnerTable = std::unordered_map<key_type, clone_type*>;

private:
    InnerTable factory_;

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
        return core::Access::is_convertible<T*, Cloneable*>();
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
    std::shared_ptr<clone_type> clone(key_type key)
    {
        auto it = factory_.find(key);
        return it != factory_.end() ? it->second->__shared() : nullptr;
    }

    template <typename TraitType,
              SIREQUIRE(meta::is_memory_raw<TraitType>())>
    clone_type* clone(key_type key)
    {
        auto it = factory_.find(key);
        return it != factory_.end() ? it->second->__raw() : nullptr;
    }

    std::shared_ptr<clone_type> cast(std::shared_ptr<void> address, key_type key)
    {
        return cast<memory::shared_ptr>(address, key);
    }

    clone_type* cast(void* address, key_type key)
    {
        return cast<memory::raw_ptr>(address, key);
    }

    bool has_factory(key_type key)
    {
        return factory_.find(key) != factory_.end();
    }

private:
    template <template <typename...> class PointerWrapper>
    PointerWrapper<clone_type> cast(PointerWrapper<void> address, key_type key)
    {
        auto it = factory_.find(key);
        return it != factory_.end() ? it->second->__cast(address) : nullptr;
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
    template <typename dT = T, SIREQUIRE(meta::is_polymorphic<dT>())>
    void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto key = core::Access::template trait<T>();

    #ifdef SIRAF_DEBUG
        if (FactoryTable::instance().has_factory(key))
            throw "The 'siraf::dynamic::FactoryTable' must contains clone instance with unique key.";
    #endif // SIRAF_DEBUG

        FactoryTable::instance().update<T>(key);
    }

    template <typename dT = T, SIREQUIRE(not meta::is_polymorphic<dT>())>
    void call() { /*pass*/ }
};

template <class T>
bool FactoryRegistry<T>::lock_ = false;

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_FACTORY_TABLE_HPP
