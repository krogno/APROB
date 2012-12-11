/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MobileAParticules.h"
#include <iostream>


void MobileAParticules::CreerParticules(int nombre, double sigma_avance, double sigma_theta)
{
    //Initialisation du generateur pseudo aleatoire
    generateur_uniforme.seed(3);

    //Creation des distributions de bruit
    distribution_avance=new std::tr1::normal_distribution<double>(0,sigma_avance);
    distribution_theta=new std::tr1::normal_distribution<double>(0,sigma_theta);

    //Creation de nombre particules, copies du mobile
    particules.clear();
    double ponderation=1.0/nombre;
    Mobile *mobile=this;
    std::pair<Mobile, double> exemplaireParticule(*mobile,ponderation);
    particules.assign(nombre, exemplaireParticule);
}

float MobileAParticules::Deplacer(double delta_avance, double delta_theta)
{
    double amplitude=std::max(delta_avance,-delta_avance)+std::max(delta_theta,-delta_theta);
    for(std::vector<std::pair<Mobile, double> >::iterator it=particules.begin(); it!=particules.end(); it++)
    {
        it->first.Deplacer(delta_avance+amplitude*(*distribution_avance)(generateur_uniforme), delta_theta+amplitude*(*distribution_theta)(generateur_uniforme));
    }
    return Mobile::Deplacer(delta_avance, delta_theta);
}

MobileAParticules::~MobileAParticules()
{
    if(distribution_avance!=NULL)
        delete distribution_avance;
    if(distribution_theta!=NULL)
        delete distribution_theta;
}

MobileAParticules::MobileAParticules()
{
    distribution_avance=NULL;
    distribution_theta=NULL;
}
