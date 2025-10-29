#include "Lab_2.h"
#include <gtest/gtest.h>

TEST(MinMaxThreadTest, ProducesCorrectGlobals)
{
    arr = { 5, 2, 8, 1, 9 };
    HANDLE hThread = CreateThread(NULL, 0, MinMaxThread, NULL, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    EXPECT_EQ(minVal, 1);
    EXPECT_EQ(maxVal, 9);
    EXPECT_EQ(minIndex, 3);
    EXPECT_EQ(maxIndex, 4);
}