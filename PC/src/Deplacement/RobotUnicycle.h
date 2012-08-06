#ifndef ROBOT_UNICYCLE_H
#define ROBOT_UNICYCLE_H

#include <SFML/System.hpp>
#include "2D/Mobile.h"

/**
Un RobotUnicycle est un robot théorique se déplaçant dans le plan avec deux roues parallèles.
On suppose ici que :
- les roues sont orientés dans la direction du Mobile.
- les roues sont placées symétriquement par rapport au point défini par l'Objet.

Note : cette classe est une classe abstraite, elle ne sert qu'à donner un squelette de classe sur lequel de "vrais" classes instanciables peuvent s'appuyer.
Les méthodes SetVitessesAngulairesRoues et Run doivent être redéfinies
*/
class RobotUnicycle :  public Mobile, public sf::Thread
{
public:
    RobotUnicycle(double rayonG, double rayonD, double dist, float periode);
    ~RobotUnicycle();
    virtual void SetVitessesAngulairesRoues(double vitesseGauche, double vitesseDroite)=0;

protected:
        ///Rayon de la roue gauche
        double rayonGauche;

        ///Rayon de la roue droite
        double rayonDroite;

        ///Distance entre les roues
        double distance;

        ///Période d'asservissement (en s)
        float periodeAsservissement;

        ///Fonction appelée par la méthode Launch() héritée permettant de lancer l'asservissement du déplacement du robot
        virtual void Run()=0;
        ///Mutex devant empêcher les modifications de consignes non souhaitées pendant une boucle d'asservissement
        sf::Mutex mutexMotificationConsignes;
        ///Booléen devant permettre d'arrêter la boucle d'asservissement lorsqu'il passe à faux.
        bool isThreadRunning;
};

#endif
