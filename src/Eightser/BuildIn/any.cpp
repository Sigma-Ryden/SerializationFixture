#include <Eightser/BuiltIn/any.hpp>
#include <Eightser/Core.hpp>

SERIALIZABLE(save, any, std::any)
    SERIALIZATION
    (
        std::uint64_t hash = any.type().hash_code();
        archive & hash;

        ::eightser::dynamic::any_registry.save(archive, any, hash);
    )
SERIALIZABLE_INIT()

SERIALIZABLE(load, any, std::any)
    SERIALIZATION
    (
        std::uint64_t hash{};
        archive & hash;

        ::eightser::dynamic::any_registry.load(archive, any, hash);
    )
SERIALIZABLE_INIT()
