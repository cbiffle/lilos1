#include "task.hh"

namespace lilos {

static Task *firstTask = 0;
static Task *currentTask = 0;

// For saveContext/restoreContext, inlining is not just an optimization:
// it's critical to correctness.  This attribute lets us mandate it.
#define ALWAYS_INLINE inline __attribute__((always_inline))

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
  : "e"(spp)
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
  currentTask = firstTask;
  restoreContext(currentTask->sp());
  
  // gcc is smart enough to recognize that this function does, in fact, return.
  // The code below is a total hack to fool it into allowing NORETURN here.
  // It adds two bytes to the output.
  asm volatile ("ret");
  while (1);
}
void yield() {
  Task *task = currentTask;
  saveContext(&task->sp());
  
  // Busy-wait until we find a runnable task.
  // (The assumption here is that an ISR will fix things.)
  do {
    task = task->next();
    if (!task) task = firstTask;
  } while (!task->runnable());

  currentTask = task;
  restoreContext(task->sp());
}

#define _PUSH(x) *(sp--) = (uint8_t) x
static const uint8_t kSregIntEnabled = 0x80;
Task::Task(main_t entry, uint8_t *stack, size_t stackSize)
  : _sp(0),
    _status(RUNNABLE) {
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

void Task::schedule() {
  _next = firstTask;
  firstTask = this;
}

}  // namespace lilos
