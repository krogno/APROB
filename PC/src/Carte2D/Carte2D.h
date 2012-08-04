#ifndef CARTE_2D_H
#define CARTE_2D_H

#include <string>
#include <SFML/Graphics.hpp>
#include "EntiteeGraphique.h"
#include <list>

/**
Classe permettant une représentations graphique d'objets sur le terrain sous forme de carte
*/
class Carte2D
{
public:
    /**
    Création d'une carte
    nomFichierFond : chemin de l'image contenant le fond de carte
    xOrigine, yOrigine : coordonnées en pixel de l'origine du repère
    echelleX : rapport entre la dimension à l'écran (en pixel) et la dimension réelle
    */
    Carte2D(std::string nomFichierFond, double x0rigine, double yOrigine, double echelleX, double echelleY);
    ///Mettre à jour l'affichage de la carte
    void MettreAJour();

    void AjouterEntitee(EntiteeGraphique* entitee);

    std::list<EntiteeGraphique*> entitees;
private:
    sf::RenderWindow fenetre;
    sf::Image imageFond;
    sf::Sprite spriteFond;
    sf::View vue;
};

#endif
