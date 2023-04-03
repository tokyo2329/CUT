#include <gtest/gtest.h>
#include <assert.h>
#include <pthread.h>

extern "C" {
  #include "buffer.h"
}


TEST(BufferTest, Init) {
  buffer b;
  init_buffer(&b);

  // try to lock and unlock the initialized mutex
  pthread_mutex_lock(&b.mtx);
  pthread_mutex_unlock(&b.mtx);

  destroy_buffer(&b);
}

TEST(BufferTest, WriteRead) {
  buffer b;
  init_buffer(&b);

  char * test_string = "Hello world!";
  write_to_buffer(&b, test_string);
  
  char * result = read_from_buffer(&b);
  
  EXPECT_STREQ(result, test_string);
  
  free(result);
  destroy_buffer(&b);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
