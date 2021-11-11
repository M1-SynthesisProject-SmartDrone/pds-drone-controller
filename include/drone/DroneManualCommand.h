#ifndef __DRONEMANUALCOMMAND_H__
#define __DRONEMANUALCOMMAND_H__

/**
 * Struct containing all data for joystick-like controls, as defined here :
 *  https://mavlink.io/en/services/manual_control.html#directional-control
 * 
 * We work with directional controls.
 * Each values are normalized between -1000 and 1000
 * 
 * @author Aldric Vitali Silvestre
 */

typedef struct drone_man_command_s {
    /**
	 * Moves forward / backward.
	 * 
	 * - > 0 : forward
	 * - < 0 : backward
	 */
    short x;

    /**
	 * Moves left / right.
	 * 
	 * - > 0 : right
	 * - < 0 : left
	 */
    short y;
    
    /**
	 * Moves up / down.
	 * 
	 * - > 0 : up
	 * - < 0 : down
	 */
    short z;

    /**
	 * Yaw, turn arround
	 * 
	 * - > 0 : counter-clockwise
	 * - < 0 : clockwise
	 */
    short r;

	// ! Doesn't exist for px4 ?
    // /**
	//  * Pitch : flips
	//  * 
	//  * - > 0 : nose-down
	//  * - < 0 : nose-up
	//  */
    // short s;

    // /**
	//  * Pitch : barrel rolls
	//  * 
	//  * - > 0 : right-down
	//  * - < 0 : left-down
	//  */
    // short t;
} DroneManualCommand;

#endif // __DRONEMANUALCOMMAND_H__