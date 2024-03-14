/*

 * Gcode.h
 *
 *  Created on: Dec 20, 2023
 *      Author: jaywe


#ifndef GCODE_H_
#define GCODE_H_

#define MAX_CHARACTER_PER_LINE 256

typedef enum {
	G0,
	G1,
	G2,
	G3,
	G38_2,
	G80,
	G81,
	G82,
	G83,
	G84,
	G85,
	G86,
	G87,
	G88,
	G89
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
	G30,
	G53,
	G92,
	G92_1,
	G92_2,
	G92_3
} nonModal_t;

typedef enum {
	M0,
	M1,
	M2,
	M30,
	M60
} stopping_t;

typedef enum {
	disable,
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

typedef struct{
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
	nonModal_t nonModal;
	stopping_t stopping;
	toolChange_t toolChange;
	spindleTurning_t spindleTurn;
	coolant_t coolant;
	SwitchOverride_t switchOverride;
} line_t;



void parse_gcode(char *line, line_t line_modals);

#endif  GCODE_H_
*/
