#ifndef LILOS_TASK_HH_
#define LILOS_TASK_HH_

/*
 * Cooperative multitasking support.
 *
 * General use:
 *  1. Create one or more Tasks.
 *  2. Call schedule() for each task.
 *  3. Call startTasking.
 *  4. To pass control to another task, call yield().
 *
 * New tasks can be created at any time, simply by constructing a Task object
 * and calling schedule().  The task will be in the rotation at the next call to
 * yield() or startTasking().
 *
 * Tasks cannot be *removed* from scheduling at this time.
 */

#include <stdint.h>
#include <stddef.h>

namespace lilos {

#define NORETURN void __attribute__((noreturn))

typedef uint8_t *stack_t;

typedef NORETURN (*main_t)();

class Task {
  stack_t _sp;
  Task *_next;
  enum {
    RUNNABLE = 0,
    WAITING = 1,
  } _status;

public:
  Task(main_t entry, uint8_t *stack, size_t stackSize);

  stack_t &sp() { return _sp; }
  Task *next() { return _next; }

  bool runnable() { return _status == RUNNABLE; }

  void schedule();
};

NORETURN startTasking();
void yield();

}  // namespace lilos

#endif  // LILOS_TASK_HH_
