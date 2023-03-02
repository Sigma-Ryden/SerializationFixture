#if __cplusplus >= 201703L

#include "SirafTestingBase.hpp"

#include <Siraf/Support/optional.hpp>
#include <Siraf/Support/atomic.hpp>

TEST(TestUtility, TestOptional)
{
    enum class State { Ident, Spotted };

    static float sv_f = 2.71f;
    static State sv_s = State::Spotted;
    static char sv_c = 'f';

    std::vector<unsigned char> storage;
    {
        std::optional<float> f = sv_f;
        std::optional<State> ns;
        std::optional<State> s = sv_s;
        std::optional<std::atomic<char>> c(sv_c);

        auto ar = oarchive(storage);
        ar & f & ns & s & c;
    }
    {
        std::optional<float> f;
        std::optional<State> ns;
        std::optional<State> s;
        std::optional<std::atomic<char>> c;

        auto ar = iarchive(storage);
        ar & f & ns & s & c;

        EXPECT("std::optional<> nullopt", !ns.has_value());

        ASSERT("std::optional<>", f.has_value() && s.has_value());
        EXPECT("std::optional<>", f.value() == sv_f && s.value() == sv_s);

        ASSERT("std::optional<atomic>", c.has_value());
        EXPECT("std::optional<atomic>", c.value() == sv_c);
    }
}

#endif // if
