/*

 * Gcode.h
 *
 *  Created on: Dec 20, 2023
 *      Author: jaywe

*/
#ifndef GCODE_HPP_
#define GCODE_HPP_

#include "mbed.h"
#include "steppers.hpp"
#include <cctype>
#include <cstdint>
#include "spindle.hpp"
#define MAX_CHARACTER_PER_LINE 256

typedef enum {
    G0,
	G1,
	G2,
	G3,
	G38_2,
    NONE
	// G80,
	// G81,
	// G82,
	// G83,
	// G84,
	// G85,
	// G86,
	// G87,
	// G88,
	// G89
} motion_t;

typedef enum {
	G17,
	G18,
	G19
} planeSelection_t;

typedef enum {
	G90,
	G91
} distance_t;

typedef enum {
	G93,
	G94
} feedRate_t;

typedef enum {
	G20,
	G21
} units_t;

typedef enum {
	G40,
	G41,
	G42
} cutterRadiusComp_t;

typedef enum {
	G43,
	G49
} toolLengthOffSet_t;

typedef enum {
	disable,
	G98,
	G99
} returnModeCannedCycle_t;

typedef enum {
	G54,
	G55,
	G56,
	G57,
	G58,
	G59,
	G59_1,
	G59_2,
	G59_3
} coordinateSystemSelection_t;

typedef enum {
	G61,
	G61_1,
	G64
} pathControlMode_t;

typedef enum{
    
	G4,
	G10,
	G28,
    G28_1,
	G30,
    G30_1,
	G53,
	G92,
	G92_1
    
} nonModal_t;

typedef enum {
    resume,
	M0,
	M1,
	M2,
	M30,
	M60
} stopping_t;

typedef enum {
	disableTC,
	M6
} toolChange_t;

typedef enum {
	M3,
	M4,
	M5
} spindleTurning_t;

typedef enum {
	M7,
	M8,
	M9,
	M7M8,
} coolant_t;

typedef enum {
	M48,
	M49
} SwitchOverride_t;

typedef struct lineModals_t{
	motion_t motion;
	planeSelection_t plane;
	distance_t distance;
	feedRate_t feed;
	units_t units;
	cutterRadiusComp_t cutterRadius;
	toolLengthOffSet_t toolLength;
	returnModeCannedCycle_t returnCanned;
	coordinateSystemSelection_t coordSystem;
	pathControlMode_t pathControl;
	stopping_t stopping;
	toolChange_t toolChange;
	spindleTurning_t spindleTurn;
	coolant_t coolant;
	SwitchOverride_t switchOverride;

    lineModals_t():
    motion(G0),
    plane(G17),
    distance(G90),
    feed(G94),
    units(G21),
    cutterRadius(G40),
    returnCanned(disable),
    coordSystem(G54),
    pathControl(G61),
    stopping(resume),
    toolLength(G49),
    spindleTurn(M5),
    coolant(M9),
    switchOverride(M49) {}
} lineModals_t;

typedef struct line_values_t {
    float feed;      // Feed
    float ijk[3];    // I,J,K Axis arc offsets
    uint8_t l;       // G10 or canned cycles parameters
    //int32_t n;       // Line number
    float dwell;     // G10 or dwell parameters
    // float q;      // G82 peck drilling
    float radius;    // Arc radius
    float spindle;   // Spindle speed
    uint8_t toolNo;  // Tool selection
    float xyz[3];    // X,Y,Z Translational axes

    line_values_t() : 
    feed(0.0f), 
    ijk{0.0f, 0.0f, 0.0f}, 
    l(0), 
    //n(0), 
    dwell(0.0f), 
    radius(0.0f), 
    spindle(0.0f), 
    toolNo(0), 
    xyz{0.0f, 0.0f, 0.0f} {}
} line_values_t;

typedef struct lineStruct_block_t{

    uint16_t nonModalFlag;
    lineModals_t modals;
    line_values_t values;

    lineStruct_block_t():
    nonModalFlag(0),
    values(),
    modals(){}


} lineStruct_block_t;

extern lineStruct_block_t line_block;

typedef struct lineStruct_state_t{
    lineModals_t modal;
    
    float spindle_speed;          // RPM
    float feed_rate;              // Millimeters/min
    uint8_t tool;                 // Tracks tool number. NOT USED.
    //int32_t line_number;          // Last line number sent

    float position[N_AXIS];       // Where the interpreter considers the tool to be at this point in the code

    float coord_system[N_AXIS];   // Current work coordinate system (G54+). Stores offset from absolute machine
                                    // position in mm. Loaded from EEPROM when called.  
    float coord_offset[N_AXIS];   // Retains the G92 coordinate offset (work coordinates) relative to
                                    // machine zero in mm. Non-persistent. Cleared upon reset and boot.    
    float tool_length_offset;     // Tracks tool length offset value when enabled.

    lineStruct_state_t():
    modal(),
    spindle_speed(0.0f),
    feed_rate(0.0f),
    tool(0),
    position{0.0f,0.0f,0.0f},
    coord_system{0.0f,0.0f,0.0f},
    coord_offset{0.0f,0.0f,0.0f},
    tool_length_offset(0.0f) {}
        
} lineStruct_state_t;
extern lineStruct_state_t line_state;

void parse_gcode(char *line);
void executeLine(lineStruct_block_t line_structure);

#endif  //GCODE_HPP_

