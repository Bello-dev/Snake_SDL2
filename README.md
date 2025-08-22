# Snake SDL2 - Enhanced Edition

Un jeu de serpent classique développé en C en utilisant la bibliothèque SDL2 sur Linux, avec des améliorations visuelles et de gameplay modernes.

## Nouvelles Fonctionnalités - Version Améliorée

### Améliorations Visuelles
- **Interface moderne** : Design sombre avec des couleurs attrayantes
- **Effets visuels avancés** :
  - Gradient de fond dynamique
  - Effets de lueur (glow) pour la tête du serpent et la nourriture
  - Système de particules lors de la consommation de nourriture
  - Animation pulsante de la nourriture
  - Yeux du serpent qui changent de direction
  - Effets de dégradé sur le corps du serpent
- **Interface utilisateur enrichie** :
  - Affichage de statistiques détaillées (score, longueur, nourriture mangée, temps)
  - Indicateur de vitesse en temps réel
  - Sélection de difficulté visuelle

### Améliorations de Gameplay
- **Trois niveaux de difficulté** :
  - Facile : Vitesse lente, score réduit
  - Normal : Équilibré
  - Difficile : Vitesse rapide, score élevé
- **Système de score amélioré** :
  - Bonus basé sur la longueur du serpent
  - Points multipliés selon la difficulté
- **Contrôles améliorés** :
  - Support WASD en plus des flèches
  - Prévention du retournement immédiat
- **États de jeu multiples** :
  - Menu principal avec sélection de difficulté
  - Pause avec overlay
  - Écran de fin avec statistiques

### Fonctionnalités Techniques
- **Architecture modulaire** : Code organisé en modules séparés
- **Gestion d'erreurs robuste** : Continue sans audio si non disponible
- **Optimisations de performance** : Rendu 60 FPS stable
- **Build system** : Makefile avec cibles multiples

## Description

Ce projet implémente le jeu classique Snake en utilisant le langage C et la bibliothèque SDL2 pour le rendu graphique et la gestion des événements. Cette version améliorée offre une expérience visuelle moderne et un gameplay enrichi.

## Fonctionnalités

### Gameplay
- Interface graphique fluide avec SDL2 et effets visuels modernes
- Contrôles au clavier (flèches directionnelles et WASD)
- Système de score avancé avec bonus de longueur
- Gestion des collisions précise
- Croissance progressive du serpent
- Trois niveaux de difficulté (Facile, Normal, Difficile)
- Système de pause
- Statistiques en temps réel (temps de jeu, vitesse, longueur)

### Effets Visuels
- Gradient de fond dynamique
- Système de particules pour les effets spéciaux
- Animations pulsantes et effets de lueur
- Interface utilisateur moderne
- Yeux du serpent directionnels
- Transitions fluides entre les états de jeu

## Prérequis

Avant de compiler et exécuter le jeu, assurez-vous d'avoir installé :

- GCC (compilateur C)
- SDL2 et ses bibliothèques de développement
- Make (optionnel, pour utiliser le Makefile)

### Installation des dépendances sur Ubuntu/Debian

```bash
sudo apt update
sudo apt install gcc libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
```

### Installation des dépendances sur Fedora

```bash
sudo dnf install gcc SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel
```

### Installation des dépendances sur Arch Linux

```bash
sudo pacman -S gcc sdl2 sdl2_image sdl2_ttf sdl2_mixer
```

## Compilation

### Avec Make (si un Makefile est présent)

```bash
make
```

### Compilation manuelle

```bash
gcc -o snake *.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

## Exécution

```bash
./snake
```

## Contrôles

- **Flèches directionnelles** ou **WASD** : Déplacer le serpent
- **1, 2, 3** : Sélectionner la difficulté (dans le menu)
- **Espace** : Démarrer le jeu / Pause / Reprendre / Retour au menu
- **R** : Redémarrer la partie (écran de fin)
- **Échap** : Quitter le jeu

## Niveaux de Difficulté

- **Facile (1)** : Vitesse lente, 5 points par nourriture
- **Normal (2)** : Vitesse modérée, 10 points par nourriture  
- **Difficile (3)** : Vitesse rapide, 20 points par nourriture

*Bonus de longueur : +2 points par segment au-delà de la longueur initiale*

## Structure du projet

```
Snake_SDL2/
├── src/            # Code source
│   ├── game.h      # Définitions et structures principales
│   ├── game.c      # Logique principale du jeu
│   ├── main.c      # Point d'entrée et initialisation
│   ├── snake.c     # Logique du serpent
│   ├── food.c      # Système de nourriture
│   └── graphics.c  # Rendu et effets visuels
├── assets/         # Ressources (captures d'écran)
├── build/          # Fichiers de compilation (ignorés par git)
├── Makefile        # Script de compilation
├── .gitignore      # Fichiers à ignorer
└── README.md       # Ce fichier
```

## Règles du jeu

1. Utilisez les flèches directionnelles pour diriger le serpent
2. Mangez la nourriture pour grandir et gagner des points
3. Évitez de toucher les murs ou votre propre corps
4. Le jeu se termine si le serpent entre en collision

## Contribuer

Les contributions sont les bienvenues ! N'hésitez pas à :

1. Fork le projet
2. Créer une branche pour votre fonctionnalité (`git checkout -b feature/AmazingFeature`)
3. Commit vos changements (`git commit -m 'Add some AmazingFeature'`)
4. Push vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrir une Pull Request

## Licence

Ce projet est sous licence libre. Voir le fichier `LICENSE` pour plus de détails.

## Auteur

**Bello-dev**

## Remerciements

- La communauté SDL2 pour la documentation excellente
- Les contributeurs et testeurs du projet

---

*Développé avec ❤️ en C et SDL2*
