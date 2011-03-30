#ifndef LILOS_TASK_HH_
#define LILOS_TASK_HH_

/*
 * Cooperative multitasking support.
 *
 * General use:
 *  1. Create one or more Tasks.
 *  2. Call startTasking.
 *  3. To pass control to another task, call yield().
 *
 * New tasks can be created at any time, simply by constructing a Task object.
 * The task will be in the rotation at the next call to yield() or
 * startTasking().
 *
 * Tasks cannot be *removed* from scheduling at this time.
 */

#include <stdint.h>
#include <stddef.h>

#include "list.hh"

namespace lilos {

#define NORETURN void __attribute__((noreturn))

typedef uint8_t *stack_t;

typedef list::List<stack_t> TaskList;

typedef NORETURN (*main_t)();

class Task {
  TaskList::Item _listItem;

public:
  Task(main_t entry, uint8_t *stack, size_t stackSize);
};

NORETURN startTasking();
void yield();

}  // namespace lilos

#endif  // LILOS_TASK_HH_
