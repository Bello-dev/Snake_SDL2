# Snake SDL2 - Édition Ultime 🐍✨

Un jeu de serpent **révolutionnaire** développé en C utilisant SDL2, transformé en une expérience gaming moderne avec des effets visuels spectaculaires, un système de pouvoirs magiques et une gameplay immersive qui redéfinit le classique Snake!

## 🌍 **NOUVEAU : Support Multi-Plateforme**

**Snake SDL2** fonctionne maintenant sur **Windows, Linux ET Android** ! 🚀

### Installation Rapide par Plateforme

#### 🐧 Linux
```bash
./build_linux.sh && cd build && ./snake
```

#### 🪟 Windows  
```cmd
build_windows.bat && cd build\Release && snake.exe
```

#### 🤖 Android
```bash
./build_android.sh && adb install android/app/build/outputs/apk/debug/app-debug.apk
```

📖 **Guide complet** : [CROSS_PLATFORM.md](CROSS_PLATFORM.md)

## 🌟 Fonctionnalités Révolutionnaires

### 🎮 Gameplay Enrichie
- **7 types de nourriture magiques** avec des effets uniques et visuellement distinctifs
- **Système de pouvoirs avancé** : boost de vitesse, traversée des murs, score multiplicateur
- **Système de combos dynamique** : enchaînez les nourritures pour des multiplicateurs de score massifs
- **Progression de niveaux intelligent** avec objectifs évolutifs et vitesse adaptative
- **Contrôles ultra-fluides** : WASD + flèches directionnelles pour une précision maximale

### ✨ Effets Visuels Spectaculaires
- **Background vivant** : étoiles scintillantes avec animations procedurales et gradients dynamiques
- **Particules magiques** : effets spécifiques pour chaque type de nourriture et action
- **Système d'éclairage avancé** : glow, pulsations et effets de brillance partout
- **Screen shake cinématique** pour les moments épiques (level up, MEGA food)
- **Animations fluides** dans tous les menus avec transitions colorées
- **Serpent évolutif** : apparence visuelle qui change selon les pouvoirs actifs

### 🍎 Arsenal de Nourritures Magiques

| Type | Effet | Bonus Score | Effet Visuel |
|------|-------|-------------|--------------|
| 🔴 **Normale** | Croissance standard | x1 | Effet de base rose |
| 🔵 **Vitesse** | Boost vitesse 3s | x2 | Lightning bleu électrique |
| 🟡 **Double** | Score x2 pendant 5s | x1 | Cercles dorés jumeaux |
| 🟠 **Dorée** | Bonus énorme instantané | x5 | Étoile dorée étincelante |
| 🟣 **Rétrécissement** | -2 segments | x3 | Spirale violette mystique |
| 🟢 **Phase** | Traverse murs 4s | x3 | Anneaux verts phaseurs |
| 🌈 **MEGA** | Score colossal + effets | x10 | Énergies tourbillonnantes |

### 🎯 Système de Progression Intelligent
- **Niveaux adaptatifs** : vitesse et complexité augmentent progressivement
- **Objectifs évolutifs** : 5 → 8 → 11 → 14+ nourritures par niveau
- **Persistance des records** : high scores automatiquement sauvegardés
- **Statistiques complètes** : niveau atteint, nourritures spéciales, temps de survie

### 🎨 Interface Moderne Ultra-Polish
- **Menu principal cinématique** : animations colorées et effets lumineux pulsés
- **HUD intelligent** : niveau actuel, progression, multiplicateurs de combo visibles
- **Indicateurs de status** : pouvoirs actifs affichés en temps réel
- **Game Over épique** : statistiques détaillées et célébration des records
- **Guide intégré** : légende des nourritures directement dans le menu

## 🚀 Installation Rapide

### Dépendances Système
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install gcc libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

# Fedora
sudo dnf install gcc SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel

# Arch Linux  
sudo pacman -S gcc sdl2 sdl2_image sdl2_ttf sdl2_mixer
```

### Compilation et Lancement
```bash
# Cloner le repo
git clone https://github.com/Bello-dev/Snake_SDL2.git
cd Snake_SDL2

# Compiler
make

# Jouer !
./snake
```

## 🎮 Contrôles

### Mouvements
- **Flèches directionnelles** ou **WASD** : Déplacer le serpent
- **ESPACE** : Démarrer/Pause/Menu
- **ESC** : Quitter
- **R** : Redémarrer (sur Game Over)

### Sélection de Difficulté
- **1** : Facile (apprentissage en douceur)
- **2** : Normal (expérience équilibrée) 
- **3** : Difficile (défi ultime)

### Bonus Secret
- **P** : Explosion de particules festive ! 🎆

## 🏆 Modes de Difficulté

### 🟢 Facile - Mode Apprentissage
- Vitesse initiale lente pour apprendre
- Bonus de temps pour maîtriser les combos
- Idéal pour découvrir tous les types de nourriture

### 🟡 Normal - Mode Équilibré  
- Progression parfaitement calibrée
- Challenge croissant et récompenses proportionnelles
- L'expérience Snake ultime recommandée

### 🔴 Difficile - Mode Expert
- Vitesse élevée dès le départ
- Défis intenses avec récompenses maximales
- Pour les maîtres qui cherchent l'adrénaline pure

## 🎨 Architecture Technique

### Structure du Projet
```
Snake_SDL2/
├── src/              # Code source modulaire
│   ├── main.c        # Point d'entrée 
│   ├── game.c        # Logic principale + systèmes
│   ├── game.h        # Structures et déclarations
│   ├── snake.c       # Gestion du serpent
│   ├── food.c        # Système de nourritures magiques
│   └── graphics.c    # Rendu et effets visuels
├── assets/           # Ressources visuelles et audio
├── Makefile          # Système de build optimisé
└── README.md         # Cette documentation
```

### Technologies Utilisées
- **Langage** : C (C99 standard)
- **Graphics** : SDL2 avec rendu hardware-acceleré
- **Audio** : SDL2_mixer pour les effets sonores
- **Fonts** : SDL2_ttf pour le texte stylisé
- **Build** : Makefile multi-plateforme

## 🎯 Stratégies Pro pour Dominer

### 🔥 Maîtriser les Combos
1. **Timing parfait** : Mangez dans les 1.5s pour maintenir le combo
2. **Multiplicateurs** : Combo x2, x3, x4, x5 (maximum)
3. **Nourriture Double** : Active pendant un combo = scores astronomiques

### 🌟 Optimiser les Pouvoirs  
1. **Vitesse** : Utilisez pour échapper aux situations critiques
2. **Phase** : Traversez votre propre corps en urgence  
3. **Rétrécissement** : Réduisez les risques dans les espaces confinés

### 🏆 Techniques Avancées
1. **Spiral Control** : Mouvements en spirale pour maximiser l'espace
2. **Wall Riding** : Longez les bords pour un contrôle optimal
3. **Power Stacking** : Combinez plusieurs pouvoirs simultanément

## 🌈 Roadmap Future

### 🎵 Prochaines Améliorations
- [ ] **Bande sonore épique** avec musiques dynamiques
- [ ] **Mode multijoueur** local en écran partagé  
- [ ] **Achievements système** avec déblocages
- [ ] **Skins de serpents** personnalisables
- [ ] **Mode speedrun** avec classements
- [ ] **Éditeur de niveaux** communautaire

## 🤝 Contribution

Les contributions sont **chaleureusement accueillies** ! Ce projet est conçu pour évoluer avec la communauté.

### Comment Contribuer
1. **Fork** le projet
2. **Créez** votre branche feature (`git checkout -b feature/IncroyableFonctionnalite`)
3. **Committez** vos changements (`git commit -m 'Ajout fonctionnalité incroyable'`)
4. **Push** vers la branche (`git push origin feature/IncroyableFonctionnalite`)
5. **Ouvrez** une Pull Request

### Idées de Contributions
- Nouveaux types de nourriture avec mécaniques uniques
- Effets visuels encore plus spectaculaires
- Optimisations de performance 
- Portage vers d'autres plateformes
- Traductions multilingues
- Documentation et tutoriels

## 📜 Licence

Ce projet est sous **licence libre**. Voir le fichier `LICENSE` pour plus de détails.

## 👨‍💻 Auteur

**Bello-dev** - Développeur passionné qui transforme les classiques en expériences modernes

## 🙏 Remerciements Spéciaux

- **Communauté SDL2** pour l'excellente documentation
- **Testeurs et contributeurs** qui rendent ce projet toujours meilleur  
- **Joueurs rétro** qui gardent vivant l'esprit des classiques
- **Vous** qui découvrez cette version révolutionnaire de Snake !

---

*🎮 Développé avec ❤️, beaucoup de ☕ et une passion débordante pour le gaming en C et SDL2*

**Rejoignez l'aventure Snake SDL2 - Où le rétro rencontre le moderne ! 🚀✨**