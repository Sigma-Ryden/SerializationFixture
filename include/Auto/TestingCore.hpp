#ifndef TESTING_CORE_HPP
#define TESTING_CORE_HPP

#include <cassert> // assert

#include <map> // map
#include <string> // string
#include <iosfwd> // ostream

// Test definition
#define TEST(test_module, test_name)                                                                    \
    namespace test_module {                                                                             \
        struct test_name : TestingInterface {                                                           \
            test_name() : TestingInterface(#test_module, #test_name) {}                                 \
            void run() override;                                                                        \
        };                                                                                              \
        static TestingInterface* s##test_module##test_name = new test_name;                             \
    }                                                                                                   \
    void test_module::test_name::run()

// Calculate completed and failed test, provide debug info
#define EXPECT(msg, ...)                                                                                \
    TestingCore::instance().check(__VA_ARGS__, this, msg)

#define ASSERT(msg, ...)                                                                                \
    assert(msg && (__VA_ARGS__));                                                                       \
    TestingCore::instance().check(__VA_ARGS__, this, msg)

// Tests Execution
#define EXECUTE_TEST(name, ...)                                                                         \
    TestingCore::instance().execute_test(name)

#define EXECUTE_MODULE(name, ...)                                                                       \
    TestingCore::instance().execute_module(name)

#define EXECUTE_ALL(...)                                                                                \
    TestingCore::instance().execute_all()

// Stat info
#define TESTING_STAT(...)                                                                               \
    TestingCore::instance().stat()

struct TestingInterface
{
public:
    const char* module = nullptr;
    const char* name = nullptr;

public:
    TestingInterface(const char* module, const char* name);

public:
    virtual void run() = 0;
};

struct TestingCore
{
public:
    using ModuleName        = std::string;
    using TestName          = std::string;

    using TestRegistry      = std::map<TestName, TestingInterface*>;
    using ModuleRegistry    = std::map<ModuleName, TestRegistry>;

private:
    TestingCore() = default;

public:
    static TestingCore& instance();

    TestingCore(const TestingCore&) = delete;
    TestingCore& operator= (const TestingCore&) = delete;

public:
    void check(bool condition, TestingInterface* test, const char* msg);

public:
    void add(TestingInterface* test);

public:
    void execute_module(const char* name); // execute test module with specify name
    void execute_test(const char* name); // execute tests with specify name
    void execute_all();

public:
    void stat();

public:
    unsigned passed;
    unsigned failed;

protected:
    static std::ostream& stream_;

protected:
    ModuleRegistry registry_;
};

#endif // TESTING_CORE_HPP
