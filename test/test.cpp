#include <gtest/gtest.h>
#include <camera.h>

TEST(blahTest, blah1) {
  Camera camera;
  EXPECT_EQ(1, 1);
}

int main (int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  int returnValue;

  //Do whatever setup here you will need for your tests here
  //
  //

  returnValue =  RUN_ALL_TESTS();

  //Do Your teardown here if required
  //
  //

  return returnValue;
}
