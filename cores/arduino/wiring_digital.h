/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _WIRING_DIGITAL_H_
#define _WIRING_DIGITAL_H_

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configures the specified pin to behave either as an input or an output.
 *
 * \param pin The number of the pin whose mode you wish to set
 * \param mode Either INPUT, INPUT_PULLUP, INPUT_PULLDOWN or OUTPUT
 */
extern void pinMode(uint32_t pin, uint32_t mode) ;

/**
 * \brief Write a HIGH or a LOW value to a digital pin.
 *
 * If the pin has been configured as an OUTPUT with pinMode(), its voltage will be set to the
 * corresponding value: 3.3V for HIGH, 0V (ground) for LOW.
 *
 * \param pin the pin number
 * \param val HIGH or LOW
 */
extern void digitalWrite(uint32_t pin, uint32_t val) ;

/**
 * \brief Reads the value from a specified digital pin, either HIGH or LOW.
 *
 * \param pin The number of the digital pin you want to read (int)
 *
 * \return HIGH or LOW
 */
extern int digitalRead(uint32_t pin) ;

/**
 * \brief Toggle the value from a specified digital pin.
 *
 * \param pin The number of the digital pin you want to toggle (int)
 */
extern void digitalToggle(uint32_t pin) ;

#ifdef __cplusplus
}
#endif

#endif // _WIRING_DIGITAL_