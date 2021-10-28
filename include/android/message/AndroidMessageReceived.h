#ifndef __ANDROIDRECEIVEDMESSAGE_H__
#define __ANDROIDRECEIVEDMESSAGE_H__
/**
 * A simple container for android messages recevied.
 * 
 * @author Aldric Vitali Silvestre
 */

typedef struct s_android_received_messsage
{
    double leftMove;

    double leftRotation;

    double forwardMove;
    
    double motorPower;

} AndroidMessageReceived;

#endif // __ANDROIDRECEIVEDMESSAGE_H__