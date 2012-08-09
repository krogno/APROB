#ifndef ROBOT_UNICYCLE_H
#define ROBOT_UNICYCLE_H

#include <SFML/System.hpp>
#include "2D/Mobile.h"


///Consigne que peut suivre le robot
enum Consigne
{
  AVANCER, TOURNER, STOP, ROUE_LIBRE
};

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
    void Stopper();
    void PasserEnModeRouesLibres();
    bool isArrete();

protected:
        ///Consigne que le robot est en train d'exécuter
        Consigne consigne;
        double distanceRestanteAAvancer; //A un sens seulement si la consigne est AVANCER
        double angleRestantATourner; //A un sens seulement si la consigne est TOURNER

//thread
        ///Fonction appelée par la méthode Launch() héritée permettant de lancer l'asservissement du déplacement du robot
        void Run();
        ///Mutex devant empêcher les modifications de consignes non souhaitées pendant une boucle d'asservissement
        sf::Mutex mutexMotificationConsignes;
        ///Booléen permettant d'arrêter la boucle d'asservissement lorsqu'il passe à faux, afin que le programme termine.
        bool isThreadRunning;

//Méthodes à surcharger
protected:
        ///Envoyer une consigne de vitesse aux moteurs, à surcharger
        virtual void SetVitessesAngulairesRoues(double vitesseGauche, double vitesseDroite)=0;
        ///Recevoir le déplacement du robot depuis le dernier appel de cette fonction, à surcharger
        //Ici comme une fonction ne peut renvoyer qu'une valeur, les arguments de la fonction sont modifiés directement.
        virtual void GetDeplacement(double& delta_avance, double& delta_theta)=0;
        virtual void SetMoteursEnModeRouesLibres()=0;
};


#endif
