# Tiny Scheduler

A lightweight and flexible task scheduling library for Arduino, allowing users to manage time-based events such as timeouts, periodic tasks, and repeated executions with an intuitive API.

This Arduino scheduling library offers a simple yet powerful way to manage timed tasks. Whether you need to run a task after a delay, repeat an action periodically, or execute a function multiple times with custom intervals, this library provides an easy-to-use interface. Built with flexibility in mind, the library supports passing functions, lambdas, or callable objects, giving you full control over scheduling behaviors.

Key features include:

**Timeouts**: Schedule tasks to run after a specified delay.

**Periodic tasks**: Repeat tasks at fixed intervals.

**Repetitive execution**: Execute tasks a set number of times, with customizable intervals between runs.

**Flexible API**: Use any callable (functions, lambdas, functors) as tasks.

**Low overhead**: Optimized for minimal memory usage, perfect for resource-constrained Arduino boards

---

## Alternative Installation

Download the library or clone the repository.
Place the folder inside your Arduino libraries directory (Arduino/libraries).
Include the library in your sketches with #include <scheduler.h>.
