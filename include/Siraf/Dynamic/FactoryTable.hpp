#ifndef SIRAF_DYNAMIC_FACTORY_TABLE_HPP
#define SIRAF_DYNAMIC_FACTORY_TABLE_HPP

#include <unordered_map> // unordered_map

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/Memory.hpp>

#include <Siraf/Dynamic/Cloneable.hpp>

namespace siraf
{

namespace dynamic
{

class FactoryTable
{
public:
    using clone_type = Cloneable::clone_type;
    using key_type   = Cloneable::key_type;

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
        return ::Serialization::is_pointer_cast_allowed<T, Cloneable>();
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
        if (not has_factory(key)) factory_[key] = Memory::allocate_raw<T>();
    }

    template <typename TraitType,
              SIREQUIRE(meta::is_memory_shared<TraitType>())>
    std::shared_ptr<clone_type> clone(key_type key)
    {
        return factory(key)->__shared();
    }

    template <typename TraitType,
              SIREQUIRE(meta::is_memory_raw<TraitType>())>
    clone_type* clone(key_type key)
    {
        return factory(key)->__raw();
    }

    std::shared_ptr<clone_type> cast(std::shared_ptr<void> address, key_type key)
    {
        return cast<Memory::shared_ptr>(address, key);
    }

    clone_type* cast(void* address, key_type key)
    {
        return cast<Memory::raw_ptr>(address, key);
    }

    bool has_factory(key_type key)
    {
        return factory_.find(key) != factory_.end();
    }

private:
    template <template <typename...> class PointerWrapper>
    PointerWrapper<clone_type> cast(PointerWrapper<void> address, key_type key)
    {
        return factory(key)->__cast(address);
    }

    clone_type* factory(key_type key)
    {
        auto it = factory_.find(key);
        if (it == factory_.end())
            throw "The 'siraf::dynamic::FactoryTable' does not has clone instance with input key.";

        return it->second;
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

        auto key = ::Serialization::template trait<T>();

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
