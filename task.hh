#ifndef LILOS_TASK_HH_
#define LILOS_TASK_HH_

#include <stdint.h>
#include <stddef.h>

#include "list.hh"

namespace lilos {

#define NORETURN void __attribute__((noreturn))

typedef list::List<uint8_t *> TaskList;

typedef NORETURN (*main_t)();

class Task {
  TaskList::Item _listItem;

public:
  Task(main_t entry, uint8_t *stack, size_t stackSize);
};

/*
 * A task is represented by its current stack pointer.  We don't maintain stack
 * base or bounds, though both might be useful for error checking.
 */
typedef volatile uint8_t *task_t;
typedef task_t *task_handle_t;


task_t initTask(main_t entry, uint8_t *stack, size_t stackSize);

extern "C" volatile task_handle_t CurrentTask;

void startTasking(task_handle_t);
void yieldTo(task_handle_t);

}  // namespace lilos

#endif  // LILOS_TASK_HH_
