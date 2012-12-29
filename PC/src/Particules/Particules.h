/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MOBILE_A_PARTICULES
#define MOBILE_A_PARTICULES

#include "2D/Mobile.h"
#include <vector>
#include <tr1/random>
#include "Carte2D/Carte2D.h"


class Particules
{
    public:
    void Deplacer(double delta_avance, double delta_theta);
    /**
    Creer nombre particules a la position
    Note: pour un robot avec une distance L entre les roues sigma_theta=2/L sigma_avance
    **/
    void CreerParticules(int nombre, double x, double y, double theta, double sigma_x, double sigma_y, double sigma_theta);
    void SetBruitDeplacement(double sigma_avance, double sigma_theta);
    Particules();
    ~Particules();

    void AfficherSurCarte(Carte2D* carte, sf::Sprite *sprite);

    ///Couples <particule, ponderation associee>
    Mobile* particules;
    double* ponderations;
    int nombre;


    private :
    void SupprimerParticules();
    Carte2D *carte;
    sf::Sprite *sprite;

    ///generateurs aleatoires du bruit de deplacement des particules
    std::tr1::normal_distribution<double>* distribution_avance;
    std::tr1::normal_distribution<double>* distribution_theta;
    std::tr1::ranlux64_base_01 generateur_uniforme;
};


#endif
