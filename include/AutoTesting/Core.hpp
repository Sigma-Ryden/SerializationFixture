#ifndef TESTING_CORE_HPP
#define TESTING_CORE_HPP

#include <cassert> // assert

#include <functional> // function

#include <map> // map
#include <string> // string

// Test definition

// Allow to hide owner classes within different translation units
#define TEST_SPACE(...)                                                                                 \
    namespace __VA_ARGS__

#define TEST(test_module, test_name)                                                                    \
    TEST_SPACE(test_module) {                                                                           \
        static struct test_name : TestingInterface {                                                    \
            test_name() : TestingInterface(#test_module, #test_name) {}                                 \
            void run() override;                                                                        \
        } s##test_name;                                                                                 \
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
    const std::string module;
    const std::string name;

public:
    TestingInterface(const std::string& module, const std::string& name);

public:
    virtual void run() = 0;
};

struct TestingCore
{
public:
    using TestRegistry      = std::map<std::string, TestingInterface*>;
    using ModuleRegistry    = std::map<std::string, TestRegistry>;
    using TextPrinter       = std::function<void(const std::string&)>;

private:
    TestingCore();

public:
    static TestingCore& instance();

    TestingCore(const TestingCore&) = delete;
    TestingCore& operator= (const TestingCore&) = delete;

public:
    void check(bool condition, TestingInterface* test, const std::string& msg);

public:
    void add(TestingInterface* test);

public:
    void execute_module(const std::string& name); // execute test module with specify name
    void execute_test(const std::string& name); // execute tests with specify name
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
