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
typedef void (*Delay)(unsigned long);


void usDelay(unsigned long us);


class Scheduler {
public:
  Scheduler(TimeProvider timeProvider, Delay delay);
  static Scheduler millis() {
    return Scheduler(::millis, ::delay);
  }
  static Scheduler micros() {
    return Scheduler(::micros, usDelay);
  }
  virtual ~Scheduler();
  unsigned long tick();
  void loop();
  bool isEmpty() const;
  unsigned int count() const;
  void debug(Stream& stream) const;


  template<typename Callable>
  Scheduler& timeout(unsigned long delta, Callable callable) {
    unsigned long when = this->timeProvider() + delta;
    this->addNode(new BaseNode<Callable>(when, callable));
    return *this;
  }


  template<typename Callable>
  Scheduler& every(unsigned long interval, Callable callable) {
    unsigned long when = this->timeProvider() + interval;
    this->addNode(new PeriodicNode<Callable>(when, interval, callable));
    return *this;
  }

  template<typename Callable>
  Scheduler& every(unsigned long firstInterval, unsigned long interval, Callable callable) {
    unsigned long when = this->timeProvider() + firstInterval;
    this->addNode(new PeriodicNode<Callable>(when, interval, callable));
    return *this;
  }


  template<typename Callable>
  Scheduler& repeat(unsigned int times, unsigned long interval, Callable callable) {
    if(times == 0) return *this;
    unsigned long when = this->timeProvider() + interval;
    this->addNode(new RepeatableNode<Callable>(when, times, interval, callable));
    return *this;
  }


  template<typename Callable>
  Scheduler& repeat(unsigned int times, unsigned long firstInterval,  unsigned long interval, Callable callable) {
    if(times == 0) return *this;
    unsigned long when = this->timeProvider() + firstInterval;
    this->addNode(new RepeatableNode<Callable>(when, times, interval, callable));
    return *this;
  }

  class Node {
    friend Scheduler;
    public:
      Node();
      Node(unsigned long when);
      /**
       * returns true if it is done
       */
      virtual bool run() { return true;};
      virtual ~Node() {};

      bool isAfter(const Node& node) const;
      bool isAfter(unsigned long delta) const;
      bool isBefore(const Node& node) const;
      bool isBefore(unsigned long delta) const;
      bool hasNext() const;
      unsigned long leftTime(unsigned long delta) const;

      void setNext(Node* next);
      void remove();


      virtual void debug(Stream& stream) const;
    private:
      Node* next;
      unsigned long when;
  };


  template<typename Callable>
  class BaseNode: public Node {
    protected:
      Callable callable;
    public:

      bool run() {
        this->callable();
        return true;
      }

      BaseNode(unsigned long when, Callable callable) : Node(when), callable(callable) {

      }
  };

  template<typename Callable>
  class PeriodicNode: public Node {
    protected:
      Callable callable;
      unsigned long interval;
    public:

      bool run() {
        this->callable();
        this->when += this->interval;
        return false;
      }

      PeriodicNode(unsigned long when, unsigned long interval, Callable callable) : Node(when), callable(callable), interval(interval) {

      }

      void debug(Stream& stream) const {
          stream.print("RepeatableNode {");
          stream.print(".when=");
          stream.print(this->when);
          stream.print(" .interval=");
          stream.print(this->interval);
          stream.print("}");
      }
  };

  template<typename Callable>
  class RepeatableNode: public Node {
    protected:
      Callable callable;
      unsigned int times;
      unsigned long interval;
    public:

      bool run() {
        this->times -= 1;
        this->callable();
        this->when += this->interval;
        return this->times == 0;
      }

      RepeatableNode(unsigned long when, unsigned int times, unsigned long interval, Callable callable) : Node(when),  times(times), callable(callable), interval(interval) {

      }

      void debug(Stream& stream) const {
          stream.print("RepeatableNode {");
          stream.print(".when=");
          stream.print(this->when);
          stream.print(" .times=");
          stream.print(this->times);
          stream.print(" .interval=");
          stream.print(this->interval);
          stream.print("}");
      }
  };

  Node* addNode(Node* newNode);

private:
  Node head;
  TimeProvider timeProvider;
  Delay delay;

};

};

#endif
