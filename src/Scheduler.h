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

  class Group;

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

  void clear();
  Group group();

  template<typename Callable>
  Scheduler& timeout(unsigned long delta, Callable callable) {
    unsigned long time =  this->timeProvider();
    unsigned long when = time + delta;
    bool overflow = when < time;
    this->addNode((new BaseNode<Callable>(when, callable))->withOverflow(overflow));
    return *this;
  }


  template<typename Callable>
  Scheduler& every(unsigned long interval, Callable callable) {
    unsigned long time =  this->timeProvider();
    unsigned long when = time + interval;
    bool overflow = when < time;
    this->addNode((new PeriodicNode<Callable>(when, interval, callable))->withOverflow(overflow));
    return *this;
  }

  template<typename Callable>
  Scheduler& every(unsigned long firstInterval, unsigned long interval, Callable callable) {
    unsigned long time =  this->timeProvider();
    unsigned long when = time + firstInterval;
    bool overflow = when < time;
    this->addNode((new PeriodicNode<Callable>(when, interval, callable))->withOverflow(overflow));
    return *this;
  }


  template<typename Callable>
  Scheduler& repeat(unsigned int times, unsigned long interval, Callable callable) {
    if(times == 0) return *this;
    unsigned long time =  this->timeProvider();
    unsigned long when = time + interval;
    bool overflow = when < time;
    this->addNode((new RepeatableNode<Callable>(when, times, interval, callable))->withOverflow(overflow));
    return *this;
  }


  template<typename Callable>
  Scheduler& repeat(unsigned int times, unsigned long firstInterval,  unsigned long interval, Callable callable) {
    if(times == 0) return *this;
    unsigned long time =  this->timeProvider();
    unsigned long when = time + firstInterval;
    bool overflow = when < time;
    this->addNode((new RepeatableNode<Callable>(when, times, interval, callable))->withOverflow(overflow));
    return *this;
  }

  class Node {
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
      bool isAfter(bool overflow, unsigned long delta) const;
      bool isBefore(const Node& node) const;
      bool isBefore(unsigned long delta) const;
      bool isBefore(bool overflow, unsigned long delta) const;
      bool isOverflow() const;
      bool hasNext() const;
      unsigned long leftTime(unsigned long delta) const;

      void setNext(Node* next);
      void remove();
      Node* withGroupId(unsigned long groupId);
      Node* withOverflow(bool overflow);


      virtual void debug(Stream& stream) const;
    private:

      friend Scheduler;
      Node* next;
      bool overflow = false;
      unsigned long when;
      unsigned long groupId = 0;
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
        unsigned long oldWhen = this->when;
        this->when += this->interval;
        this->overflow = this->when < oldWhen;
        return false;
      }

      PeriodicNode(unsigned long when, unsigned long interval, Callable callable) : Node(when), callable(callable), interval(interval) {

      }

      void debug(Stream& stream) const {
          stream.print("RepeatableNode {");
          stream.print(" .groupId=");
          stream.print(this->groupId);
          stream.print(" .when=");
          stream.print(this->when);
          stream.print(" .interval=");
          stream.print(this->interval);
          stream.print(" .overflow=");
          stream.print(this->overflow);
          stream.print(" }");
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
        unsigned long oldWhen = this->when;
        this->when += this->interval;
        this->overflow = this->when < oldWhen;
        return this->times == 0;
      }

      RepeatableNode(unsigned long when, unsigned int times, unsigned long interval, Callable callable) : Node(when),  times(times), callable(callable), interval(interval) {

      }

      void debug(Stream& stream) const {
          stream.print("RepeatableNode {");
          stream.print(" .groupId=");
          stream.print(this->groupId);
          stream.print(" .when=");
          stream.print(this->when);
          stream.print(" .times=");
          stream.print(this->times);
          stream.print(" .interval=");
          stream.print(this->interval);
          stream.print(" .overflow=");
          stream.print(this->overflow);
          stream.print(" }");
      }
  };


  class Group {
  public:
    void clear();

    template<typename Callable>
    Group& timeout(unsigned long delta, Callable callable) {
      unsigned long time =  this->scheduler.timeProvider();
      unsigned long when = time + delta;
      bool overflow = when < time;
      this->scheduler.addNode((new BaseNode<Callable>(when, callable))->withGroupId(this->id)->withOverflow(overflow));
      return *this;
    }


    template<typename Callable>
    Group& every(unsigned long interval, Callable callable) {
      unsigned long time =  this->scheduler.timeProvider();
      unsigned long when = time + interval;
      bool overflow = when < time;
      this->scheduler.addNode((new PeriodicNode<Callable>(when, interval, callable))->withGroupId(this->id)->withOverflow(overflow));
      return *this;
    }

    template<typename Callable>
    Group& every(unsigned long firstInterval, unsigned long interval, Callable callable) {
      unsigned long time =  this->scheduler.timeProvider();
      unsigned long when = time + firstInterval;
      bool overflow = when < time;
      this->scheduler.addNode((new PeriodicNode<Callable>(when, interval, callable))->withGroupId(this->id)->withOverflow(overflow));
      return *this;
    }


    template<typename Callable>
    Group& repeat(unsigned int times, unsigned long interval, Callable callable) {
      if(times == 0) return *this;
      unsigned long time =  this->scheduler.timeProvider();
      unsigned long when = time + interval;
      bool overflow = when < time;
      this->scheduler.addNode((new RepeatableNode<Callable>(when, times, interval, callable))->withGroupId(this->id)->withOverflow(overflow));
      return *this;
    }


    template<typename Callable>
    Group& repeat(unsigned int times, unsigned long firstInterval,  unsigned long interval, Callable callable) {
      if(times == 0) return *this;
      unsigned long time =  this->scheduler.timeProvider();
      unsigned long when = time + firstInterval;
      bool overflow = when < time;
      this->scheduler.addNode((new RepeatableNode<Callable>(when, times, interval, callable))->withGroupId(this->id)->withOverflow(overflow));
      return *this;
    }

  private:
    friend Scheduler;
    Group(Scheduler& scheduler, unsigned long id);
    unsigned long id;
    Scheduler& scheduler;
  };

  Node* addNode(Node* newNode);

private:
  friend Group;
  Node head;
  TimeProvider timeProvider;
  Delay delay;
  unsigned long lastTick = 0;
  unsigned long nextGroupId = 1;

  void handleNode(Node* node);
  void handleOverflow();
  void clearGroup(unsigned long groupId);

  unsigned long getNextGroupId() {
    return this->nextGroupId++;
  }

};

};

#endif
