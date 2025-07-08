#include <SF/BuiltIn/any.hpp>
#include <SF/Core.hpp>

SERIALIZABLE(save, any, std::any)
    SERIALIZATION
    (
        std::uint64_t hash = any.type().hash_code();
        archive & hash;

        ::sf::dynamic::any_registry.save(archive, any, hash);
    )
SERIALIZABLE_INIT()

SERIALIZABLE(load, any, std::any)
    SERIALIZATION
    (
        std::uint64_t hash{};
        archive & hash;

        ::sf::dynamic::any_registry.load(archive, any, hash);
    )
SERIALIZABLE_INIT()
