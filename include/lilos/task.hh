/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_TASK_HH_
#define LILOS_TASK_HH_

/*
 * Cooperative multitasking support.
 *
 * General use:
 *  1. Create one or more Tasks.
 *  2. Call schedule() for each task.
 *  3. Call startTasking().
 *  4. To pass control to another task, call yield() or use send().
 *
 * New tasks can be created at any time, simply by constructing a Task object
 * and calling schedule().  The task will be in the rotation at the next call to
 * yield(), send(), or startTasking().
 */

#include <stdint.h>
#include <stddef.h>

#include <lilos/util.hh>
#include <lilos/atomic.hh>

namespace lilos {

typedef uint8_t *stack_t;

typedef NORETURN (*main_t)();

/*
 * The type of message arguments and return values.  Large enough for a single
 * pointer-sized value.  To pass anything larger, pass its address.
 */
typedef uintptr_t msg_t;

class Task;

/*
 * A list of tasks, as its name implies.  TaskList is used for queueing tasks
 * that share a common state: the central Ready List, wait queues, etc.
 *
 * TaskLists are doubly-linked, with the link pointers stored in the Tasks
 * themselves.  By implication, a Task can only belong to a single TaskList at
 * a given time.
 */
class TaskList {
  Task * volatile _head;
  Task * volatile _tail;

public:
  TaskList() : _head(0), _tail(0) {}

  // Atomically retrieves the first task.
  Task *head();
  // Atomically retrieves the last task.
  Task *tail();
  // Atomically checks whether the list is empty.
  bool empty() { return !head(); }

  /*
   * Ensures that the given Task is in this list.  If the Task was not
   * previously in the list, it becomes the new last element.  If it was already
   * in the list, its position is unchanged.
   *
   * This function operates on task links in a non-atomic manner and must be
   * called with interrupts disabled.
   */
  void append(Task *);

  /*
   * Ensures that the given Task is not in this list.  If the Task was in the
   * list, it is removed; otherwise, nothing changes.
   *
   * This function operates on task links in a non-atomic manner and must be
   * called with interrupts disabled.
   */
  void remove(Task *);

  /*
   * Like head(), but not atomic.  This is only safe for use in ISRs or in
   * contexts where interrupts are disabled.  When in doubt, use head().
   */
  Task *headNonAtomic() { return _head; }

};

/*
 * A single thread of control, with dedicated stack.
 */
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
   * Links for the containing TaskList, if any.  _next and _prev will be NULL
   * iff this task is the last or first in the list (respectively) or the task
   * is not a member of any list (in which case _container is also NULL).
   */
  Task * volatile _next;
  Task * volatile _prev;

  // A pointer to the containing TaskList, or NULL.
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
  /*
   * Prepares a new Task, but does not schedule it.  (See schedule(), below.)
   *
   *  entry: a pointer to the Task's outer loop function, which must not return.
   *  stack: a pointer to the *lowest* address in the task's stack area.
   *  stackSize: number of bytes in the stack area.
   */
  Task(main_t entry, uint8_t *stack, size_t stackSize);

  // Returns the stack pointer.  Only valid if the Task is not running.
  stack_t &sp() { return _sp; }

  // Checks whether this task is in a certain TaskList.
  bool in(TaskList *tl) { return _container == tl; }

  /*
   * next() and prev() retrieve the next or previous task in the containing
   * list, respectively.  next() and prev() will return NULL if this Task is
   * last or first in the list (respectively) or if this Task is not a member
   * of any list.
   *
   * These functions operate on task links in a non-atomic manner and must be
   * called with interrupts disabled.
   */
  Task *next() { return _next; }
  Task *prev() { return _prev; }

  /*
   * Returns a reference to this task's messaging slot.  If the task is blocked
   * sending a message, the slot contains the message being sent.  Otherwise,
   * it contains the value returned by the last completed message.
   */
  msg_t message() { return _message; }

  // Sets the task's messaging slot, e.g. to provide a response.
  void setMessage(msg_t msg) { _message = msg; }

  /*
   * Convenience form of message() that deals with pointer types.
   */
  template<typename T>
  T message() { return &*reinterpret_cast<T>(_message); }

  /*
   * Convenience form of setMessage() that deals with pointer types.
   */
  template<typename T>
  void setMessage(T msg) { _message = reinterpret_cast<msg_t>(&*msg); }

  // The list containing all tasks blocked sending messages to this task.
  TaskList &waiters() { return _waiters; }

  /*
   * Removes this Task from its containing list.  If the Task is not a member
   * of any list, nothing changes.
   *
   * This function operates on task links in a non-atomic manner and must be
   * called with interrupts disabled.
   */
  void detach();

  friend class TaskList;
};


/*
 * Basic Task API
 */

/*
 * If the given Task is not in any TaskList, adds it to the Ready List, so that
 * it will execute on some future blocking call.  Otherwise -- including if the
 * Task is already in the Ready List -- nothing changes.
 *
 * This function's effects are atomic.
 */
void schedule(Task *);

/*
 * Permanently abandons the calling program and starts running Tasks from the
 * Ready List instead.  This is only safe to call once, during startup.
 */
NORETURN startTasking();

/*
 * Pauses execution of the calling Task, allowing some other Task from the
 * Ready List to run.
 */
void yield();

// Returns a pointer the currently executing Task.
Task *currentTask();

/*
 * Synchronous messaging support
 *
 * Example server task that increments numbers:
 *
 *  TASK(incrementer, 32) {
 *    while (1) {
 *      Task *sender = receive();
 *      msg_t input = sender->message();
 *      answer(sender, input + 1);
 *    }
 *  }
 *
 * Example client library function:
 *
 *  uint16_t increment(uint16_t input) {
 *    return (uint16_t) send(incrementer, (msg_t) input);
 *  }
 */

/*
 * Messages can be sent to a specific Task, or to a TaskList.  In the latter
 * case, the sender queues itself on the TaskList, which may be serviced at a
 * later date by some other Task.  This can be used to implement mutexes, for
 * example.
 *
 * In either case, the sending Task blocks until some other Task wakes it up
 * using answer().  When the sender wakes up, the value returned by send() will
 * be the same one passed to answer().
 */
msg_t send(Task *, msg_t);
msg_t send(TaskList *, msg_t);
msg_t sendVoid(Task *);
msg_t sendVoid(TaskList *);

/*
 * Convenience templates for send that take pointer values.
 */
template <typename A, typename R>
R *sendPtr(Task *t, A *arg) {
  return send(t, reinterpret_cast<msg_t>(arg));
}

template <typename A, typename R>
R *sendPtr(TaskList *tl, A *arg) {
  return send(tl, reinterpret_cast<msg_t>(arg));
}

/*
 * Further convenience templates that don't require a response.
 */
template <typename A>
void sendPtr(Task *t, A *arg) {
  send(t, reinterpret_cast<msg_t>(arg));
}

template <typename A>
void sendPtr(TaskList *tl, A *arg) {
  send(tl, reinterpret_cast<msg_t>(arg));
}


/*
 * Returns the Task that has been waiting longest to send a message to the
 * current task.  This is a convenience function for the common case of handling
 * senders in FIFO order; it is equivalent to
 *
 *  currentTask()->waiters().head()
 *
 * Note that receive() does not remove the sender from the queue.  Calling it
 * repeatedly, without calling answer() or manually manipulating the queue, will
 * return the same Task every time.
 */
Task *receive();

/*
 * Wakes a Task from send(), with the given return value.
 */
void answer(Task *, msg_t);

// Convenience version of answer for senders who don't care about the result.
void answerVoid(Task *);

// For debugging only: writes task info using the debug API.
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
