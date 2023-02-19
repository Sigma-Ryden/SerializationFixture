#include "TestModule.h"

#include "TestNumber.h"
#include "TestEnum.h"
#include "TestUserType.h"

#include "TestVector.h"
#include "TestArray.h"

#include "TestString.h"

#include "TestDeque.h"

#include "TestQueue.h"
#include "TestStack.h"

#include "TestBitset.h"

#include "TestList.h"
#include "TestForwardList.h"

#include "TestMap.h"
#include "TestSet.h"

#include "TestPair.h"
#include "TestTuple.h"

#include "TestMemory.h"

int main()
{
    RUN_AUTO_TEST(TestNumber)
    RUN_AUTO_TEST(TestEnum)
    RUN_AUTO_TEST(TestUserType)

    RUN_AUTO_TEST(TestVector)
    RUN_AUTO_TEST(TestArray)

    RUN_AUTO_TEST(TestString)

    RUN_AUTO_TEST(TestDeque)

    RUN_AUTO_TEST(TestQueue)
    RUN_AUTO_TEST(TestStack)

    RUN_AUTO_TEST(TestBitset)

    RUN_AUTO_TEST(TestList)
    RUN_AUTO_TEST(TestForwardList)

    RUN_AUTO_TEST(TestMap)
    RUN_AUTO_TEST(TestSet)

    RUN_AUTO_TEST(TestPair)
    RUN_AUTO_TEST(TestTuple)

    RUN_AUTO_TEST(TestUniquePtr)
    RUN_AUTO_TEST(TestSharedPtr)
    RUN_AUTO_TEST(TestWeakPtr)

    TestModule::Stat();
    return 0;
}
