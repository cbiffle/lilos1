#include <util/atomic.h>

#include <lilos/atomic.hh>
#include <lilos/task.hh>
#include <lilos/util.hh>
#include <lilos/debug.hh>
#include <lilos/pgmspace.h>

namespace lilos {

static TaskList readyList;
static Task * volatile currentTask = 0;

/*
 * Context save/restore
 *
 * You might note that the functions below save only a subset of registers.
 * This is deliberate: these are the 'callee-save' registers under avr-gcc's
 * calling conventions.  We can rely on the compiler to save others before
 * entering yield() or startTasking().  This saves 48 cycles per task switch
 * on ATmega, 24 on xmega.
 */

static ALWAYS_INLINE void saveContext(stack_t *spp) {
  asm volatile (
    "push r0 \n\t"
    "in r0, __SREG__ \n\t"
    "cli \n\t"
    "push r0 \n\t"
    "push r1 \n\t"
    // Callee-saved registers
    "push r2 \n\t"
    "push r3 \n\t"
    "push r4 \n\t"
    "push r5 \n\t"
    "push r6 \n\t"
    "push r7 \n\t"
    "push r8 \n\t"
    "push r9 \n\t"
    "push r10 \n\t"
    "push r11 \n\t"
    "push r12 \n\t"
    "push r13 \n\t"
    "push r14 \n\t"
    "push r15 \n\t"
    "push r16 \n\t"
    "push r17 \n\t"
    "push r28 \n\t"
    "push r29 \n\t"
    // Stack pointer
    "in r0, __SP_L__ \n\t"
    "st %a0, r0 \n\t"
    "in r0, __SP_H__ \n\t"
    "std %a0+1, r0 \n\t"
  : /* no output */
  : "b"(spp)
  );
}

static ALWAYS_INLINE void restoreContext(stack_t sp) {
  asm volatile (
    // Stack pointer
    "out __SP_L__, %A0 \n\t"
    "out __SP_H__, %B0 \n\t"
    // Callee-save registers
    "pop r29  \n\t"
    "pop r28  \n\t"
    "pop r17  \n\t"
    "pop r16  \n\t"
    "pop r15  \n\t"
    "pop r14  \n\t"
    "pop r13  \n\t"
    "pop r12  \n\t"
    "pop r11  \n\t"
    "pop r10  \n\t"
    "pop r9  \n\t"
    "pop r8  \n\t"
    "pop r7  \n\t"
    "pop r6  \n\t"
    "pop r5  \n\t"
    "pop r4  \n\t"
    "pop r3  \n\t"
    "pop r2  \n\t"
    "pop r1  \n\t"
    // SREG
    "pop r0  \n\t"
    "out __SREG__, r0  \n\t"
    "pop r0 \n\t"
  : /* no output */
  : "r"(sp)
  );
}

NORETURN startTasking() {
  currentTask = readyList.head();
  restoreContext(currentTask->sp());
  
  // gcc is smart enough to recognize that this function does, in fact, return.
  // The code below is a total hack to fool it into allowing NORETURN here.
  // It adds two bytes to the output.
  asm volatile ("ret");
  while (1);
}

Task *nextTask() {
  Task *newTask = currentTask->next();
  if (!newTask) newTask = readyList.head();
  return newTask;
}

NEVER_INLINE void yieldTo(Task *next) {
  /*
   * This is a bit of a hack.  We need somewhere to store the next task,
   * without displacing the return address on the stack.  So, we stash it in
   * this static variable.  Because yieldTo is not reentrant (by definition)
   * this is safe -- we declare it 'volatile' only so the compiler doesn't get
   * any big ideas about caching the contents in a register.
   */
  static Task * volatile _nextTask;
  _nextTask = next;

  saveContext(&currentTask->sp());
  currentTask = _nextTask;
  restoreContext(currentTask->sp());
}

void NEVER_INLINE yield() {
  if (!currentTask) return;
  saveContext(&currentTask->sp());
  Task *newTask = nextTask();
  currentTask = newTask;
  restoreContext(newTask->sp());
}

#define _PUSH(x) *(sp--) = (uint8_t) (x)
static const uint8_t kSregIntEnabled = 0x80;
Task::Task(main_t entry, uint8_t *stack, size_t stackSize)
  : _sp(0),
    _next(0),
    _prev(0) {
  uint8_t *sp = stack + stackSize - 1;

  // Code "return address" of entry routine
  uintptr_t code = (uintptr_t) entry;
  _PUSH(code);
  _PUSH(code >> 8);

  // Registers
  _PUSH(0);  // r0
  _PUSH(kSregIntEnabled);  // SREG
  _PUSH(0);  // r1
  for (int i = 2; i <= 17; i++) {
    _PUSH(i);
  }
  _PUSH(28);
  _PUSH(29);

  _sp = sp;
}
#undef _PUSH

Task *Task::next() {
  ATOMIC { return _next; }
}

Task *Task::prev() {
  ATOMIC { return _prev; }
}

Task *TaskList::head() {
  ATOMIC { return _head; }
}

Task *TaskList::tail() {
  ATOMIC { return _tail; }
}

void TaskList::appendAtomic(Task *task) {
  ATOMIC {
    task->_prev = _tail;
    task->_next = 0;
    task->_container = this;

    if (_tail) {
      _tail->_next = task;
    } else {
      _head = task;
    }
    _tail = task;
  }
}

void TaskList::removeAtomic(Task *task) {
  ATOMIC {
    if (task->_container != this) return;

    if (task->_prev) task->_prev->_next = task->_next;
    if (task->_next) task->_next->_prev = task->_prev;

    if (task == _head) _head = task->_next;
    if (task == _tail) _tail = task->_prev;

    task->_container = 0;
    task->_prev = 0;
    task->_next = 0;
  }
}

void schedule(Task *task) {
  readyList.appendAtomic(task);
}

void dump1(Task *task, uint8_t indentLevel) {
  while (indentLevel--) {
    debugWrite_P(PSTR("  "));
  }
  debugWrite((uint32_t) task);
  debugWrite_P(PSTR(" sp="));
  debugWrite((uint32_t) task->sp());
  if (task == currentTask) {
    debugWrite_P(PSTR("(you are here)"));
  } else {
    debugWrite_P(PSTR("pc="));
    union {
      struct {
        uint8_t lo;
        uint8_t hi;
      } bytes;
      uint16_t pc;
    };
    uint8_t *pcp = task->sp() + 22;  /* 21-byte context + 1-byte offset */
    bytes.lo = pcp[1];
    bytes.hi = pcp[0];
    debugWrite(pc);
  }
  debugLn();

  for (Task *w = task->waiters().head(); w; w = w->next()) {
    dump1(w, indentLevel + 1);
  }
}

void taskDump() {
  debugWrite_P(PSTR("--- task dump ---\r"));

  debugWrite_P(PSTR("Current: "));
  debugWrite((uint32_t) currentTask);
  debugLn();

  debugWrite_P(PSTR("All:\r"));
  Task *t = readyList.head();
  while (t) {
    dump1(t, 1);
    t = t->next();
  }

  debugWrite_P(PSTR("--- end task dump ---\r"));
}

msg_t send(Task *target, msg_t message) {
  // Gotta do this and store the result before calling detach()
  Task *next = nextTask();

  Task *me = currentTask;
  me->message() = message;
  
  me->detach();
  target->waiters().appendAtomic(me);

  yieldTo(next);

  return currentTask->message();
}

Task *receive() {
  Task *me = currentTask;
  while (me->waiters().empty()) yield();
  return me->waiters().head();
}

void answer(Task *sender, msg_t response) {
  sender->message() = response;
  sender->detach();
  schedule(sender);
}

}  // namespace lilos
