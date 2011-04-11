/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_BOARD_DEBUG_HH_
#define LILOS_BOARD_DEBUG_HH_

namespace lilos {

class USART;
extern USART usart0;

static USART &debugUsart = usart0;
static const uint32_t kDebugBaudrate = 38400;

};

#endif  // LILOS_BOARD_DEBUG_HH_
