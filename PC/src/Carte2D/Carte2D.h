#ifndef CARTE_2D_H
#define CARTE_2D_H

#include <string>
#include <SFML/Graphics.hpp>
#include <map>
#include "2D/Objet.h"

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

    ///Ajouter un objet sur la carte, représenté par le sprite passé en argument
    void AjouterObjet(Objet* objet, sf::Sprite* sprite);
    ///Ajouter un objet de la carte
    void RetirerObjet(Objet *objet);
    ///Mettre à jour l'affichage de la carte
    void MettreAJour();

private:
    ///Fenêtre de dessin
    sf::RenderWindow fenetre;
    ///Image du fond de carte
    sf::Image imageFond;
    ///Sprite à l'échelle et positionné du fond de carte
    sf::Sprite spriteFond;
    ///Vue de rendu
    sf::View vue;
    ///Container où sont stockés les pointeurs vers les objets à afficher ainsi que les sprites associés
    std::map<Objet*, sf::Sprite*> objets;
};

#endif
