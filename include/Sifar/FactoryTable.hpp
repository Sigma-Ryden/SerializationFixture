#ifndef SIFAR_FACTORY_TABLE_HPP
#define SIFAR_FACTORY_TABLE_HPP

#include <cstddef> // size_t

#include <unordered_map> // unordered_map

#include <Sifar/Utility.hpp>

#define _FACTORY_TABLE_UPDATER_DECLARATION(...)                                                         \
    static ::sifar::dynamic::FactoryTabelUpdater updater;

#define _FACTORY_TABLE_UPDATER_IMPLEMENTATION_WITH(prefix, ...)                                         \
    prefix ::sifar::dynamic::FactoryTabelUpdater __VA_ARGS__::updater                                   \
    ( new __VA_ARGS__, __VA_ARGS__::static_key() );

#define _FACTORY_TABLE_UPDATER_IMPLEMENTATION(...)                                                      \
    _FACTORY_TABLE_UPDATER_IMPLEMENTATION_WITH(, __VA_ARGS__)

#define _FACTORY_TABLE_UPDATER_TEMPLATE_IMPLEMENTATION(...)                                             \
     _FACTORY_TABLE_UPDATER_IMPLEMENTATION_WITH(template <>, __VA_ARGS__)

#define _CLONEABLE_DECLARATION_WITHOUT_KEY(...)                                                         \
    private:                                                                                            \
    virtual clone_type* clone() const override;                                                         \
    virtual clone_type* cast(void* address) const override;

#define _CLONEABLE_DECLARATION(...)                                                                     \
    _CLONEABLE_DECLARATION_WITHOUT_KEY()                                                                \
    static constexpr key_type static_key() noexcept;                                                    \
    virtual key_type dynamic_key() const noexcept override;                                             \
    public:

#define _CLONEABLE_TEMPLATE_DECLARATION(...)                                                            \
    _CLONEABLE_DECLARATION_WITHOUT_KEY()                                                                \
    static constexpr key_type static_key() noexcept                                                     \
    { return ::sifar::static_hash<key_type>(#__VA_ARGS__); }                                            \
    virtual key_type dynamic_key() const noexcept override                                              \
    { return static_key(); }                                                                            \
    public:

#define _CLONEABLE_IMPLEMENTATION_WITH(prefix, ...)                                                     \
    prefix auto __VA_ARGS__::clone() const -> clone_type*                                               \
    { return new __VA_ARGS__; }                                                                         \
    prefix auto __VA_ARGS__::cast(void* address) const -> clone_type*                                   \
    { return static_cast<__VA_ARGS__*>(address); }                                                      \
    prefix constexpr auto __VA_ARGS__::static_key() noexcept -> key_type                                \
    { return ::sifar::static_hash<key_type>(#__VA_ARGS__); }                                            \
    prefix auto __VA_ARGS__::dynamic_key() const noexcept -> key_type                                   \
    { return static_key(); }                                                                            \

#define _CLONEABLE_IMPLEMENTATION(...)                                                                  \
    _CLONEABLE_IMPLEMENTATION_WITH(, __VA_ARGS__)

#define _CLONEABLE_TEMPLATE_IMPLEMENTATION(...)                                                         \
    _CLONEABLE_IMPLEMENTATION_WITH(template <>, __VA_ARGS__)

namespace sifar
{

namespace dynamic
{

class Cloneable; // need for type alias

struct FactoryTableCore
{
    using key_type   = let::u64;
    using clone_type = Cloneable;
};

class FactoryTable; // ned for factory access

class Cloneable
{
    friend class FactoryTable;

public:
    using key_type   = FactoryTableCore::key_type;
    using clone_type = FactoryTableCore::clone_type;

public:
    virtual ~Cloneable() = default;

private:
    virtual key_type dynamic_key() const noexcept = 0;

    virtual clone_type* clone() const = 0;
    virtual clone_type* cast(void* address) const = 0;
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

    void update(clone_type* clone, key_type key)
    {
        if (not has_factory(key)) factory_[key] = clone;
    }

    clone_type* clone(key_type key)
    {
        auto has_clone = has_factory(key);
        return has_clone ? factory_[key]->clone() : nullptr;
    }

    clone_type* cast(void* address, key_type key)
    {
        auto has_clone = has_factory(key);
        return has_clone ? factory_[key]->cast(address) : nullptr;
    }

private:
    bool has_factory(key_type key)
    {
        return factory_.find(key) != factory_.end();
    }
};

struct FactoryTabelUpdater
{
    using key_type   = FactoryTableCore::key_type;
    using clone_type = FactoryTableCore::clone_type;

    FactoryTabelUpdater(clone_type* clone, key_type key)
    {
        FactoryTable::instance().update(clone, key);
    }
};

} // namespace dynamic

} // namespace sifar

#endif // SIFAR_FACTORY_TABLE_HPP
