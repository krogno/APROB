#ifndef MOBILE_H
#define MOBILE_H

#include "Objet.h"

/**
Un Mobile est un objet qui a la capacité d'être avancé dans la direction de son orientation et d'être tourné en même temps
*/
class Mobile : public Objet
{
public:
    /**
    Déplacer le mobile : l'avancer de delta_avance en le tournant de delta_theta
    (Deplacer à n'utiliser que pour des petits déplacements, puisque cette définition n'est pas rigoureuse)
    */
    void Deplacer(double delta_avance, double delta_theta);

    protected:
    ///Estimation de la vitesse linéaire d'après les appels à Deplacer
    double vitesse;
    ///Estimation de la vitesse angulaire d'après les appels à Deplacer
    double omega;
    //Note : la téléportation (modification de x, y ou theta manuellement) est ignorée pour les calculs de vitesse
};


#endif
