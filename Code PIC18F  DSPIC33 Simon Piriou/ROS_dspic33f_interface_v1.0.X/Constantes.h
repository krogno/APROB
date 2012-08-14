
#define L 0
#define R 1

#define DIST_MIN 0.01 // Pour l'odometrie


#define MIN_GOAL_DIST 50	// distance pour freiner (en mm)
#define MIN_DIST_ERR 10		// précision en distance à l'arret (en mm)
#define OBST_THRESHOLD 500      // if far enough don't care of obstacles

#define _VERSION "Carte dsPic33f v2.0 SP 2012>"
#define FALSE 0
#define TRUE 1
#define ODO_MAX 0xFFFF
#define ODO_MAX_2 0x7FFF
#define VITESSE_MAX 700 // en mm/s

#define TMR2_VALUE 2000 // 20kHz
#define TMR4_VALUE 400 // 10us (100kHz)

#define STAND_BY 5

#define TWOPI 6.2831853072	// 360°
#define HALFPI 1.570796327	// 90°
#define QUARTPI 0.7853981634    // 45°

#define RAD2DEG 57.295779513	// = 180/PI
#define DEG2RAD 0.0174532925	// = PI/180
#define MIN_THETA_ERR DEG2RAD	// acceptable angle error in radians = 1°

