#if __cplusplus >= 201703L

#include "SFTestingBase.hpp"

#include <SF/Support/optional.hpp>
#include <SF/Support/atomic.hpp>

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

        EXPECT("std::optional<>.nullopt", ns == std::nullopt);

        ASSERT("std::optional<>.has_value", f.has_value() && s.has_value());
        EXPECT("std::optional<>.value", f.value() == sv_f && s.value() == sv_s);

        ASSERT("std::optional<atomic>.has_value", c.has_value());
        EXPECT("std::optional<atomic>.value", c.value() == sv_c);
    }
}

#endif // if
