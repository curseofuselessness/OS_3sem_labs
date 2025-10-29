#include "Lab_2.h"
#include <gtest/gtest.h>
#include <thread>

TEST(MinMaxThreadTest, ProducesCorrectGlobals) {
    arr = { 5, 2, 8, 1, 9 };

    std::thread t(MinMaxThread);
    t.join();

    EXPECT_EQ(minVal, 1);
    EXPECT_EQ(maxVal, 9);
    EXPECT_EQ(minIndex, 3);
    EXPECT_EQ(maxIndex, 4);
}