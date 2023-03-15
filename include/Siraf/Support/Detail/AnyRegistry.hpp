#ifndef SIRAF_SUPPORT_DETAIL_ANY_REGISTRY_HPP
#define SIRAF_SUPPORT_DETAIL_ANY_REGISTRY_HPP

#if __cplusplus >= 201703L

#include <functional> // function
#include <unordered_map> // unordered_map

#include <any> // any

#include <typeinfo> // typeid, type_info

#include <Siraf/Core/PolymorphicArchive.hpp>
#include <Siraf/Core/TypeCore.hpp>

#include <Siraf/Detail/Meta.hpp>

namespace siraf
{

class AnyRegistry
{
private:
    struct AnySerialization
    {
        using Function = std::function<void(core::ArchiveBase&, std::any&)>;

        Function save = nullptr;
        Function load = nullptr;
    };

private:
    using InnerTable = std::unordered_map<std::size_t, AnySerialization>;

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

    void load(core::ArchiveBase& archive, std::any& any, std::size_t hash)
    {
        serialization(hash).load(archive, any);
    }

private:
    const AnySerialization& serialization(std::size_t hash)
    {
        auto it = registry_.find(hash);
        if (it == registry_.end())
            throw "The 'siraf::AnyRegistry' must registry type with specify hash code.";

        return it->second;
    }
};

// Type registry for any serialization
template <typename T> void serializable()
{
    AnyRegistry::instance().update<meta::decay<T>>();
}

template <typename T> T&& serializable(T&& object)
{
    serializable<T>();
    return std::forward<T>(object);
}

} // namespace siraf

#endif // if

#endif // SIRAF_SUPPORT_DETAIL_ANY_REGISTRY_HPP

