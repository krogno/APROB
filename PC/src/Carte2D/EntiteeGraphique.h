#ifndef ENTITEE_GRAPHIQUE_H
#define ENTITEE_GRAPHIQUE_H

#include "../2D/Objet.h"
#include <SFML/Graphics.hpp>

/**
Une entitée graphique est un objet dérivé d'un point associé à un sprite permettant de le représenter
*/
class EntiteeGraphique
{
public:
    EntiteeGraphique(Objet* obj, sf::Sprite& sprit);
    Objet* objet;
    sf::Sprite sprite;
    void Afficher(sf::RenderWindow& fenetre);
};

#endif
