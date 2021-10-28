/**
 * All the commands that a drone can make associated with their respective multiplier.
 * Useful to make calibrations more easily
 * 
 * @author Aldric Vitali Silvestre
 */

#ifndef __DRONECOMMANDS_H__
#define __DRONECOMMANDS_H__

namespace droneCommands
{
    typedef struct s_drone_com_data
    {
        long action;
        /**
     * We will receive values between -1 and 1, and each 
     * command may want a more "powerful" value
     */
        double multiplier;
    } DroneCommand;

    // "constexpr" means "evaluate at compile time" and is more a const than "const"
    // (which only means "read only")

    static constexpr DroneCommand ARM      = {0l, 1.0};
    static constexpr DroneCommand DISARM   = {1l, 1.0};
    static constexpr DroneCommand UP       = {2l, 1.0};
    static constexpr DroneCommand DOWN     = {3l, 1.0};
    static constexpr DroneCommand LEFT     = {4l, 1.0};
    static constexpr DroneCommand RIGHT    = {5l, 1.0};
    static constexpr DroneCommand POW      = {6l, 1.0};
}

#endif // __DRONECOMMANDS_H__