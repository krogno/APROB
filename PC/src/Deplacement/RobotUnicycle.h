/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ROBOT_UNICYCLE_H
#define ROBOT_UNICYCLE_H

#include <SFML/System.hpp>
#include "2D/Mobile.h"
#include "Constantes.h"
#include "CibleTrajectoire.h"
#include "CiblePosition.h"
#include "CibleOrientation.h"
#include <list>

/**
Un RobotUnicycle est un robot théorique se déplaçant dans le plan avec deux roues parallèles.
On suppose ici que :
- les roues sont orientés dans la direction du Mobile.
- les roues sont placées symétriquement par rapport au point défini par l'Objet.

Note : cette classe est une classe abstraite, elle ne sert qu'à donner un squelette de classe sur lequel de "vrais" classes instanciables peuvent s'appuyer.
Les méthodes SetVitessesAngulairesRoues, GetDeplacement, SetMoteursEnModeRouesLibres doivent être redéfinies.
*/
class RobotUnicycle :  public Mobile, public sf::Thread
{
public:
    RobotUnicycle();
    ~RobotUnicycle();
    void Avancer(double distance);
    void Tourner(double angle);
    void AjouterCibleOrientation(const CibleOrientation& cible);
    void AjouterCiblePosition(CiblePosition cible);

    void Stop();
    void Pause();
    void Reprendre();
    bool isArrete();

protected:
        std::list<CibleTrajectoire*> cibles;

        bool pause;

        double erreurVitesse;
        double primitiveErreurVitesse;

        double erreurOmega;
        double primitiveErreurOmega;

        void ReinitialiserErreurs();

//thread
        ///Fonction appelée par la méthode Launch() héritée permettant de lancer l'asservissement du déplacement du robot
        void Run();
        ///Mutex devant empêcher les modifications de consignes non souhaitées pendant une boucle d'asservissement
        sf::Mutex mutexMotificationConsignes;
        ///Booléen permettant d'arrêter la boucle d'asservissement lorsqu'il passe à faux, afin que le programme termine.
        bool isThreadRunning;

//Correcteurs
        inline double CorrectionVitesse(double delta_t);
        inline double CorrectionOmega(double delta_t);

        inline double CalculerConsigneVitesse(const double& erreurLineaire, const double& vitesse_passage,const double& delta_t);
        inline double CalculerConsigneOmega(const double& erreurAngulaire, const double &delta_t);

//Méthodes à surcharger
protected:
        ///Envoyer une consigne de vitesse aux moteurs, à surcharger
        virtual void SetVitesses(double vitesse_avance, double omega)=0;
        ///Recevoir le déplacement du robot depuis le dernier appel de cette fonction, à surcharger
        //Ici comme une fonction ne peut renvoyer qu'une valeur, les arguments de la fonction sont modifiés directement.
        virtual void GetDeplacement(double& delta_avance, double& delta_theta)=0;
        virtual void SetMoteursEnModeRouesLibres()=0;
};


#endif
