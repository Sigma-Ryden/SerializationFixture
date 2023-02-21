#include "SirafTestingBase.hpp"

#include <Siraf/Support/forward_list.hpp>

TEST(TestSTL, TestForwardList)
{
    enum class State { Free, Blocked, Forced };

    static std::forward_list<State> s_fl = { State::Forced, State::Blocked, State::Forced, State::Free };
    static auto s_fl_size = std::distance(s_fl.begin(), s_fl.end());

    std::vector<unsigned char> storage;
    {
        std::forward_list<State> fl = s_fl;

        auto ar = oarchive<OByteStream>(storage);
        ar & fl;
    }
    {
        std::forward_list<State> fl;

        auto ar = iarchive<IByteStream>(storage);
        ar & fl;

        EXPECT("std::forward_list<>", std::distance(fl.begin(), fl.end()) == s_fl_size);
        EXPECT("std::forward_list<>", fl == s_fl);
    }
}
