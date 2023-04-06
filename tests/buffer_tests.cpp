#include <gtest/gtest.h>
#include <assert.h>
#include <pthread.h>

extern "C" {
  #include "buffer.h"
  #include "cpu_data.h"
}

TEST(BufferTest, InitBufferTest) {
  buffer b;
  init_buffer(&b, sizeof(cpu_data));

  EXPECT_EQ(b.head, nullptr);
  EXPECT_EQ(b.tail, nullptr);

  destroy_buffer(&b);
}

TEST(BufferTest, DestroyBufferTest) {
  buffer b;
  init_buffer(&b, sizeof(cpu_data));

  cpu_data data[] = { (cpu_data){ .user = 5 } };
  enqueue(&b, (void *)data);

  destroy_buffer(&b);

  EXPECT_EQ(b.head, nullptr);
  EXPECT_EQ(b.tail, nullptr);
}

TEST(BufferTest, EnqueueTest) {
  buffer b;
  init_buffer(&b, sizeof(cpu_data));

  cpu_data data[] = { (cpu_data){ .user = 5 } };
  enqueue(&b, (void *)data);

  EXPECT_EQ(b.head, b.tail);
  EXPECT_EQ(b.head->next, nullptr);
  cpu_data * tmp = (cpu_data *)b.head->values;
  EXPECT_EQ(tmp[0].user, data[0].user);

  destroy_buffer(&b);
}

TEST(BufferTest, DequeueTest) {
  buffer b;
  init_buffer(&b, sizeof(cpu_data));

  cpu_data data[] = { (cpu_data){ .user = 5 } };
  enqueue(&b, (void *)data);

  cpu_data * result = (cpu_data *)dequeue(&b);

  EXPECT_EQ(result[0].user, data[0].user);
  EXPECT_EQ(b.head, nullptr);
  EXPECT_EQ(b.tail, nullptr);

  free(result);
  destroy_buffer(&b);
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
