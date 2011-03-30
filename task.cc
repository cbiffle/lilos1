#include "task.hh"

namespace lilos {

volatile task_handle_t CurrentTask = 0;

static inline void saveContext() {
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
    "lds r26, CurrentTask \n\t"
    "lds r27, CurrentTask + 1 \n\t"
    "in r0, __SP_L__ \n\t"
    "st X+, r0 \n\t"
    "in r0, __SP_H__ \n\t"
    "st X+, r0 \n\t"
  );
}

static inline void restoreContext() {
  asm volatile (
    // Stack pointer
    "lds r26, CurrentTask \n\t"
    "lds r27, CurrentTask + 1 \n\t"
    "ld r0, X+ \n\t"
    "out __SP_L__, r0 \n\t"
    "ld r0, X+ \n\t"
    "out __SP_H__, r0 \n\t"
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
  );
}

void startTasking(task_handle_t firstTask) {
  CurrentTask = firstTask;
  restoreContext();
}
void yieldTo(task_handle_t newTask) {
  saveContext();
  CurrentTask = newTask;
  restoreContext();
}

#define _PUSH(x) *(sp--) = (uint8_t) x
static const uint8_t kSregIntEnabled = 0x00;
task_t initTask(main_t entry, uint8_t *stack, size_t stackSize) {
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

  return sp;
}
#undef _PUSH

}  // namespace lilos
