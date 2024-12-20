|||||||||||||||| RAPPORT DE PROJET D'ARCHITECTURE DES ORDINATEURS ||||||||||||||||

Ce projet est une reproduction du jeu du dinosaure de Google sur le simulation mini-RISC Harvey en se basant sur le port de FREERTOS (lien pour jouer au jeu officiel https://chromedino.com/).

Le but du projet était de reproduire le comportement du jeu officiel en code C, en implémentant les différentes fonctionnalités du jeu original.

Le but du jeu est de parcourir la plus grande distance possible avec son dinosaure tout en esquivant les obstacles sur son chemin (cactus et ptérodactyls) en sautant au dessus d'eux ou en se penchant pour passer en dessous.

|||||||| COMMENT JOUER AU JEU ? ||||||||

Téléchargez le dépôt git sur votre machine, déplacez vous dans le dossier téléchargé et exécutez $make afin de compiler le projet ou $make exec afin de l'exécuter.

Contrôles :

            - Z ou flèche du haut pour sauter.

            - S ou flèche du bas pour se baisser.

            - Q ou Alt+F4 pour quitter le jeu en cours de partie.

|||||||| FONCTIONNALITES DU JEU ||||||||

Les fonctionnalités du jeu sont similaires à celles du jeu officiel :

    - Les sauts du dinosaures sont soumis à une gravité constante, le dino ne peut pas sauter alors qu'il est en l'air
    - Le dino peut augmenter sa propre gravité en se baissant en cours de saut, il peut également passer sous certains ptérodactyls (pas tous) en se baissant.
    - Les obstacles apparaissent de manière aléatoire et se déplacent à une vitesse aléatoire de la droite de l'écran vers la gauche
        - les cactus sont ancrés dans le sol
        - les ptérodactyls peuvent avoir une hauteur aléatoire et sont plus rapides que les cactus
    - Le dinosaure et les ptérodactyls ont des animations quand ils se déplacent, comme dans le jeu officiel
    - Un score est affiché en haut à gauche de l'écran et s'incrémente à chaque pas du dinosaure
    - Un système de collision détecte à chaque frame si il y a contact entre le dino et les divers obstacles
    - En cas de collision, la fenêtre de jeu se ferme, et le score est affiché dans le terminal

|||||||| EXPLICATION DU PROJET ||||||||

    - Les différents sprites des objets ont été créées par le script ruby fourni gensprite.rb à partir de captures d'écrans de sprites présents sur les fichiers spritesDino.png et spritesObstacles.png trouvés sur https://www.spriters-resource.com.
    - Le controleur video du mini-RISC affiche une fenêtre de jeu et l'image est rafraichie selon la fréquence de rafraichissement de l'écran de l'utilisateur.
    - L'appui de touches sur le clavier génère des interruptions qui gèrent des évènements selon les touches pressées
        - Les toucehs Z et haut ne réagissent que losque elles sont pressées
        - Les touches S et bas réagissent lorsqu'elles sont préssées et relachées pour que le dino ne se baisse que lorsque ces touches sont à l'état bas.
    - A chaque nouvelle frame, le jeu met à jour les nouvelles positions des objets à l'écran, leur sprites et détecte les collisions.
    - L'affichage des sprites est effectué par la fonction draw_sprite() fournie
    - L'affichage du texte de score en jeu se fait avec la fonction font_16x32_draw_text() fournie

|||||||| AMELIORATIONS POTENTIELLES |||||||||

    - La gestion des obstacles est très basique et ne ressemble pas en tout point à celle du jeu original. Il n'y a aucun contrôle de générations d'obstacles qui seraient impossibles à passer, ce qui peut créer des situations où le joueur est obligé de perdre.
    - La gestion des collisions est elle aussi très basique, elle regarde simplement si les sprites se superposent ou non. Comme les sprites sont simplement des captures d'écran rectangulaires à la louche, il arrive souvent qu'il y ai des collisions détectées, alors que visuellement les deux objets ne se touchent pas. Cela pourrait être amélioré en testant les collisions seulement sur les pixels noirs des sprites plutôt que sur l'intégralité des sprites
    - Les dimensions des objets n'ont pas été modifiées par rapport aux captures d'écrans effectuées, ce qui modifie l'équilibre du jeu original. Entre autre, le sprite du dino penché est très grand, alors que celle des cactus est petite.
    - L'arrière-plan peut être modifié pour ressembler au jeu officiel, en étant mobile pour simuler le déplacement du dino.
    - Le controleur audio du mini-RISC aurait pu être utilisé pour générer les sons du jeu officiel.

Bon jeu.
