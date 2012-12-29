/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ROBOT_A_PARTICULES_H
#define ROBOT_A_PARTICULES_H

#include "Deplacement/RobotUnicycle.h"
#include "Deplacement/RobotUnicycleVirtuel.h"
#include "Particules/Particules.h"



class RobotAParticulesVirtuel :  public RobotUnicycleVirtuel
{
    public:
    RobotAParticulesVirtuel(double retard, double facteur_erreur_vitesse_gauche, double facteur_erreur_vitesse_droite);

    void Deplacer(double delta_avance, double delta_theta);

    void AfficherParticulesSurCarte(Carte2D * carte, sf::Sprite * spriteParticule);
    void CreerParticules(int nombre, double x, double y, double theta, double sigma_x, double sigma_y, double sigma_theta);
    void SetBruitDeplacement(double sigma_avance, double sigma_theta);

    Particules particules;
};

#endif
