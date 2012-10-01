/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CIBLE_POSITION_H
#define CIBLE_POSITION_H

#include "Constantes.h"


///Différents modes que peut avoir à respecter le robot dans son asservissement en position
/*Note : ce sont des flags qui peuvent être combinés entre eux
par exemple (MARCHE_AVANT | RALENTIR_A_L_ARRIVEE) signifie que la marche avant est autorisée et qu'il faut ralentir à l'arrivée*/
#define MARCHE_AVANT                1<<0
#define MARCHE_ARRIERE              1<<1
#define RALENTIR_A_L_ARRIVEE        1<<3
#define MARCHE_AVANT_OU_ARRIERE     MARCHE_AVANT | MARCHE_ARRIERE

///Cible (point de passage) permettant de fixer un objectif a l asservissement en position
class CiblePosition : public CibleTrajectoire
{
private:
    ///Coordonnees de la cible
    double x;
    double y;
    ///Vitesse maximale toleree au passage de la cible
    double vitessePassage;
    ///Rayon au carre de la zone que represente la cible
    double precisionCarre;
    ///Mode de deplacement (MARCHE_AVANT, MARCHE_ARRIERE ou MARCHE_AVANT_OU_ARRIERE)
    int mode;
public:
    TypeCible getType(){return POSITION;};

    CiblePosition(double x, double y, int mode = MARCHE_AVANT, double vitessePassage=0, double precision=ROBOT_UNICYCLE_PRECISION_DISTANCE)
    {
        this->x=x;
        this->y=y;
        if(vitessePassage >=0)
            this->vitessePassage=vitessePassage;
        else this->vitessePassage=0;

        this->precisionCarre=precision*precision;
        this->mode=mode;
    }

    CiblePosition(const CiblePosition &cible)
    {
        this->x=cible.x;
        this->y=cible.y;
        this->vitessePassage=cible.vitessePassage;
        this->precisionCarre=cible.precisionCarre;
        this->mode=cible.mode;
    }

    double getX(){return x;};
    double getY(){return y;};
    double getVitessePassage(){return vitessePassage;};
    double getPrecisionCarre(){return precisionCarre;};
    double getPrecision(){return sqrt(precisionCarre);};
    int getMode(){return mode;};
    bool isMarcheArriereAutorisee(){return (mode & MARCHE_ARRIERE);};
    bool isMarcheAvantAutorisee(){return (mode & MARCHE_AVANT);};

    void setVitessePassage(double vitesse){vitessePassage=vitesse;};
};

#endif
