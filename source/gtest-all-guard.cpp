// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#if __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
#include "googletest/src/gtest-all.cc"
#include <iostream>
namespace testing {
namespace internal {
  extern bool g_help_flag;
}
}

using namespace testing::internal;
using namespace std;

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
