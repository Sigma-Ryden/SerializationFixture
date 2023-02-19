#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include <iosfwd> // ostream
#include <memory> // unique_ptr

#include <Siraf/Core.hpp>

// Helpful macros:
// Auto test declaration
#define DECLARE_AUTO_TEST(test, ...)                                                                    \
    struct test { test(); const char* name = #test; };

// Define your own impl or Run test function
#define DEFINE_AUTO_TEST(test, ...)                                                                     \
    test::test()

#define RUN_AUTO_TEST(test, ...)                                                                        \
    std::unique_ptr<test> s##test(new test);

// Calculate completed and failed test, provide debug info
#define EXPECT(msg, ...)                                                                                \
    TestModule::Expect(name, msg, __VA_ARGS__)

using siraf::oarchive;
using siraf::wrapper::OByteStream;

using siraf::iarchive;
using siraf::wrapper::IByteStream;

using siraf::hierarchy;

struct TestModule
{
    static std::ostream& stream;

    static unsigned passed;
    static unsigned failed;

    static void Expect(const char* name, const char* text, bool condition);
    static void Stat();
};

#endif // TEST_MODULE_H
