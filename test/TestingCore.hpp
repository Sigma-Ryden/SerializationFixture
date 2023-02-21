#ifndef TESTING_CORE_HPP
#define TESTING_CORE_HPP

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
    TestingCore::check(module, name, msg, false, __VA_ARGS__)

#define ASSERT(msg, ...)                                                                                \
    TestingCore::check(module, name, msg, true, __VA_ARGS__)

// Tests Execution
#define EXECUTE_TEST(name, ...)                                                                         \
    TestingCore::instance().execute_test(name);

#define EXECUTE_MODULE(name, ...)                                                                       \
    TestingCore::instance().execute_module(name);

#define EXECUTE_ALL(...)                                                                                \
    TestingCore::instance().execute_all();

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
    void check(TestingInterface* test, const char* msg, bool strict, bool condition);

public:
    void add(TestingInterface* test);

public:
    void execute_module(const char* name);
    void execute_test(const char* name);
    void execute_all();

public:
    unsigned passed;
    unsigned failed;

protected:
    ModuleRegistry registry_;

protected:
    static std::ostream& stream_;
};

#endif // TESTING_CORE_HPP
