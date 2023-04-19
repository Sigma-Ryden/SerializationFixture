#ifndef TESTING_CORE_HPP
#define TESTING_CORE_HPP

#include <cassert> // assert

#include <functional> // function

#include <map> // map
#include <string> // string

// Test definition

// Allow to hide owner classes within different translation units
#define TEST_MODULE(test_module)                                                                        \
    namespace test_module

#define TEST(test_module, test_name)                                                                    \
    TEST_MODULE(test_module) {                                                                          \
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

// Show stat info
#define TESTING_STAT(...)                                                                               \
    TestingCore::instance().stat()

// Set own text printer
#define PRINTER(...)                                                                                    \
    TestingCore::instance().text_printer = __VA_ARGS__

// Will catch exception and show by text printer
#define TRY_CATCH(...)                                                                                  \
    TestingCore::instance().try_catch([]{ (void)(__VA_ARGS__); })

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

    using TextPrinter       = std::function<void(const std::string&)>;

private:
    TestingCore();

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
    void try_catch(std::function<void()>&& call) const noexcept;

public:
    TextPrinter console_printer() const noexcept;

public:
    unsigned passed;
    unsigned failed;

public:
    TextPrinter text_printer;

protected:
    ModuleRegistry registry_;
};

#endif // TESTING_CORE_HPP
