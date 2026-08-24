# Snake SDL2

Un jeu de serpent classique développé en C en utilisant la bibliothèque SDL2 sur Linux.

## Description

Ce projet implémente le jeu classique Snake en utilisant le langage C et la bibliothèque SDL2 pour le rendu graphique et la gestion des événements. Le jeu est optimisé pour fonctionner sur les systèmes Linux.

## Fonctionnalités

- Interface graphique fluide avec SDL2
- Contrôles au clavier (flèches directionnelles)
- Système de score
- Gestion des collisions
- Croissance progressive du serpent

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

- **Flèches directionnelles** : Déplacer le serpent
- **Échap** : Quitter le jeu
- **Espace** : Pause/Reprendre (si implémenté)

## Structure du projet

```
Snake_SDL2/
├── src/           # Code source
├── assets/        # Ressources (images, sons, polices)
├── Makefile       # Script de compilation
└── README.md      # Ce fichier
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
