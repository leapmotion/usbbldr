set(USBDescBuilderTest_SRCS
  USBDescBuilderTest.h
  USBDescBuilderTest.cpp
)

add_executable(USBDescBuilderTest ${USBDescBuilderTest_SRCS} "../gtest-all-guard.cpp")
target_link_libraries(USBDescBuilderTest USBDescBuilder)
