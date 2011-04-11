/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_USART_HH_
#define LILOS_USART_HH_

#include <stddef.h>

#include <lilos/pgmspace.hh>
#include <lilos/static_assert.hh>
#include <lilos/task.hh>

namespace lilos {

// Opaque declaration of hardware USART struct.
class USARTRegisters;

/*
 * A USART peripheral.  Each particular type of MCU has static instances of
 * this class defined in mcu/$(MCU)/include/lilos/mcu_usart.hh.
 */
class USART {
  USARTRegisters *_reg;
  TaskList _transmitTasks;
  TaskList _receiveTasks;

public:
  enum DataBits {
    DATA_5,
    DATA_6,
    DATA_7,
    DATA_8,
    DATA_9,
  };

  enum Parity {
    PARITY_NONE,
    PARITY_EVEN,
    PARITY_ODD,
  };

  enum StopBits {
    STOP_1,
    STOP_2,
  };


  USART(USARTRegisters *reg) : _reg(reg) {}

  void initialize(uint32_t baudrate, DataBits, Parity, StopBits);

  bool available();
  uint8_t read();

  void write(uint8_t);
  void write(const uint8_t *, size_t);
  void write_P(const prog_char *, size_t);

  /*
   * These functions are intended for use from interrupt handlers,
   * but could be appropriated for other purposes....
   */
  bool senderWaiting();
  uint8_t readAndUnblockSender();
  bool receiverWaiting();
  void unblockReceiver(uint8_t);

private:
  uint8_t readNow();
  void activateTransmission();
};

}  // namespace lilos

#include <lilos/mcu_usart.hh>

#endif  // LILOS_USART_HH_
