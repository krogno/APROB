/**Fichier dans lequel définir l'ensemble des constantes
Ainsi, lors du versionnement du code, seul ce fichier est modifié lorsque l'on paufine les paramètres, ce qui limite les risques de modifications simultanées
d'un même fichier
*/

#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <cmath>

///Rayon des roues codeuses.
#define ROBOT_UNICYCLE_RAYON_GAUCHE                 3.0
#define ROBOT_UNICYCLE_RAYON_DROITE                 3.0
///Distance entre les roues codeuses
#define ROBOT_UNICYCLE_DISTANCE_ROUES               20.0

#define ROBOT_UNICYCLE_VITESSE_AVANCER_MAX          300.0


///précision utilisée pour arrêter un ordre en distance.
#define ROBOT_UNICYCLE_PRECISION_DISTANCE           10.0
///précision utilisée pour arrêter un ordre en rotation.
#define ROBOT_UNICYCLE_PRECISION_ANGLE              M_PI/180*2
///Période d'asservissement du côté PC
#define ROBOT_UNICYCLE_PERIODE_ASSERVISSEMENT       0.05


///Constantes utilisées pour accélérer les calculs
#define ROBOT_UNICYCLE_PRECISION_DISTANCE_CARRE    ROBOT_UNICYCLE_PRECISION_DISTANCE*ROBOT_UNICYCLE_PRECISION_DISTANCE
#endif
