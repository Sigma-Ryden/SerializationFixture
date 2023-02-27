#include "SirafTestingBase.hpp"

#include <Siraf/Support/forward_list.hpp>

TEST(TestSTL, TestForwardList)
{
    enum class State { Free, Blocked, Forced };

    static std::forward_list<State> s_fl = { State::Forced, State::Blocked, State::Forced, State::Free };

    std::vector<unsigned char> storage;
    {
        std::forward_list<State> fl = s_fl;

        auto ar = oarchive(storage);
        ar & fl;
    }
    {
        std::forward_list<State> fl;

        auto ar = iarchive(storage);
        ar & fl;

        EXPECT("std::forward_list<>", fl == s_fl);
    }
}
