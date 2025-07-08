#include <SF/BuiltIn/vector.hpp>
#include <SF/Core.hpp>

// slow impl
SERIALIZABLE(save, vector, std::vector<bool>)
    SERIALIZATION
    (
        std::uint64_t size = vector.size();
        archive & size;

        for (auto item : vector)
        {
            bool boolean = item;
            archive & boolean;
        }
    )
SERIALIZABLE_INIT()

SERIALIZABLE(load, vector, std::vector<bool>)
    SERIALIZATION
    (
        std::uint64_t size{};
        archive & size;

        vector.resize(size);

        for (auto item/*bit_reference*/ : vector)
        {
            bool boolean{};
            archive & boolean;
            item = boolean;
        }
    )
SERIALIZABLE_INIT()
