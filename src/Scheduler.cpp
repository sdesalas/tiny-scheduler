#include "Scheduler.h"

namespace scheduler {


Scheduler::Node::Node(): when(0) {
  this->next = NULL;
  this->prev = NULL;
} 



Scheduler::Node::Node(unsigned long when): when(when) {
  this->next = NULL;
  this->prev = NULL;
} 

bool Scheduler::Node::isAfter(const Scheduler::Node& other) const {
  return this->when > other.when;
} 

bool Scheduler::Node::isBefore(const Scheduler::Node& other) const {
  return this->when < other.when;
} 

bool Scheduler::Node::isAfter(unsigned long delta) const {
  return this->when > delta;
} 

bool Scheduler::Node::isBefore(unsigned long delta) const {
  return this->when < delta;
} 


unsigned long Scheduler::Node::leftTime(unsigned long delta) const {
  return this->when - delta;
} 


bool Scheduler::Node::hasNext() const {
  return this->next != NULL;
}


void Scheduler::Node::setNext(Scheduler::Node* next) {
  this->next = next;
  if(next != NULL) {
    next->prev = this;
  }
} 


void Scheduler::Node::setPrev(Scheduler::Node* prev) {
  this->prev = prev;
  if(prev != NULL) {
    prev->next = this;
  }
} 


void Scheduler::Node::remove() {
  auto next = this->next;
  auto prev = this->prev;
  if(next != NULL) {
    next->prev = prev;
  }
  if(prev != NULL) {
    prev->next = next;
  }
}

Scheduler::Node* Scheduler::addNode(Scheduler::Node* newNode) {
  Scheduler::Node* node = &this->head;
  while(node->hasNext() && newNode->isAfter(*node)) {
    node = node->next;
  }
  newNode->setNext(node->next);
  newNode->setPrev(node);
  return newNode;
}

Scheduler::Scheduler(TimeProvider timeProvider) : timeProvider(timeProvider) {

}

Scheduler::~Scheduler() {
  Scheduler::Node* node = this->head.next;
  while(node != NULL) {
    auto next = node->next;
    delete node;
    node = next;
  }
}

bool Scheduler::isEmpty() const {
  return !this->head.hasNext();
}

unsigned long Scheduler::tick() {
  if(this->isEmpty()) {
    return 0;
  }
  unsigned long delta = this->timeProvider();
  return this->tick(delta);
}


unsigned long Scheduler::tick(unsigned long delta) {
  Scheduler::Node* node = this->head.next;
  while(!node->isAfter(delta)) {
    auto next = node->next;
    node->run();
    auto done = node;
    done->remove();
    delete done;
    node = next;
    if(node == NULL)  {
      break;
    }
  }
  return 0;
}

unsigned int Scheduler::count() const {
  if(this->isEmpty()) {
    return 0;
  }
  unsigned int counter = 0;
  Scheduler::Node* node = this->head.next;
  while(node != NULL) {
    counter += 1;
    node = node->next;
  }
  return counter;
}


void Scheduler::loop() {
  while(!this->isEmpty()) {
    const unsigned long wait = this->tick();
    if(wait != 0) {
      delayMicroseconds(wait);
    }
  }
}

}
