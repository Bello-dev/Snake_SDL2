# 🐍 Snake SDL2 - Édition Révolutionnaire ✨

Un jeu de serpent **révolutionnaire** développé en C++ utilisant SDL2, transformé en une expérience gaming moderne avec des effets visuels spectaculaires, un système de pouvoirs magiques et une gameplay immersive qui redéfinit le classique Snake!

![Snake Game Screenshot](screenshot.png)

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
- 🟢 **Normale** : Croissance classique (+10 points)
- 🟡 **Speed Boost** : Vitesse temporaire x1.6 (5s)
- 🔵 **Double Score** : Multiplicateur x2 (8s)  
- 🟠 **Golden** : Bonus de score x3
- 🟣 **Shrink** : Réduit la taille (-2 segments)
- 💜 **Phase** : Traverse les murs (6s)
- 🔴 **MEGA** : Bonus massif x5 + screen shake!

### 🎯 Système de Progression Intelligent
- **Niveaux adaptatifs** avec objectifs croissants (5→7→9→11...)
- **Vitesse dynamique** basée sur le niveau et les pouvoirs
- **High scores persistants** avec sauvegarde automatique
- **Statistiques complètes** en fin de partie

### 🎨 Interface Moderne Ultra-Polish
- **Menu principal cinématique** : animations colorées et effets lumineux pulsés
- **HUD intelligent** : niveau actuel, progression, multiplicateurs de combo visibles
- **Indicateurs de status** : pouvoirs actifs affichés en temps réel
- **Game Over épique** : statistiques détaillées et célébration des records
- **Guide intégré** : légende des nourritures directement dans le menu
- **Écran de chargement "BELLO DEV"** conservé avec style

## 🚀 Installation Rapide

### Prérequis
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev

# Ou utilisez notre commande automatique
make install-deps
```

### Compilation & Lancement
```bash
# Clone le repository
git clone https://github.com/Bello-dev/Snake_SDL2.git
cd Snake_SDL2

# Compilation optimisée
make all

# Lancement du jeu
make run

# Version debug (optionnel)
make debug
```

## 🎮 Contrôles

| Touche | Action |
|--------|--------|
| **WASD** / **Flèches** | Déplacement du serpent |
| **1/2/3** | Sélection difficulté (menu) |
| **ESPACE** / **ENTRÉE** | Démarrer / Redémarrer |
| **P** / **ÉCHAP** | Pause / Menu |

## 🏆 Modes de Difficulté

| Mode | Vitesse | Multiplicateur | Description |
|------|---------|----------------|-------------|
| **🟢 Facile** | 250ms | x1.0 | Parfait pour débuter |
| **🟡 Normal** | 200ms | x1.2 | Équilibré et fun |
| **🔴 Difficile** | 150ms | x1.5 | Pour les experts ! |

## 🎨 Architecture Technique

### Structure du Projet
```
Snake_SDL2/
├── src/              # Code source modulaire
│   ├── main.cpp      # Point d'entrée 
│   ├── game.cpp      # Logic principale + systèmes
│   └── game.h        # Structures et déclarations
├── data/             # Ressources visuelles et audio
│   ├── images/       # Backgrounds et textures
│   ├── sons/         # Musiques et effets sonores
│   └── snake_revolutionary  # Exécutable
├── obj/              # Fichiers objets compilés
├── Makefile          # Système de build optimisé
└── README.md         # Cette documentation
```

### Technologies Utilisées
- **Langage** : C++11 avec POO moderne
- **Graphics** : SDL2 avec rendu hardware-acceleré
- **Audio** : SDL2_mixer pour les effets sonores
- **Fonts** : SDL2_ttf pour le texte stylisé
- **Build** : Makefile multi-plateforme
- **Memory Management** : RAII + std::vector (sécurité maximale)

### Fonctionnalités Clés
- **Gestion des pouvoirs** avec timers et effets
- **Système de particules** avancé par type d'action
- **Rendu amélioré** avec effets de glow et animations
- **Système de niveaux** avec progression intelligente
- **Persistence des données** pour les high scores

## 🎯 Stratégies Pro pour Dominer

### 🔥 Techniques Avancées
1. **Maîtrise des Combos** : Enchaînez 3+ nourritures rapidement pour x2, x3, x4...
2. **Gestion des Power-ups** : Stackez Speed + Double Score pour un combo dévastateur
3. **Phase Stratégique** : Utilisez Phase mode pour des positions impossibles
4. **MEGA Hunting** : Les MEGA foods (2% chance) valent 5x - chassez-les !

### 🏅 Optimisation du Score
- Visez les **Golden foods** en priorité (x3 multiplicateur)
- Maintenez votre **combo** actif (timeout 3s)
- **Double Score** + combo = multiplicateurs exponentiels
- Niveaux élevés = **vitesse rapide** mais **points massifs**

## 🌈 Roadmap Future

### 🚀 Prochaines Versions
- [ ] **Système de niveaux thématiques** (Forêt, Océan, Espace)
- [ ] **Mode multijoueur local** 2-4 joueurs
- [ ] **Boss fights** avec serpents géants
- [ ] **Power-ups permanents** à débloquer
- [ ] **Skins de serpent customisables**
- [ ] **Achievements system** complet
- [ ] **Leaderboards en ligne**

### 🎵 Améliorations Audio/Visuelles
- [ ] **Soundtrack dynamique** qui évolue avec le niveau
- [ ] **Effets de particules 3D**
- [ ] **Shaders personnalisés** pour plus de bling
- [ ] **Support VR** (pourquoi pas !)

## 🛠️ Développement

### Commandes de Build
```bash
make all          # Build standard
make run          # Build + run
make clean        # Nettoyer
make debug        # Build debug
make help         # Aide complète
```

### Structure de Classes
- **Game** : Gestionnaire principal, rendering, logique
- **Snake** : Gestion du serpent avec std::vector sécurisé  
- **Food** : Système de nourritures magiques
- **PowerUps** : Gestion des effets temporaires
- **Particle** : Système d'effets visuels

## 🤝 Contribution

Envie d'ajouter votre magie au projet ? 

1. Fork le repository
2. Créez une branch feature (`git checkout -b feature/MaSuperIdee`)
3. Commit vos changements (`git commit -am 'Add: Ma super feature'`)
4. Push vers la branch (`git push origin feature/MaSuperIdee`)
5. Ouvrez une Pull Request

### 🐛 Signaler des Bugs
Utilisez les [GitHub Issues](https://github.com/Bello-dev/Snake_SDL2/issues) avec :
- OS et version SDL2
- Screenshot si applicable
- Steps pour reproduire

## 📜 Licence

Ce projet est sous licence MIT - voir le fichier [LICENSE](LICENSE) pour les détails.

## 👨‍💻 Auteur

**BELLO DEV** - *Architecte du Snake Révolutionnaire*
- GitHub: [@Bello-dev](https://github.com/Bello-dev)

## 🙏 Remerciements Spéciaux

- **SDL Development Team** pour cette incroyable librairie
- **La communauté Snake** pour l'inspiration continue  
- **Tous les beta-testers** qui ont rendu ce jeu possible
- **Vous** pour jouer et faire vivre ce projet ! 🎉

---

**🐍 Prêt à révolutionner votre expérience Snake ? Téléchargez maintenant et entrez dans la légende ! ⚡**

*"Ce n'est plus juste un jeu de serpent, c'est une œuvre d'art interactive !"* - Un joueur conquis

---