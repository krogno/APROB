#include "Carte2D.h"
#include <iostream>
#include <cmath>

Carte2D::Carte2D(std::string nomFichierFond, double x0rigine, double yOrigine, double echelleX, double echelleY)
{
    //Fixation d'une valeur par défaut de la période de rafraichissement si la mise à jour automatique est active
    periodeRafraichissement=0.1;

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
    //fenetre.Clear();

    sf::Vector2f origineGlobale=spriteFond.TransformToGlobal(sf::Vector2f(0,0));
    //Création d'une vue permettant que le fond occupe tout l'écran malgré le fait qu'on aie chagé de coordonnées
    vue.SetFromRect(sf::FloatRect(origineGlobale.x, origineGlobale.y, spriteFond.GetSize().x+origineGlobale.x, spriteFond.GetSize().y+origineGlobale.y));
    fenetre.SetView(vue);

    MettreAJour();
}

Carte2D::~Carte2D()
{
    Stop();
}

void Carte2D::AjouterObjet(Objet* objet, sf::Sprite* sprite)
{
    sf::Lock lock(mutex);
    objets.insert(std::pair<Objet*, sf::Sprite*>(objet, sprite));
}

void Carte2D::RetirerObjet(Objet *objet)
{
    sf::Lock lock(mutex);
    objets.erase(objet);
}

void Carte2D::MettreAJour()
{
    static const double RadiansToDegres=180.0/M_PI;

    ///Verouille la modification de la liste d'objets
    sf::Lock lock(mutex);

    //Dessin du fond
    fenetre.Draw(spriteFond);

    //Dessin des objets
    for(std::map<Objet*, sf::Sprite*>::iterator it=objets.begin(); it!=objets.end() ; it++)
    {
        Objet* objet = it->first;
        sf::Sprite* sprite = it->second;
        sprite->SetPosition(objet->x, objet->y);
        sprite->SetRotation(objet->theta*RadiansToDegres);
        fenetre.Draw(*sprite);
    }
    //Affichage
    fenetre.Display();
}

bool Carte2D::SetPeriodeRafraichissement(float p)
{
    if(p>0)
    {
        periodeRafraichissement = p;
        return true;
    }
    return false;
}


void Carte2D::Run()
{
    rafraichissementAutomatique = true;
    while(rafraichissementAutomatique)
    {
        MettreAJour();
        sf::Sleep(periodeRafraichissement);
    }
}

void Carte2D::Stop()
{
    rafraichissementAutomatique = false;
    Wait();
}
