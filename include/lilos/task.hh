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

#include <lilos/util.hh>

namespace lilos {

typedef uint8_t *stack_t;

typedef NORETURN (*main_t)();

/*
 * The type of message arguments and return values.  Large enough for a single
 * pointer-sized value.  To pass anything larger, pass its address.
 */
typedef uintptr_t msg_t;

class Task;
class TaskList {
  Task * volatile _head;
  Task * volatile _tail;

public:
  TaskList() : _head(0), _tail(0) {}

  Task *head();
  Task *tail();
  bool empty() { return !head(); }

  Task *headNonAtomic() { return _head; }

  void appendAtomic(Task *);
  void removeAtomic(Task *);
};

class Task {
  /*
   * Each task has its own stack pointer and code pointer.  When the task is
   * at rest (i.e. it has called yield()), the code pointer is pushed onto the
   * stack, and the resulting stack pointer is stored here.
   *
   * This is the first member of the class to simplify context save/restore,
   * which both access this from assembly language.
   */
  stack_t _sp;

  /*
   * Each task is a member of a TaskList, which is a doubly-linked list of
   * tasks in some common state.  These pointers contain the links.
   */
  Task * volatile _next;
  Task * volatile _prev;
  TaskList * volatile _container;

  /*
   * When task A sends a message to task B, task A leaves the runnable list
   * and joins task B's waiter list.
   */
  TaskList _waiters;

  /*
   * When this task is blocked sending a message to another task, _message
   * contains the argument.  After the receiving task returns, _message contains
   * the response.
   */
  msg_t _message;

public:
  Task(main_t entry, uint8_t *stack, size_t stackSize);

  stack_t &sp() { return _sp; }

  Task *next();
  Task *prev();

  Task *nextNonAtomic() { return _next; }

  msg_t &message() { return _message; }
  TaskList &waiters() { return _waiters; }

  void detach() { _container->removeAtomic(this); }

  friend class TaskList;
};


/*
 * Messaging
 */
msg_t send(Task *, msg_t);
Task *receive();
void answer(Task *, msg_t);

Task *currentTask();
void schedule(Task *);
NORETURN startTasking();
void NEVER_INLINE yield();

// Removes this task from the ready list and yields.
void detachAndYield();

void taskDump();

}  // namespace lilos

/*
 * This convenience macro declares a task's control structure, stack, and main
 * routine, all in one go.  It's intended to be used like this:
 *
 *  TASK(myTask, 64) {
 *    while (1) {
 *      do work
 *    }
 *  }
 */
#define TASK(name, stackSize) \
  uint8_t name ## Stack[stackSize]; \
  NORETURN name ## Main(void); \
  lilos::Task name(name ## Main, name ## Stack, stackSize); \
  NORETURN name ## Main ()

#endif  // LILOS_TASK_HH_
