#include "SirafTestingBase.hpp"

#include <Siraf/Span.hpp>

TEST(TestCommon, TestDynamicArray)
{
    // allow to serialize multi dimension raw dynamic array
    using siraf::span;

    static const unsigned sv_h = 3;
    static const unsigned sv_w = 5;

    static int sv_arr[sv_h][sv_w] = {
        {7, 8, 1, 4, 7},
        {4, 2, 6, 0, 1},
        {7, 8, 9, 1, 8},
    };

    std::vector<unsigned char> storage;
    {
        unsigned h = sv_h;
        unsigned w = sv_w;
        int** arr = nullptr;

        arr = new int* [h];
        for (unsigned i = 0; i < h; ++i)
            arr[i] = new int [w];

        for (unsigned i = 0; i < h; ++i)
            for (unsigned j = 0; j < w; ++j)
                arr[i][j] = sv_arr[i][j];

        auto ar = oarchive(storage);
        ar & span(arr, h, w);
    }
    {
        unsigned h = 0;
        unsigned w = 0;
        int** arr = nullptr;

        auto ar = iarchive(storage);
        ar & span(arr, h, w);

        ASSERT("dynamic array.inited", arr != nullptr);
        EXPECT("dynamic array.size", h == sv_h && w == sv_w);

        // lazy check
        auto is_inited = true;
        for (unsigned i = 0; i < h; ++i)
            is_inited &= (arr[i] != nullptr);

        ASSERT("dynamic array.inited inner", is_inited);

        // lazy check
        auto is_equal = true;
        for (unsigned i = 0; i < h; ++i)
            for (unsigned j = 0; j < w; ++j)
                is_equal &= (arr[i][j] == sv_arr[i][j]);

        EXPECT("dynamic array.value", is_equal);
    }
}
