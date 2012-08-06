#ifndef CARTE_2D_H
#define CARTE_2D_H

#include <string>
#include <SFML/Graphics.hpp>
#include <map>
#include "2D/Objet.h"

/**
Classe permettant une représentations graphique d'objets sur le terrain sous forme de carte
*/
class Carte2D : public sf::Thread
{
public:
    /**
    Créer une carte
    nomFichierFond : chemin de l'image contenant le fond de carte
    xOrigine, yOrigine : coordonnées en pixel de l'origine du repère
    echelleX, echelleY : rapport entre la dimension à l'écran (en pixel) et la dimension réelle
    */
    Carte2D(std::string nomFichierFond, double x0rigine, double yOrigine, double echelleX, double echelleY);
    ///Ajouter un objet sur la carte, représenté par le sprite passé en argument
    void AjouterObjet(Objet* objet, sf::Sprite* sprite);
    ///Ajouter un objet de la carte
    void RetirerObjet(Objet *objet);

    ///Mettre à jour l'affichage de la carte
    void MettreAJour();
    ///Fixer la période de rafraichissement automatique de la fenêtre, si activé (en s)
    bool SetPeriodeRafraichissement(float p);
    ///Lancer le rafraichissement automatique de la carte (méthode héritée)
    //Launch()
    ///Arreter le rafraichissement automatique
    void Stop();
    ///Destructeur
    ~Carte2D();
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
    ///Période de rafraichissement automatique de la fenêtre, si activé (en s)
    float periodeRafraichissement;
    ///Methode "thread" : est éxécutée lors d'un appel à Launch
    void Run();
    ///Variable testée dans Run afin de pouvoir arrêter le thread
    bool rafraichissementAutomatique;
    /**
    Permet d'empêcher de modifier la liste des objets pendant la mise à jour de l'affichage
    (ce n'est pas totalement sécurisé car un objet listé peut être modifié ou détruit pendant l'affichage)
    */
    sf::Mutex mutex;
};

#endif
