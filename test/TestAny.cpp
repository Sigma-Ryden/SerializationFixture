#if __cplusplus >= 201703L

#include "SirafTestingBase.hpp"

#include <Siraf/Support/any.hpp>

#include <Siraf/Support/string.hpp>
#include <Siraf/Support/vector.hpp>

TEST(TestUtility, TestAny)
{
    using siraf::serializable; // do possible serialize data assigned to any

    enum class Property { Speed, Force, Intelige };

    static std::string sv_va_0 = "Tom";
    static int sv_va_1 = 20;
    static Property sv_va_2 = Property::Intelige;

    std::vector<unsigned char> storage;
    {
        std::vector<std::any> va = {
            "Jack", // serializable can be omitted, see type registry (0)
            serializable(sv_va_1),
            serializable(Property::Force) // (1)
        };

        va[0] = serializable(sv_va_0); // (0)
        va[2] = sv_va_2; // serializable can be omitted, see previous type registry (1)

        auto ar = oarchive(storage);
        ar & va;
    }
    {
        std::vector<std::any> va;

        auto ar = iarchive(storage);
        ar & va;

        ASSERT("std::any.inited", va[0].has_value() && va[1].has_value() && va[1].has_value());

        ASSERT("std::any.any_cast",
            std::any_cast<std::string>(&va[0]) != nullptr &&
            std::any_cast<int>(&va[1]) != nullptr &&
            std::any_cast<Property>(&va[2]) != nullptr);

        EXPECT("std::any.value",
            std::any_cast<std::string&>(va[0]) == sv_va_0 &&
            std::any_cast<int>(va[1]) == sv_va_1 &&
            std::any_cast<Property>(va[2]) == sv_va_2);
    }
}

#endif // if
