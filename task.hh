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

NORETURN startTasking();
void yield();

}  // namespace lilos

#endif  // LILOS_TASK_HH_
