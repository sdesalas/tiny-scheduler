#include "Scheduler.h"

namespace scheduler {


void usDelay(unsigned long us) {
  delayMicroseconds(us);
}


Scheduler::Node::Node(): when(0) {
  this->next = NULL;
} 

Scheduler::Node::Node(unsigned long when): when(when) {
  this->next = NULL;
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
} 


Scheduler::Node* Scheduler::Node::withGroupId(unsigned long groupId) {
  this->groupId = groupId;
  return this;
} 


void Scheduler::Node::debug(Stream& stream) const {
  stream.print("Node {");
  stream.print(" .when=");
  stream.print(this->when);
  stream.print(" .groupId=");
  stream.print(this->groupId);
  stream.print(" }");
} 



Scheduler::Node* Scheduler::addNode(Scheduler::Node* newNode) {
  Scheduler::Node* node = &this->head;
  while(node->hasNext() && node->next->isBefore(*newNode)) {
    node = node->next;
  }
  newNode->setNext(node->next);
  node->setNext(newNode);
  return newNode;
}

Scheduler::Scheduler(TimeProvider timeProvider, Delay delay) : timeProvider(timeProvider), delay(delay) {

}

Scheduler::~Scheduler() {
  Scheduler::Node* node = this->head.next;
  while(node != NULL) {
    auto next = node->next;
    delete node;
    node = next;
  }
}

void Scheduler::debug(Stream& stream) const {
  stream.println("Scheduler::debug");
  stream.println("Scheduler Tasks:");
  Scheduler::Node* node = this->head.next;
  while (node != NULL) {
    stream.print("\t");
    node->debug(stream);
    stream.println();
    node = node->next;
  }
  stream.println("-----");
}

bool Scheduler::isEmpty() const {
  return !this->head.hasNext();
}


unsigned long Scheduler::tick() {
  while (this->head.hasNext()) {
    Node* node = this->head.next;
    unsigned long delta = this->timeProvider();
    if (node->isAfter(delta)) {
      unsigned long leftTime = node->leftTime(delta);
      return leftTime;
    }
    this->head.setNext(node->next);
    bool deleteNode = node->run();
    if (deleteNode) {
      delete node;
    }
    else {
      this->addNode(node);
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
      this->delay(wait);
    }
  }
}

void Scheduler::clear() {
  while (this->head.hasNext()) {
    Node* node = this->head.next;
    this->head.setNext(node->next);
    delete node;
  }
}


void Scheduler::clearGroup(unsigned long groupId) {
  Node* prev = &this->head;
  Node* node = this->head.next;
  while(node != NULL) {
    bool removeIt = node->groupId == groupId;
    if(removeIt) {
      prev->next = node->next;
      delete node;
    }
    else{
      prev = node;
    }
    node = prev->next;
  }
}

Scheduler::Group Scheduler::group() {
  return Group(*this, this->getNextGroupId());
}



Scheduler::Group::Group(Scheduler& scheduler, unsigned long id): scheduler(scheduler), id(id) {

}


void Scheduler::Group::abort() {
  this->scheduler.clearGroup(this->id);
}

}
