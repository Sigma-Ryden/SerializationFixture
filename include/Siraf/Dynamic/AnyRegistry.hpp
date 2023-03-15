#ifndef SIRAF_DYNAMIC_ANY_REGISTRY_HPP
#define SIRAF_DYNAMIC_ANY_REGISTRY_HPP

#if __cplusplus >= 201703L

#include <unordered_map> // unordered_map
#include <any> // any

#include <typeinfo> // typeid, type_info

#include <Siraf/Core/PolymorphicArchive.hpp>
#include <Siraf/Core/TypeCore.hpp>
#include <Siraf/Core/TypeRegistry.hpp>

#include <Siraf/Detail/Meta.hpp>

namespace siraf
{

namespace dynamic
{

class AnyRegistry
{
private:
    struct AnySerialization
    {
        // we use raw function ptr instead std::function to reach perfomance
        using Function = void(*)(core::ArchiveBase&, std::any&);

        Function save = nullptr;
        Function load = nullptr;
    };

private:
    using InnerTable = std::unordered_map<let::u64, AnySerialization>;

private:
    InnerTable registry_;

private:
    AnyRegistry() : registry_() {}

    AnyRegistry(const AnyRegistry&) = delete;
    AnyRegistry& operator=(const AnyRegistry&) = delete;

public:
    static AnyRegistry& instance() noexcept
    {
        static AnyRegistry self;
        return self;
    }

    template <typename T> void update()
    {
        auto save = [](core::ArchiveBase& archive, std::any& any)
        {
            archive << std::any_cast<T&>(any);
        };

        auto load = [](core::ArchiveBase& archive, std::any& any)
        {
            any.emplace<T>();
            archive >> std::any_cast<T&>(any);
        };

        auto hash = SIRAF_TYPE_HASH(typeid(T));
        registry_[hash] = {save, load};
    }

public:
    void save(core::ArchiveBase& archive, std::any& any)
    {
        auto hash = SIRAF_TYPE_HASH(any.type());
        return serialization(hash).save(archive, any);
    }

    void load(core::ArchiveBase& archive, std::any& any, let::u64 hash)
    {
        serialization(hash).load(archive, any);
    }

private:
    const AnySerialization& serialization(let::u64 hash)
    {
        auto it = registry_.find(hash);
        if (it == registry_.end())
            throw "The 'siraf::AnyRegistry' must registry type with specify hash code.";

        return it->second;
    }
};

} // namespace dynamic

// Type registry for any serialization, allowed registered and supported types only
template <typename T> void spy()
{
    static_assert(not meta::is_unsupported<T>(), "The 'T' is an unsupported type for serialization.");
    static_assert(meta::is_registered<T>(), "The 'T' is an unregistered type for serialization.");

    dynamic::AnyRegistry::instance().update<T>();
}

template <typename T> T&& spy(T&& object)
{
    spy<meta::decay<T>>();
    return std::forward<T>(object);
}

} // namespace siraf

#endif // if

#endif // SIRAF_DYNAMIC_ANY_REGISTRY_HPP

