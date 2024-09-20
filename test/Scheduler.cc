#include "scheduler.h"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace scheduler;

int counter = 1;

unsigned long zero() {
  return 0;
}

void add_and_print() {
  std::cout << "add_and_print" << std::endl;
  std::cout << ++counter << std::endl;
}

TEST(Scheduler, Init) {
  Scheduler scheduler(zero);
}

TEST(Scheduler, AddNode) {
  Scheduler scheduler(zero);
  scheduler.addNode(new Scheduler::Node());
  ASSERT_EQ(scheduler.count(), 1);
}

TEST(Scheduler, AddMultipleNode) {
  Scheduler scheduler(zero);
  scheduler.addNode(new Scheduler::Node());
  scheduler.addNode(new Scheduler::Node());
  scheduler.addNode(new Scheduler::Node());
  scheduler.addNode(new Scheduler::Node());
  ASSERT_EQ(scheduler.count(), 4);
}


TEST(Scheduler, RemoveWithTick) {
  Scheduler scheduler(zero);
  scheduler.addNode(new Scheduler::Node(1));
  scheduler.addNode(new Scheduler::Node(2));
  scheduler.addNode(new Scheduler::Node(3));
  scheduler.addNode(new Scheduler::Node(4));
  ASSERT_EQ(scheduler.count(), 4);
  scheduler.tick(1);
  ASSERT_EQ(scheduler.count(), 3);
  scheduler.tick(2);
  ASSERT_EQ(scheduler.count(), 2);
  scheduler.tick(3);
  ASSERT_EQ(scheduler.count(), 1);
  scheduler.tick(4);
  ASSERT_EQ(scheduler.count(), 0);
}

TEST(Scheduler, RemoveAllWithTick) {
  Scheduler scheduler(zero);
  scheduler.addNode(new Scheduler::Node(1));
  scheduler.addNode(new Scheduler::Node(2));
  scheduler.addNode(new Scheduler::Node(3));
  scheduler.addNode(new Scheduler::Node(4));
  ASSERT_EQ(scheduler.count(), 4);
  scheduler.tick(4);
  ASSERT_EQ(scheduler.count(), 0);
}


TEST(Scheduler, RemoveAllWithLoop) {
  Scheduler scheduler = Scheduler::micros();
  scheduler.addNode(new Scheduler::Node(1));
  scheduler.addNode(new Scheduler::Node(2));
  scheduler.addNode(new Scheduler::Node(3));
  scheduler.addNode(new Scheduler::Node(4));
  ASSERT_EQ(scheduler.count(), 4);
  scheduler.loop();
  ASSERT_EQ(scheduler.count(), 0);
}


TEST(Scheduler, Timeout) {
  bool done;
  bool* doneAddress = &done;
  Scheduler scheduler(zero);
  scheduler.timeout(5, [doneAddress](){
    *doneAddress = true;
  });
  scheduler.tick(4);
  ASSERT_FALSE(done);
  scheduler.tick(5);
  ASSERT_TRUE(done);
}


unsigned long timer = 0;

unsigned long getTimer() {
  return timer;
}

TEST(Scheduler, Every) {
  int counter = 0;
  int* counterAddress = &counter;
  Scheduler scheduler(getTimer);
  scheduler.every(2, [counterAddress](){
    *counterAddress += 1;
  });
  ASSERT_EQ(scheduler.count(), 1);
  for(timer=0;timer<10;timer++) {
    scheduler.tick(timer);
    ASSERT_EQ(counter, timer / 2);
  }
  ASSERT_EQ(scheduler.count(), 1);
}


TEST(Scheduler, EveryWithFirstInterval) {
  int counter = -1;
  int* counterAddress = &counter;
  timer = 0;
  Scheduler scheduler(getTimer);
  scheduler.every(0, 1, [counterAddress](){
    *counterAddress += 1;
  });
  ASSERT_EQ(scheduler.count(), 1);
  for(timer=0;timer<10;timer++) {
    scheduler.tick(timer);
    ASSERT_EQ(counter, timer);
  }
  ASSERT_EQ(scheduler.count(), 1);
}


TEST(Scheduler, RepeatAndLoop) {
  int counter = 0;
  int* counterAddress = &counter;
  Scheduler scheduler = Scheduler::micros();
  scheduler.repeat(10, 1, [counterAddress](){
    *counterAddress += 1;
  });
  ASSERT_EQ(scheduler.count(), 1);
  scheduler.loop();
  ASSERT_EQ(scheduler.count(), 0);
  ASSERT_EQ(counter, 10);
}


TEST(Scheduler, RepeatWithFirstInterval) {
  int counter = 0;
  int* counterAddress = &counter;
  timer = 0;
  Scheduler scheduler(getTimer);
  scheduler.repeat(10, 0, 1, [counterAddress](){
    *counterAddress += 1;
  });
  ASSERT_EQ(scheduler.count(), 1);
  for(timer=0;timer<10;timer++) {
    ASSERT_EQ(counter, timer);
    scheduler.tick(timer);
  }
  ASSERT_EQ(scheduler.count(), 0);
  ASSERT_EQ(counter, 10);
}


TEST(Scheduler_Node, Init) {
  Scheduler::Node node;
}

/*
TEST(Scheduler, CancelAll) {
  counter = 0;
  auto& scheduler = Scheduler::main();
  scheduler.timeout(1, [&scheduler]() { scheduler.cancelAll(); });
  scheduler.timeout(1, add_and_print);
  scheduler.timeout(2, add_and_print);
  scheduler.timeout(3, add_and_print);
  EXPECT_EQ(counter, 0);
  scheduler.loop();
  EXPECT_EQ(counter, 0);
}

TEST(Schedule, Every) {
  Scheduler::main().every(1, []() { Scheduler::main().debug(Serial); });
  for (int i = 0; i < 10; i += 1) {
    delay(Scheduler::main().tick());
  }
}
*/

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}