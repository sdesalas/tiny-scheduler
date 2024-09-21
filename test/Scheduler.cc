#include "scheduler.h"
#include "Arduino.h"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace scheduler;

unsigned long timer = 0;

unsigned long getTimer() {
  return timer;
}

TEST(Scheduler, Init) {
  timer = 0;
  Scheduler scheduler(getTimer, delay);
}

TEST(Scheduler, AddNode) {
  timer = 0;
  Scheduler scheduler(getTimer, delay);
  scheduler.addNode(new Scheduler::Node());
  ASSERT_EQ(scheduler.count(), 1);
}

TEST(Scheduler, AddMultipleNode) {
  timer = 0;
  Scheduler scheduler(getTimer, delay);
  scheduler.addNode(new Scheduler::Node());
  scheduler.addNode(new Scheduler::Node());
  scheduler.addNode(new Scheduler::Node());
  scheduler.addNode(new Scheduler::Node());
  ASSERT_EQ(scheduler.count(), 4);
}


TEST(Scheduler, RemoveWithTick) {
  timer = 0;
  Scheduler scheduler(getTimer, delay);
  scheduler.addNode(new Scheduler::Node(1));
  scheduler.addNode(new Scheduler::Node(2));
  scheduler.addNode(new Scheduler::Node(3));
  scheduler.addNode(new Scheduler::Node(4));
  timer = 0;
  scheduler.tick();
  ASSERT_EQ(scheduler.count(), 4);
  timer = 1;
  scheduler.tick();
  ASSERT_EQ(scheduler.count(), 3);
  timer = 2;
  scheduler.tick();
  ASSERT_EQ(scheduler.count(), 2);
  timer = 3;
  scheduler.tick();
  ASSERT_EQ(scheduler.count(), 1);
  timer = 4;
  scheduler.tick();
  ASSERT_EQ(scheduler.count(), 0);
}

TEST(Scheduler, RemoveAllWithTick) {
  timer = 0;
  Scheduler scheduler(getTimer, delay);
  scheduler.addNode(new Scheduler::Node(1));
  scheduler.addNode(new Scheduler::Node(2));
  scheduler.addNode(new Scheduler::Node(3));
  scheduler.addNode(new Scheduler::Node(4));
  timer = 0;
  ASSERT_EQ(scheduler.count(), 4);
  timer = 4;
  scheduler.tick();
  ASSERT_EQ(scheduler.count(), 0);
}


TEST(Scheduler, RemoveAllWithLoop) {
  timer = 0;
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
  timer = 0;
  bool done;
  bool* doneAddress = &done;
  Scheduler scheduler(getTimer, delay);
  scheduler.timeout(5, [doneAddress](){
    *doneAddress = true;
  });
  timer = 4;
  scheduler.tick();
  ASSERT_FALSE(done);
  timer = 5;
  scheduler.tick();
  ASSERT_TRUE(done);
}



TEST(Scheduler, Every) {
  timer = 0;
  int counter = 0;
  int* counterAddress = &counter;
  Scheduler scheduler(getTimer, delay);
  scheduler.every(2, [counterAddress](){
    *counterAddress += 1;
  });
  ASSERT_EQ(scheduler.count(), 1);
  for(timer=0;timer<10;timer++) {
    scheduler.tick();
    ASSERT_EQ(counter, timer / 2);
  }
  ASSERT_EQ(scheduler.count(), 1);
}


TEST(Scheduler, EveryWithFirstInterval) {
  timer = 0;
  int counter = -1;
  int* counterAddress = &counter;
  timer = 0;
  Scheduler scheduler(getTimer, delay);
  scheduler.every(0, 1, [counterAddress](){
    *counterAddress += 1;
  });
  ASSERT_EQ(scheduler.count(), 1);
  for(timer=0;timer<10;timer++) {
    scheduler.tick();
    ASSERT_EQ(counter, timer);
  }
  ASSERT_EQ(scheduler.count(), 1);
}


TEST(Scheduler, RepeatAndLoop) {
  timer = 0;
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
  timer = 0;
  int counter = 0;
  int* counterAddress = &counter;
  timer = 0;
  Scheduler scheduler(getTimer, delay);
  scheduler.repeat(10, 0, 1, [counterAddress](){
    *counterAddress += 1;
  });
  ASSERT_EQ(scheduler.count(), 1);
  for(timer=0;timer<10;timer++) {
    ASSERT_EQ(counter, timer);
    scheduler.tick();
  }
  ASSERT_EQ(scheduler.count(), 0);
  ASSERT_EQ(counter, 10);
}


TEST(Scheduler_Node, Init) {
  Scheduler::Node node;
}


int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}