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
  Task *_head;
  Task *_tail;

public:
  TaskList() : _head(0), _tail(0) {}

  Task *head() { return _head; }
  Task *tail() { return _tail; }
  bool empty() { return !_head; }

  void append(Task *);
  void remove(Task *);
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
  Task *_next;
  Task *_prev;
  TaskList *_container;

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

  Task *next() { return _next; }
  Task *prev() { return _prev; }

  msg_t &message() { return _message; }
  TaskList &waiters() { return _waiters; }

  void detach() { _container->remove(this); }

  friend class TaskList;
};


/*
 * Messaging
 */
msg_t send(Task *, msg_t);
Task *receive();
void answer(Task *, msg_t);

void schedule(Task *);
NORETURN startTasking();
void NEVER_INLINE yield();

void taskDump();

}  // namespace lilos

#endif  // LILOS_TASK_HH_
