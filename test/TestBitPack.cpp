#include "SFTestingBase.hpp"

#include <SF/Utility/BitPack.hpp>

TEST_SPACE()
{

struct BitMaskObject
{
    std::uint8_t c0 : 3;
    std::uint8_t c1 : 1;
    std::uint8_t : 2; // free space
    std::uint8_t c2 : 4;
};

} // namespace TEST_SPACE

TEST(Common, TestBitPack)
{
    static unsigned int s_c0 = 7;
    static unsigned int s_c1 = 1;
    static unsigned int s_c2 = 14;

    std::vector<unsigned char> storage;
    {
        auto ar = sf::oarchive(storage);

        BitMaskObject object;

        object.c0 = s_c0;
        object.c1 = s_c1;
        object.c2 = s_c2;

        BITPACK(std::uint8_t)(ar, (object.c0, 3), (object.c1, 1), (object.c2, 4));
    }
    {
        auto ar = sf::iarchive(storage);

        BitMaskObject object;

        // BITPACK above will generate following equivalent code:
        {
            auto bitpack = sf::bitpack<std::uint8_t>(ar);
            object.c0 = bitpack(object.c0, 3);
            object.c1 = bitpack(object.c1, 1);
            object.c2 = bitpack(object.c2, 4);
        }

        EXPECT("inited", storage.size() == 1); // since we serialize bitpack with std::uint8_t
        EXPECT("value", object.c0 == s_c0 && object.c1 == s_c1 && object.c2 == s_c2);
    }
}
