#ifndef _LIMITS_HPP_
#define _LIMITS_HPP_


#include "mbed.h"
#include "Gcode.hpp"
#include <cstdint>
#include "settings.hpp"


#define xlimit1_PIN     PE_0
#define xlimit2_PIN     PE_2
#define y1limit1_PIN    PE_3
#define y1limit2_PIN    PE_4
#define zlimit1_PIN     PE_5
#define zlimit2_PIN     PE_6


#define LIMIT_PIN GPIOE->IDR
#define LIMIT_MASK ((1U<<0)|(1U<<2)|(1U<<3)|(1U<<4)|(1U<<5)|(1U<<6))

// Homing axis search distance multiplier. Computed by this value times the cycle travel.
#ifndef HOMING_AXIS_SEARCH_SCALAR
  #define HOMING_AXIS_SEARCH_SCALAR  1.5 // Must be > 1 to ensure limit switch will be engaged.
#endif
#ifndef HOMING_AXIS_LOCATE_SCALAR
  #define HOMING_AXIS_LOCATE_SCALAR  5.0 // Must be > 1 to ensure limit switch is cleared.
#endif



void limitsInit();
uint8_t softLimitCheck(lineStruct_block_t line_structure);
void limitsEnable(bool EN);
uint8_t limits_get_state();
void limitsHOME(uint8_t cycle_mask);

#endif