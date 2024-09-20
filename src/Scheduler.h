#ifndef __SCHEDULER__SCHEDULER__
#define __SCHEDULER__SCHEDULER__

#include "Arduino.h"
/**
 * https://www.arduino.cc/reference/en/language/functions/time/millis/
 * https://www.arduino.cc/reference/en/language/functions/time/micros/
 */

namespace scheduler {


class Callable {
public:
  virtual void operator()() = 0;
};

typedef unsigned long (*TimeProvider)();


class Scheduler {
public:
  Scheduler(TimeProvider timeProvider);
  static Scheduler millis() {
    return Scheduler(::millis);
  }
  static Scheduler micros() {
    return Scheduler(::micros);
  }
  virtual ~Scheduler();
  unsigned long tick(unsigned long delta);
  unsigned long tick();
  void loop();
  bool isEmpty() const;
  unsigned int count() const;


  template<typename Callable>
  Scheduler& timeout(unsigned long delta, Callable callable) {
    unsigned long when = this->timeProvider() + delta;
    this->addNode(new BaseNode<Callable>(when, callable));
    return *this;
  }


  template<typename Callable>
  Scheduler& every(unsigned long interval, Callable callable) {
    unsigned long when = this->timeProvider() + interval;
    this->addNode(new PeriodicNode<Callable>(*this, when, interval, callable));
    return *this;
  }

  template<typename Callable>
  Scheduler& every(unsigned long firstInterval, unsigned long interval, Callable callable) {
    unsigned long when = this->timeProvider() + firstInterval;
    this->addNode(new PeriodicNode<Callable>(*this, when, interval, callable));
    return *this;
  }


  template<typename Callable>
  Scheduler& repeat(unsigned int times, unsigned long interval, Callable callable) {
    if(times == 0) return *this;
    unsigned long when = this->timeProvider() + interval;
    this->addNode(new RepeatableNode<Callable>(*this, when, times, interval, callable));
    return *this;
  }


  template<typename Callable>
  Scheduler& repeat(unsigned int times, unsigned long firstInterval,  unsigned long interval, Callable callable) {
    if(times == 0) return *this;
    unsigned long when = this->timeProvider() + firstInterval;
    this->addNode(new RepeatableNode<Callable>(*this, when, times, interval, callable));
    return *this;
  }

  class Node {
    friend Scheduler;
    public:
      Node();
      Node(unsigned long when);
      virtual void run() {};
      virtual ~Node() {};

      bool isAfter(const Node& node) const;
      bool isAfter(unsigned long delta) const;
      bool isBefore(const Node& node) const;
      bool isBefore(unsigned long delta) const;
      bool hasNext() const;
      unsigned long leftTime(unsigned long delta) const;

      void setNext(Node* next);
      void setPrev(Node* prev);
      void remove();
    private:
      Node* next;
      Node* prev;
      unsigned long when;
  };


  template<typename Callable>
  class BaseNode: public Node {
    protected:
      Callable callable;
    public:

      void run() {
        this->callable();
      }

      BaseNode(unsigned long when, Callable callable) : Node(when), callable(callable) {

      }
  };

  template<typename Callable>
  class PeriodicNode: public Node {
    protected:
      Scheduler& scheduler;
      Callable callable;
      unsigned long interval;
    public:

      void run() {
        this->callable();
        this->scheduler.every(this->interval, this->callable);
      }

      PeriodicNode(Scheduler& scheduler, unsigned long when, unsigned long interval, Callable callable) : Node(when),  scheduler(scheduler), callable(callable), interval(interval) {

      }
  };

  template<typename Callable>
  class RepeatableNode: public Node {
    protected:
      Scheduler& scheduler;
      Callable callable;
      unsigned int times;
      unsigned long interval;
    public:

      void run() {
        this->callable();
        if(this->times > 1) {
          this->scheduler.repeat(this->times - 1, this->interval, this->callable);
        }
      }

      RepeatableNode(Scheduler& scheduler, unsigned long when, unsigned int times, unsigned long interval, Callable callable) : Node(when),  scheduler(scheduler), times(times), callable(callable), interval(interval) {

      }
  };

  Node* addNode(Node* newNode);

private:
  Node head;
  TimeProvider timeProvider;

};

};

#endif
