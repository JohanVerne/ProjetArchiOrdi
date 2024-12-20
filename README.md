Faire rapport ici : comment l'exécuter, qu'est ce que j'ai cherché à faire ...

Exécution : make exec

|||||||||||||||| RAPPORT DE PROJET D'ARCHITECTURE DES ORDINATEURS ||||||||||||||||

Ce projet est une reproduction du jeu du dinosaure de Google sur le simulation mini-RISC Harvey en se basant sur le port de FREERTOS (lien pour jouer au jeu officiel https://chromedino.com/).

Le but du projet était de reproduire le comportement du jeu officiel en code C, en implémentant les différentes fonctionnalités du jeu original :

Le but du jeu est de parcourir la plus grande distance possible avec son dinosaure tout en esquivant les obstacles sur son chemin (cactus et ptérodactyls) en sautant au dessus d'eux ou en se penchant pour passer en dessous.

|||||||| COMMENT JOUER AU JEU ? ||||||||

Téléchargez le dépôt git sur votre machine, rendez-vous dans le dossier téléchargé et exécutez $make afin de compiler le projet ou $make exec afin de l'exécuter.

Contrôles :

            - Z ou flèche du haut pour sauter.

            - S ou flèche du bas pour se baisser.

            - Q ou Alt+F4 pour quitter le jeu en cours de partie.

|||||||| FONCTIONNALITES DU JEU ||||||||

Les fonctionnalités du jeu sont similaires à celles du jeu officiel :

    - Les sauts du dinosaures sont soumis à une gravité constante, le dino ne peut pas sauter alors qu'il est en l'air
    - Le dino peut augmenter sa propre gravité en se baissant en cours de saut, il peut également passer sous certains (pas tous) ptérodactyls en se baissant.
    - Les obstacles apparaissent de manière aléatoire et se déplacent à une vitesse aléatoire de la droite de l'écran vers la gauche
        - les cactus sont ancrés dans le sol
        - les ptérodactyls peuvent avoir une hauteur aléatoire et sont plus rapides que les cactus
    - Le dinosaure et les ptérodactyls ont des animations quand ils se déplacent, comme dans le jeu officiel
    - Un score est affiché en haut à gauche de l'écran et s'incrémente à chaque pas du dinosaure
    - Un système de collision détecte à chaque frame si il y a contact entre le dino et les divers obstacles
    - En cas de collision, la fenêtre de jeu se ferme, et le score est affiché dans le terminal
