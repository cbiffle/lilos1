#include "task.hh"

namespace lilos {

static TaskList tasks;
static TaskList::Item *currentTaskItem = 0;

static inline void saveContext(uint8_t **sp) {
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
  : "e"(sp)
  );
}

static inline void restoreContext(uint8_t *sp) {
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
  currentTaskItem = tasks.head();
  restoreContext(currentTaskItem->value());
  
  // gcc is smart enough to recognize that this function does, in fact, return.
  // The code below is a total hack to fool it into allowing NORETURN here.
  asm volatile ("ret");
  while (1);
}
void yield() {
  saveContext(&currentTaskItem->value());
  TaskList::Item *next = currentTaskItem->next();
  if (!next) next = tasks.head();
  currentTaskItem = next;
  restoreContext(next->value());
}

#define _PUSH(x) *(sp--) = (uint8_t) x
static const uint8_t kSregIntEnabled = 0x00;
Task::Task(main_t entry, uint8_t *stack, size_t stackSize) : _listItem(0) {
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

  _listItem.value() = sp;
  tasks.insert(&_listItem);
}
#undef _PUSH

}  // namespace lilos
