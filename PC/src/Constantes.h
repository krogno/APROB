/**Fichier dans lequel définir l'ensemble des constantes
Ainsi, lors du versionnement du code, seul ce fichier est modifié lorsque l'on paufine les paramètres, ce qui limite les risques de modifications simultanées
d'un même fichier
*/

#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <cmath>
/*
PARAMETRES GÉOMETRIQUES
*/
///Rayon des roues codeuses.
#define ROBOT_UNICYCLE_RAYON_GAUCHE                 3.0
#define ROBOT_UNICYCLE_RAYON_DROITE                 6.0
///Distance entre les roues codeuses
#define ROBOT_UNICYCLE_DISTANCE_ROUES               20.0

/*
PARAMETRES CINÉMATIQUES
*/
///Vitesse maximum de translation (en unité de longueur par seconde)
#define ROBOT_UNICYCLE_VITESSE_MAX       500.0
///Vitesse maximum de rotation (en rad/s)
#define ROBOT_UNICYCLE_OMEGA_MAX                    2*M_PI*0.5

/*
PARAMÈTRES DE PRÉCISION
*/
///précision utilisée pour arrêter un ordre en distance.
#define ROBOT_UNICYCLE_PRECISION_DISTANCE           10.0
///précision utilisée pour arrêter un ordre en rotation.
#define ROBOT_UNICYCLE_PRECISION_ANGLE              M_PI/180*1
///Vitesse maximale auquelle peut avancer le robot pour être considérer comme arrêté (en unité de longueur/s)
#define ROBOT_UNICYCLE_EPSILON_VITESSE              5.0
///Vitesse maximale auquelle peut tourner le robot pour être considérer comme arrêté (en rad/s)
#define ROBOT_UNICYCLE_EPSILON_OMEGA                M_PI/180*2
/*
PARAMÈTRES D'ASSERVISSEMENT
*/
///Période d'asservissement du côté PC (en s)
#define ROBOT_UNICYCLE_PERIODE_ASSERVISSEMENT       0.02
#define ROBOT_UNICYCLE_KP_ROTATION                  50.0
#define ROBOT_UNICYCLE_KD_ROTATION                  10.0

#define ROBOT_UNICYCLE_KP_TRANSLATION               50.0
#define ROBOT_UNICYCLE_KD_TRANSLATION               10.0

#endif
