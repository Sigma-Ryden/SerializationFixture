#include <iostream>

#include "TestModule.h"

std::ostream& TestModule::stream = std::cout;

unsigned TestModule::passed = 0;
unsigned TestModule::failed = 0;

void TestModule::Expect(const char* name, const char* text, bool condition)
{
    passed += condition;
    failed += !condition;
    stream << name << ". " << text << " - " << (condition?"Ok":"Fail") << '\n';
}

void TestModule::Stat()
{
    stream << "\nPassed: " << passed << "\nFailed: " << failed << '\n';
}
