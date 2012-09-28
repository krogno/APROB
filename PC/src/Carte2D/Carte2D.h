#ifndef CARTE_2D_H
#define CARTE_2D_H

#include <string>
#include <SFML/Graphics.hpp>
#include <map>
#include "2D/Objet.h"

/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
