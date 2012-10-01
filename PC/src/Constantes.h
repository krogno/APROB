/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**Fichier dans lequel définir l'ensemble des constantes
Ainsi, lors du versionnement du code, seul ce fichier est modifié lorsque l'on paufine les paramètres, ce qui limite les risques de modifications simultanées
d'un même fichier
*/

#ifndef CONSTANTES_H
#define CONSTANTES_H

#include <cmath>
/*
PARAMETRES PORT SERIE ASSERVISSEMENT ROBOT UNICYCLE
*/
#define ROBOT_UNICYCLE_PORT_SERIE                   "/dev/ttyACM0"
#define ROBOT_UNICYCLE_PORT_SERIE_FREQUENCE         115200

/*
PARAMETRES GÉOMETRIQUES
*/
///Rayon des roues codeuses.
#define ROBOT_UNICYCLE_RAYON_ROUE                   30.0
///Distance entre les roues codeuses
#define ROBOT_UNICYCLE_DISTANCE_ROUES               100.0
#define ROBOT_UNICYCLE_NOMBRE_CRANS_PAR_TOUR        512

/*
PARAMETRES CINÉMATIQUES ET DYNAMIQUE
*/
///Vitesse maximum de translation (en unité de longueur par seconde)
#define ROBOT_UNICYCLE_VITESSE_MAX       100.0
///Vitesse maximum de rotation (en rad/s)
#define ROBOT_UNICYCLE_OMEGA_MAX                    2*M_PI*0.5

#define ROBOT_UNICYCLE_ACCELERATION_MAX             10.0

/*
PARAMÈTRES DE PRÉCISION
*/
///précision utilisée pour arrêter un ordre en distance.
#define ROBOT_UNICYCLE_PRECISION_DISTANCE           30.0
///précision utilisée pour arrêter un ordre en rotation.
#define ROBOT_UNICYCLE_PRECISION_ANGLE              M_PI/180.0*2.0
///Vitesse maximale auquelle peut avancer le robot pour être considérer comme arrêté (en unité de longueur/s)
#define ROBOT_UNICYCLE_EPSILON_VITESSE              5.0
///Vitesse maximale auquelle peut tourner le robot pour être considérer comme arrêté (en rad/s)
#define ROBOT_UNICYCLE_EPSILON_OMEGA                M_PI/180*2
///Distance de la cible a partir de le robot doit diminuer sa consigne de vitesse
#define ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT      150.0
/**Distance a la cible a partir de laquelle on n'a plus a respecter le mode de deplacement choisi
    e.g. si seule la marche avant est autorisee mais qu'on a depasse la cible de moins de ROBOT_UNICYCLE_DISTANCE_AJUSTEMENT, on tolere une marche arriere.
**/
#define ROBOT_UNICYCLE_DISTANCE_AJUSTEMENT          40.0

/*
PARAMÈTRES D'ASSERVISSEMENT
*/
///Période d'asservissement du côté PC (en s)
#define ROBOT_UNICYCLE_PERIODE_ASSERVISSEMENT       0.05

#define ROBOT_UNICYCLE_KP_ROTATION                  0.2*50.0
#define ROBOT_UNICYCLE_KI_ROTATION                  2.0*ROBOT_UNICYCLE_KP_ROTATION/0.1 //(Tu=0.1s)
#define ROBOT_UNICYCLE_KD_ROTATION                  ROBOT_UNICYCLE_KP_ROTATION*0.1/3
#define ROBOT_UNICYCLE_SEUIL_SATURATION_I_ROTATION  0.005

#define ROBOT_UNICYCLE_KP_TRANSLATION               1.0
#define ROBOT_UNICYCLE_KD_TRANSLATION               0.0


#define ROBOT_UNICYCLE_KP_VITESSE                  0.5 //0.2*2.0
#define ROBOT_UNICYCLE_KI_VITESSE                  2.0 //2*0.2*2.0/0.1
#define ROBOT_UNICYCLE_KD_VITESSE                  0.0 //0.2*2.0/3
#define ROBOT_UNICYCLE_SEUIL_SATURATION_I_VITESSE  255.0/ROBOT_UNICYCLE_KI_VITESSE

#endif
