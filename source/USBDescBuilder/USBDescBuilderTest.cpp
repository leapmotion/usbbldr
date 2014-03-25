#include "USBDescBuilderTest.h"
#include "usbdescbuilder.h"

TEST_F(USBDescBuilderTest, HelloWorldTest) {
  ASSERT_EQ(1, HelloWorld()) << "Hello world did not return 1!";
}