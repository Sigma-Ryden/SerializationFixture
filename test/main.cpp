#include <Auto/TestingCore.hpp>

#include <SF/Core.hpp>

int main()
{
    TRY_CATCH(EXECUTE_ALL());
    TESTING_STAT();

    return 0;
}
