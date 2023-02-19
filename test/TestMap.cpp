#include "TestMap.h"

#include <Siraf/Support/map.hpp>

template <typename Tree>
static bool is_tree_equal(const Tree& lhs, const Tree& rhs)
{
    if (lhs.size() != rhs.size()) return false;

    for(auto& pair:rhs) if(lhs.count(pair.first) == 0) return false;
    return true;
}

DEFINE_AUTO_TEST(TestMap)
{
    enum class Hint { Alpha, Relative, Fly };

    static std::map<int, float> s_m = { {0, 0.5f}, {3, 0.1f}, {4, 0.17f} };
    static std::unordered_map<int, char> s_um = { {1, 'c'}, {2, '+'}, {3, '+'} };
    static std::multimap<Hint, int> s_mm = { {Hint::Alpha, 1}, {Hint::Fly, -1}, {Hint::Fly, 0} };
    static std::unordered_multimap<int, int> s_umm = { {-1, 0}, {-1, 2}, {1, -1}, {1, 0}, {1, 2} };

    std::vector<unsigned char> storage;
    {
        std::map<int, float> m = s_m;
        std::unordered_map<int, char> um = s_um;
        std::multimap<Hint, int> mm = s_mm;
        std::unordered_multimap<int, int> umm = s_umm;

        auto ar = oarchive<OByteStream>(storage);
        ar & m & um & mm & umm;
    }
    {
        std::map<int, float> m;
        std::unordered_map<int, char> um;
        std::multimap<Hint, int> mm;
        std::unordered_multimap<int, int> umm;

        auto ar = iarchive<IByteStream>(storage);
        ar & m & um & mm & umm;

        EXPECT("std::map<>", is_tree_equal(m, s_m));

        EXPECT("std::unordered_map<>", um.size() == s_um.size());
        EXPECT("std::unordered_map<>", um == s_um);

        EXPECT("std::multimap<>", is_tree_equal(mm, s_mm));

        EXPECT("std::unordered_multimap<>", umm.size() == s_umm.size());
        EXPECT("std::unordered_multimap<>", umm == s_umm);
    }
}
