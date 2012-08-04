#include "Carte2D.h"
#include <iostream>

Carte2D::Carte2D(std::string nomFichierFond, double x0rigine, double yOrigine, double echelleX, double echelleY)
{
    //Chargement  du fond de carte
    imageFond.LoadFromFile(nomFichierFond);

    //Liage du sprite permettant d'afficher le fond de carte  à l'image
    spriteFond.SetImage(imageFond);

    //Choix du centre et mise à l'échelle du sprite pour correspondre au modèle
    spriteFond.SetCenter(x0rigine, yOrigine);
    spriteFond.SetScale(echelleX, echelleY);

    //Création de la fenêtre permettant d'afficher la carte : choix -> même dimensions que l'image
    fenetre.Create(sf::VideoMode(imageFond.GetWidth(), imageFond.GetHeight()), "carte 2D");

    //On nettoie la fenêtre en la remplissant de noir (inutile mais c'est pour montrer la syntaxe)
    fenetre.Clear();

    sf::Vector2f origineGlobale=spriteFond.TransformToGlobal(sf::Vector2f(0,0));
    //Création d'une vue permettant que le fond occupe tout l'écran malgré le fait qu'on aie chagé de coordonnées
    vue.SetFromRect(sf::FloatRect(origineGlobale.x, origineGlobale.y, spriteFond.GetSize().x+origineGlobale.x, spriteFond.GetSize().y+origineGlobale.y));
    fenetre.SetView(vue);

    MettreAJour();
}

void Carte2D::AjouterEntitee(EntiteeGraphique* entitee)
{
    entitees.push_back(entitee);
}

void Carte2D::MettreAJour()
{
    fenetre.Draw(spriteFond);

    for(std::list<EntiteeGraphique*>::iterator it=entitees.begin(); it!=entitees.end() ; it++)
        (*it)->Afficher(fenetre);

    fenetre.Display();
}
