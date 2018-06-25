#ifndef UBCONFIG_H
#define UBCONFIG_H

#define STL_PORT    5760
#define NET_PORT    15760
#define PWR_PORT    35760
#define PXY_PORT    45760

#define MAV_DIR "mav_"
#define OBJ_DIR "objects"

#define PACKET_END      "\r\r\n\n"
#define BROADCAST_ID    255

#define SERIAL_PORT "ttyACM0"
#define BAUD_RATE   115200

#define POINT_ZONE      1           // Distance tolerance (meters)
#define TAKEOFF_ALT     5           // Target altitude (meters)

#define MISSION_TRACK_PERIOD  1.0   // Mission update period (seconds)

#endif // UBCONFIG_H
