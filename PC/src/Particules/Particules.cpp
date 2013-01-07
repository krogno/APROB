/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Particules.h"
#include <iostream>
#include <ctime>
#include <algorithm>
#include <vector>

void Particules::CreerParticules(int nombre, double x, double y, double theta, double sigma_x, double sigma_y, double sigma_theta)
{
    //Initialisation du generateur pseudo aleatoire
    generateur_uniforme.seed(time(NULL));

    //Generateur de distribution normale
    std::tr1::normal_distribution<double> distribution_normale(0,1);

    //Suppression des eventuelles particules existantes
    SupprimerParticules();

    //Creation de nombre particules, copies du mobile, avec des ponderations uniformes suivant les dispersions donnees
    particules=new Mobile[nombre];
    ponderations=new double[nombre];
    this->nombre=nombre;

    double ponderation=1.0/nombre;
    for (int i=0; i<nombre ; i++)
    {
        particules[i].x=x+distribution_normale(generateur_uniforme)*sigma_x;
        particules[i].y=y+distribution_normale(generateur_uniforme)*sigma_y;
        particules[i].theta=theta+distribution_normale(generateur_uniforme)*sigma_theta;
        ponderations[i]=ponderation;
        if(carte)
            carte->AjouterObjet(&particules[i],sprite);
    }
}

void Particules::SetBruitDeplacement(double sigma_avance, double sigma_theta)
{
    //Creation des distributions de bruit
    distribution_avance=new std::tr1::normal_distribution<double>(0,sigma_avance);
    distribution_theta=new std::tr1::normal_distribution<double>(0,sigma_theta);

}

void Particules::AfficherSurCarte(Carte2D *lacarte, sf::Sprite *lesprite)
{
    carte=lacarte;
    sprite=lesprite;
    for(int i=0; i<nombre; i++)
    {
        carte->AjouterObjet(&particules[i],sprite);
    }

}

void Particules::Deplacer(double delta_avance, double delta_theta)
{
    double amplitude=std::max(delta_avance,-delta_avance)+std::max(delta_theta,-delta_theta);
    for(int i=0; i<nombre; i++)
    {
        particules[i].Deplacer(delta_avance+amplitude*(*distribution_avance)(generateur_uniforme), delta_theta+amplitude*(*distribution_theta)(generateur_uniforme));
    }
}

void Particules::SupprimerParticules()
{
    if(carte)
        for(int i=0; i<nombre; i++)
            carte->RetirerObjet(&particules[i]);
    if(particules)
        delete [] particules;
    if(ponderations)
        delete [] ponderations;
    nombre=0;
}

Particules::~Particules()
{
    if(distribution_avance!=NULL)
        delete distribution_avance;
    if(distribution_theta!=NULL)
        delete distribution_theta;
    SupprimerParticules();
}

Particules::Particules()
{
    //Creation des distributions de bruit sans bruit : deplacement ideal
    distribution_avance=new std::tr1::normal_distribution<double>(0,0);
    distribution_theta=new std::tr1::normal_distribution<double>(0,0);
    distribution_uniforme=new std::tr1::uniform_real<double>(0,1);
    particules=NULL;
    ponderations=NULL;
    carte=NULL;
    CreerParticules(1,0,0,0,0,0,0);
}


void Particules::Reechantillonner()
{
    //Realisation d un vecteur des ponderations cumulees des particules
    std::vector<double> ponderations_cumulees(nombre);
    ponderations_cumulees[0]=0;
    for(int i=0; i<nombre-1; i++)
    {
        ponderations_cumulees[i+1]=ponderations_cumulees[i]+ponderations[i];
    }
    //Somme des ponderations
    double normalisateur=ponderations_cumulees[nombre-1]+ponderations[nombre-1];


    //Tableau qui contiendra les particules reechantillonnees
    Mobile *nouvelles_particules=new Mobile[nombre];
    for(int i=0; i<nombre; i++)
    {
        //Selection aleatoire d une particule suivant la distribution de ponderation, avec remise
        double alea=(*distribution_uniforme)(generateur_uniforme)*normalisateur;
        //j est l indice de cette particule
        int j=(int)(lower_bound(ponderations_cumulees.begin(), ponderations_cumulees.end(),alea)-ponderations_cumulees.begin())-1;
        /*for(int k=0; k<nombre; k++)
            std::cout<<ponderations_cumulees[k]<<" ";
        std::cout<<std::endl;
        std::cout<<"alea "<<alea<<"\t"<<j<<std::endl;*/

        //Copie de cette particule dans le vecteur des nouvelles particules
        nouvelles_particules[i].x=particules[j].x;
        nouvelles_particules[i].y=particules[j].y;
        nouvelles_particules[i].theta=particules[j].theta;
    }

    //Mis a jour du tableau de particules et homogénisation des ponderations
    for(int i=0; i<nombre; i++)
    {
        if(carte)
        {
            carte->RetirerObjet(&particules[i]);
            carte->AjouterObjet(&nouvelles_particules[i],sprite);
        }
        ponderations[i]=1;
    }
    delete [] particules;
    particules=nouvelles_particules;
}
