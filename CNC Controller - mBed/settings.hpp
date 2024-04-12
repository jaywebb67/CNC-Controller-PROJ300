#ifndef _SETTINGS_HPP_
#define _SETTINGS_HPP_


#include "mbed.h"
#include "flash.hpp"
#include "steppers.hpp"
#include "system.hpp"


// Define settings restore bitflags.
#define SETTINGS_RESTORE_ALL 0xFF // All bitflags
#define SETTINGS_RESTORE_DEFAULTS bit(0)
#define SETTINGS_RESTORE_PARAMETERS bit(1)
#define SETTINGS_RESTORE_STARTUP_LINES bit(2)
#define SETTINGS_RESTORE_BUILD_INFO bit(3)

// Define bit flag masks for the boolean settings in settings.flag.
#define BITFLAG_REPORT_INCHES      bit(0)
// #define BITFLAG_AUTO_START         (1U<<1) // Obsolete. Don't alter to keep back compatibility.
#define BITFLAG_INVERT_ST_ENABLE   bit(2)
#define BITFLAG_HARD_LIMIT_ENABLE  bit(3)
#define BITFLAG_HOMING_ENABLE      bit(4)
#define BITFLAG_SOFT_LIMIT_ENABLE  bit(5)
#define BITFLAG_INVERT_LIMIT_PINS  bit(6)
#define BITFLAG_INVERT_PROBE_PIN   bit(7)

// Define EEPROM address indexing for coordinate parameters
#define N_COORDINATE_SYSTEM 6  // Number of supported work coordinate systems (from index 1)
#define SETTING_INDEX_NCOORD N_COORDINATE_SYSTEM+1 // Total number of system stored (from index 0)
// NOTE: Work coordinate indices are (0=G54, 1=G55, ... , 6=G59)
#define SETTING_INDEX_G28    N_COORDINATE_SYSTEM    // Home position 1
#define SETTING_INDEX_G30    N_COORDINATE_SYSTEM+1  // Home position 2

#define AXIS_N_SETTINGS          4
#define AXIS_SETTINGS_START_VAL  100 // NOTE: Reserving settings values >= 100 for axis settings. Up to 255.
#define AXIS_SETTINGS_INCREMENT  10  // Must be greater than the number of axis settings


#define USER_DATA_FLASH_BASE 0x081C0000

#define FLASH_ADDR_PARAMETERS 0x081C0200
#define FLASH_ADDR_STARTUP_BLOCK  0x081C0300
#define FLASH_ADDR_BUILD_INFO     0x081C03AE


// Global persistent settings (Stored from byte Flash_ADDR_GLOBAL onwards)
typedef struct {
  // Axis settings
  float steps_per_mm[N_AXIS];
  float max_rate[N_AXIS];
  float acceleration[N_AXIS];
  float max_travel[N_AXIS];

  // Remaining Grbl settings
  uint8_t pulse_microseconds;
  uint8_t step_invert_mask;
  uint8_t dir_invert_mask;
  uint8_t stepper_idle_lock_time; // If max value 255, steppers do not disable.
  float junction_deviation;
  float arc_tolerance;
  
  uint8_t flags;  // Contains default boolean settings

  uint8_t homing_dir_mask;
  float homing_feed_rate;
  float homing_seek_rate;
  uint16_t homing_debounce_delay;
  float homing_pulloff;
} settings_t;

extern settings_t settings; 

// Initialize the configuration subsystem (load settings from Flash)
void settings_init();

// Helper function to clear and restore Flash defaults
void settings_restore(uint8_t restore_flag);

// A helper method to set new settings from command line
uint16_t settings_store_global_setting(uint16_t parameter, float value);

// Stores build info user-defined string
void settings_store_build_info(char *line);

// Reads build info user-defined string
uint8_t settings_read_build_info(char *line);

// Writes selected coordinate data to Flash
void settings_write_coord_data(uint8_t coord_select, float *coord_data);

// Reads selected coordinate data from Flash
uint8_t settings_read_coord_data(uint8_t coord_select, float *coord_data);

// Stores the protocol line variable as a startup line in EEPROM
void settings_store_startup_line(uint16_t n, char *line);

// Reads an EEPROM startup line to the protocol line variable
uint16_t settings_read_startup_line(uint16_t n, char *line);

// Returns the step pin mask according to internal axis numbering
uint16_t get_step_pin_mask(uint8_t i);

// Returns the direction pin mask according to internal axis numbering
uint8_t get_direction_pin_mask(uint8_t i);

// Returns the limit pin mask according to internal axis numbering
uint8_t get_limit_pin_mask(uint8_t i);



#endif