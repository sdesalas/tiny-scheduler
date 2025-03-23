# Tiny Scheduler

[![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)](https://www.arduino.cc/reference/en/libraries/tinyscheduler/)

A lightweight and flexible task scheduling library for Arduino, allowing users to manage time-based events such as timeouts, periodic tasks, and repeated executions with an intuitive API.

This Arduino scheduling library offers a simple yet powerful way to manage timed tasks. Whether you need to run a task after a delay, repeat an action periodically, or execute a function multiple times with custom intervals, this library provides an easy-to-use interface. Built with flexibility in mind, the library supports passing functions, lambdas, or callable objects, giving you full control over scheduling behaviors.

Inspired by: <https://github.com/OpenCIAg/BlueFairy>

## Why?

### The "normal" blink

When working with microcontrollers like Arduino, a common beginner task is to blink an LED using the `delay()` function.
This is a simple approach but introduces significant limitations, especially when more complex interactions are needed.

```cpp
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(1000);                     
  digitalWrite(LED_BUILTIN, LOW);  
  delay(1000);                     
}
```

<https://docs.arduino.cc/built-in-examples/basics/Blink/>

However, this approach pauses the entire program for the duration of the `delay()`.
During this time, the microcontroller can't perform any other tasks, which means any additional functionality, like reading sensor data or responding to user inputs (like button presses), would be ignored until the `delay()` finishes.
This creates a bottleneck in your program and limits what the microcontroller can do.

### Blink Without Delay

To overcome this, a common alternative is to use the `millis()` function. With `millis()`, instead of pausing the program, you track time manually by checking how much time has passed since the last action.
This approach is non-blocking and allows you to handle multiple tasks in parallel, such as blinking an LED while also checking for button presses.
However, even though `millis()` is more efficient, the code can become complex, especially as the number of tasks grows, as each task needs to manage its own time-tracking logic.

```cpp
const int ledPin = LED_BUILTIN;
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }
}
```

<https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay/>

### Scheduler Blink

This is where the TinyScheduler comes in.
TinyScheduler simplifies time-based task scheduling by allowing you to define tasks with intervals, without manually managing time with `millis()` for each task.
Instead, you define tasks in a more structured way, and TinyScheduler handles the timing logic, making your code more readable, maintainable, and scalable.

```cpp
// examples/Blink/Blink.ino

#include <Scheduler.h>


Scheduler msScheduler = Scheduler::millis();

const int LED_PIN = LED_BUILTIN;
int LED_VALUE = 0;

void toggleLed() {
  LED_VALUE = (LED_VALUE + 1) % 2;
  digitalWrite(LED_PIN, LED_VALUE);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_VALUE);
  msScheduler.every(1000, toggleLed);
}

void loop() {
  msScheduler.loop();
}
```

Using TinyScheduler, you can schedule the LED to blink without blocking other tasks and add more time-based actions, like reading sensors or controlling other devices, without adding complexity to your program.
It helps avoid delays and makes your microcontroller code more responsive and efficient, especially when handling multiple tasks simultaneously.

## TinyScheduler vs FreeRTOS

TinyScheduler offers a simpler, more flexible approach to task scheduling by supporting modern C++ features like lambdas and templates, which enhance code reuse and readability.
Unlike FreeRTOS, which is a full-fledged real-time operating system with advanced features like task prioritization, mutexes, and semaphores, TinyScheduler focuses on lightweight task management without the overhead of a full RTOS.
It’s not meant to replace FreeRTOS but can be used alongside it when you need a minimal scheduler for certain tasks, giving you flexibility depending on the complexity of your project.

## Key features include

**Timeouts**: Schedule tasks to run after a specified delay.

**Periodic tasks**: Repeat tasks at fixed intervals.

**Repetitive execution**: Execute tasks a set number of times, with customizable intervals between runs.

**Flexible API**: Use any callable (functions, lambdas, functors) as tasks.

**Low overhead**: Optimized for minimal memory usage, perfect for resource-constrained Arduino boards

**Task Groups**: Organize related tasks into groups, allowing you to manage them collectively, such as canceling all tasks within a group at once, improving modularity and control over task management.

## Alternative Installation

Download the library or clone the repository.
Place the folder inside your Arduino libraries directory (Arduino/libraries).
Include the library in your sketches with `#include <Scheduler.h>`.

## Examples

```cpp
// examples/Blink2/Blink2.ino

#include <Scheduler.h>


Scheduler msScheduler = Scheduler::millis();

const int LED_PIN = LED_BUILTIN;
int LED_VALUE = 0;

void toggleLed() {
  LED_VALUE = (LED_VALUE + 1) % 2;
  digitalWrite(LED_PIN, LED_VALUE);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_VALUE);
  msScheduler
    .repeat(5, 250, 500, toggleLed)
    .repeat(5, 0, 500, toggleLed)
    .every(3000, 1000, toggleLed);
  msScheduler.debug(Serial);
}

void loop() {
  msScheduler.loop();
}
```
