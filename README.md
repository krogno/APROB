Contient les codes sources développés pour les robots de l'Atelier Piston ROBotique, le club de robotique des élèves de l'École Centrale de Lyon.
Pour plus d'informations, visitez notre site www.aprob.eclair.ec-lyon.fr.

Descriptif des répertoires
============================
### PC : contient le code utilisable sur le PC embarqué

PC
===========
### Langage
C++
projet permettant la compilation sous CodeBlocks.

### Bibliothèques utilisées :
SFML 1.6 (sfml-dev.org) : Simple and Fast Media Library
ArUco 1.2.4 : Augmented Reality library from the University of Cordoba (code source intégré au code PC)

Outils
===========
### Calibration d'une Webcam
Code d'exemple distribué avec OpenCV pour calibrer une webcam au moyen d'un damier noir et blanc.
1. Compiler le projet Codeblocks.
2. Lancer le programme avec les bonnes options en paramètres. Par exemple, pour mon damier de 9 par 6 intersections (soit 10 colonnes par 7 lignes), avec des cases de 0.025m de côté :
calibration -w 9 -h 6 -s 0.025 -o camera.yml -op
3. Je cite la page web du projet ArUco "You should take at least five different pictures. A frontal one, and four where the border of the pattern is near the image border. By placing the pattern near the image border it will be possible to estimate accurately the camera distorsion." (http://www.uco.es/investiga/grupos/ava/node/26 au 09/01/2012)


Comment compiler le code et exécuter le programme :
-----------------


### Sous Ubuntu (testé sur la version 12.04)

Installer les paquets codeblocks et libsfml-dev. Le projet devrait compiler correctement sous CodeBlocks.

### Sous Windows (testé sur Windows XP)

comme d'habitude, c'est galère, et encore SFML est une bibliothèque facile à installer.

- Télécharger depuis le site officiel (http://www.codeblocks.org/downloads/binaries) et installer CodeBlocks.
Bien faire attention à choisir la version avec le compilateur intégré (codeblocks-10.05mingw-setup.exe au 5 août 2012).
- Télécharger le SDK (Software Development Kit pour ceux que ça intéresse) de SFML depuis le site officiel (http://www.sfml-dev.org/download.php).
Bien choisir la version Windows - MinGW (MinGW est le nom du compilateur utilisé par CodeBlocks).
- Extraire le SDK (c'est un fichier .zip) quelque part, par exemple dans "C:/Program Files/CodeBlocks".
On devrait alors obtenir un répertoire du genre "C:/Program Files/CodeBlocks/SFML-1.6".
- Lancer CodeBlocks, aller dans le menu "Settings" -> "Compiler and Debugger...".
    - Dans la fenêtre qui vient de s'ouvrir, sélectionner "Global Compiler Settings".
    - Aller dans l'onglet "Search directories".
    - Aller dans le sous-onglet "Compiler".
    - Cliquer sur "Add" et sélectionner l'adresse du répertoire Include de SFML (par exemple "C:/Program Files/CodeBlocks/SFML-1.6/include")
    - Aller dans le sous-onglet "Linker".
    - Cliquer sur "Add" et sélectionner l'adresse du répertoire lib de SFML (par exemple "C:/Program Files/CodeBlocks/SFML-1.6/lib")
- Cliquer sur OK et essayer de compiler et de lancer le projet PC.cbp avec CodeBlocks.
- Si tout va bien, l'étape précédente n'a pas marché. Le code a compilé mais lors de l'exécution un message d'erreur est apparu spécifiant que des bibliothèques dynamiques (fichiers .dll) étaient introuvables.
- Aller dans le répertoire où vous avez installé SFML, dossier lib (par exemple "C:\Program Files\CodeBlocks\SFML-1.6\lib").
- Copier tous les fichiers dll du dossier dans le répertoire bin du dépôt Git (par exemple pour moi C:\Documents and Settings\Romain Brégier\Mes documents\GitHub\PC\bin).
- Il est possible que vous ayez toujours un message d'erreur indiquant que "libgcc_s_dw2-1.dll" est introuvable. Dans ce cas :
    - Aller dans "C:\Program Files\CodeBlocks\MinGW\bin" (ou autre) et copier "libgcc_s_dw2-1.dll" dans le même répertoire que précédemment ("C:\Documents and Settings\Romain Brégier\Mes documents\GitHub\PC\bin").
    - Si ce fichier .dll n'est pas présent, récupérez en une copie (par exemple sur http://www.dll-files.com/dllindex/dll-files.shtml?libgcc_s_dw2-1 en choisissant Download Zip-file) et copier la dans C:\Documents and "Settings\Romain Brégier\Mes documents\GitHub\PC\bin" et dans "C:\Program Files\CodeBlocks\MinGW\bin".
- Si ça ne fonctionne toujours pas, ouvrer la fenêtre, attraper votre ordinateur à deux mains et jeter le violemment.
- Si après ça, ça ne fonctionne toujours pas, au moins maintenant il y a une raison.
        
    
    
