# ft_irc - IRC Server Implementation

*This project has been created as part of the 42 curriculum by ezeppa and vbonnard.*

---

## 📋 Description

This project is an implementation of an IRC (Internet Relay Chat) server compliant with the IRC protocol (RFC 1459) written in C++98. The server allows multiple IRC clients to connect, exchange messages, create and join channels, and manage user and channel modes.

**Goal**: Learn network programming with TCP/IP sockets, I/O multiplexing with epoll/poll, and the IRC protocol implementation.

## 🎯 Conformité au Sujet

Ce projet répond aux exigences de l'école 42 pour le projet **ft_irc**:

### ✅ Fonctionnalités Obligatoires Implémentées

#### Authentification
- ✅ **PASS**: Authentification par mot de passe
- ✅ **NICK**: Définition/changement de pseudo
- ✅ **USER**: Enregistrement des informations utilisateur

#### Gestion des Canaux
- ✅ **JOIN**: Rejoindre un canal
- ✅ **PART**: Quitter un canal
- ✅ **TOPIC**: Consulter/définir le sujet d'un canal
- ✅ **NAMES**: Lister les utilisateurs d'un canal

#### Modes de Canal
- ✅ **MODE +i**: Canal en mode "invite-only" (sur invitation uniquement)
- ✅ **MODE +t**: Restriction de modification du topic aux opérateurs
- ✅ **MODE +k**: Définir une clé (mot de passe) pour le canal
- ✅ **MODE +l**: Limiter le nombre d'utilisateurs dans le canal
- ✅ **MODE +o**: Donner/retirer les privilèges d'opérateur

#### Commandes Opérateur
- ✅ **KICK**: Expulser un utilisateur d'un canal
- ✅ **INVITE**: Inviter un utilisateur dans un canal en mode +i

#### Messagerie
- ✅ **PRIVMSG**: Envoyer des messages privés ou à un canal

#### Utilitaires
- ✅ **PING/PONG**: Maintien de la connexion
- ✅ **QUIT**: Déconnexion propre
- ✅ **CAP**: Négociation de capacités (compatibilité clients modernes)
- ✅ **WHO**: Liste d'utilisateurs

### 🏗️ Architecture Technique

#### Choix Techniques Conformes
- **C++98**: Strict respect du standard (compilation avec `-std=c++98`)
- **I/O Non-bloquant**: Utilisation d'`epoll` (Linux natif)
- **Parsing Robuste**: Intégration de BNFParser pour valider le protocole IRC
- **Gestion d'Erreurs**: Codes de réponse IRC standards (numerics RFC 1459)
- **Flags de Compilation**: `-Wall -Wextra -Werror`

#### Structure du Projet
```
ft_irc/
├── include/              # Fichiers d'en-tête
│   ├── Server.hpp        # Serveur principal
│   ├── Client.hpp        # Gestion des clients
│   ├── Channel.hpp       # Gestion des canaux
│   ├── CommandManager.hpp# Gestionnaire de commandes
│   ├── Logger.hpp        # Système de journalisation
│   └── network/
│       └── IrcReplies.hpp# Codes de réponse IRC
├── src/                  # Implémentation
│   ├── main.cpp          # Point d'entrée
│   ├── Server.cpp        # Logique serveur
│   ├── Client.cpp        # Logique client
│   ├── Channel.cpp       # Logique canal
│   ├── Logger.cpp        # Logs
│   └── commands/         # Implémentation des commandes
│       ├── cmd_auth.cpp      # PASS, NICK, USER, CAP, QUIT
│       ├── cmd_channel.cpp   # JOIN, PART, TOPIC, NAMES
│       ├── cmd_mode.cpp      # MODE (utilisateur et canal)
│       ├── cmd_ops.cpp       # KICK, INVITE
│       └── cmd_privmsg.cpp   # PRIVMSG
│   └── bnf/              # BNFParser (parseur IRC intégré)
├── include/bnf/          # Headers BNFParser
└── Makefile              # Compilation (all, clean, fclean, re)
```

## � Instructions

### Compilation

The project uses a standard Makefile with the following rules:

```bash
# Compile the project
make

# Clean object files
make clean

# Remove all generated files including the executable
make fclean

# Recompile everything
make re
```

**Requirements:**
- C++ compiler supporting C++98 standard (g++ or clang++)
- Make
- Linux (les sockets epoll sont natives)

The executable `ircserv` will be generated in the project root directory.

### Execution

Run the server with the following syntax:

```bash
./ircserv <port> <password>
```

**Parameters:**
- `<port>`: The port number for IRC connections (e.g., 6667)
- `<password>`: The server password required for client authentication

**Example:**
```bash
./ircserv 6667 mypassword123
```

## 📖 Utilisation

### Démarrage du Serveur

```bash
./ircserv <port> <password>
```

**Paramètres:**
- `<port>`: Port d'écoute (ex: 6667, 6697)
- `<password>`: Mot de passe de connexion au serveur

**Exemple:**
```bash
./ircserv 6667 motdepasse123
```

### Connexion avec un Client IRC

Le client de référence pour ce projet est **irssi** (utilisé lors de la correction 42).

#### Avec **irssi**
```bash
irssi
```
Puis dans irssi:
```irc
/connect 127.0.0.1 6667 motdepasse123
/nick VotrePseudo
/join #salon
```

Voir `MANUAL_TESTS.md` pour le guide complet de tests manuels avec irssi.

#### Avec **nc** (tests protocolaires bruts)
```bash
nc 127.0.0.1 6667
```
Puis taper:
```irc
PASS motdepasse123
NICK VotrePseudo
USER votrenom 0 * :Nom Complet
JOIN #salon
PRIVMSG #salon :Bonjour tout le monde!
```

> Le serveur est compatible avec tout client IRC conforme RFC 1459 (WeeChat, HexChat, etc.).

## 🎮 Console de Gestion du Serveur

Le serveur dispose d'une console interactive pour gérer et surveiller l'état en temps réel.

### Commandes Disponibles

| Commande | Description |
|----------|-------------|
| `help` | Affiche l'aide des commandes |
| `quit` | Arrête le serveur proprement |
| `clear` | Efface le terminal |
| `clients` | Liste tous les clients connectés |
| `channels` | Liste tous les canaux actifs |
| `client <fd\|nick>` | Affiche les détails d'un client |
| `log show` | Affiche l'état des filtres de logs |
| `log all` | Active tous les types de logs |
| `log none` | Désactive tous les logs |
| `log <type> <on\|off\|toggle>` | Active/désactive un type de log |

### Types de Logs
- **in**: Messages reçus des clients
- **out**: Messages envoyés aux clients
- **info**: Informations générales
- **error**: Erreurs

**Exemples:**
```bash
log show              # Voir l'état actuel
log in off            # Désactiver les logs de réception
log error toggle      # Basculer les logs d'erreur
log all               # Tout activer
```

## 🔧 Commandes IRC Implémentées

### Authentification et Connexion

#### PASS
```irc
PASS <password>
```
Authentifie l'utilisateur avec le mot de passe du serveur.

#### NICK
```irc
NICK <nickname>
```
Définit ou change le pseudo. Restrictions:
- Doit commencer par une lettre
- Caractères autorisés: lettres, chiffres, `-`, `[`, `]`, `\`, ``, `^`, `{`, `}`
- Ne peut pas être déjà utilisé

#### USER
```irc
USER <username> <mode> <unused> :<realname>
```
Enregistre les informations utilisateur (nom, nom réel).

### Gestion des Canaux

#### JOIN
```irc
JOIN #<canal> [<clé>]
```
Rejoint un canal (le crée s'il n'existe pas). Le premier utilisateur devient opérateur.

#### PART
```irc
PART #<canal> :[<raison>]
```
Quitte un canal avec une raison optionnelle.

#### TOPIC
```irc
TOPIC #<canal> [:<nouveau sujet>]
```
- Sans argument: affiche le sujet actuel
- Avec argument: change le sujet (si autorisé)

#### NAMES
```irc
NAMES [#<canal>]
```
Liste les utilisateurs d'un canal (@ = opérateur).

### Modes

#### MODE (Utilisateur)
```irc
MODE <nickname> <+|-><mode>
```
Modes disponibles:
- `+i`: Mode invisible

#### MODE (Canal)
```irc
MODE #<canal> <+|-><modes> [<paramètres>]
```

Modes disponibles:
- `+i`: Canal en mode invite-only
- `+t`: Seuls les opérateurs peuvent changer le topic
- `+k <clé>`: Définir un mot de passe pour le canal
- `+l <limite>`: Limiter le nombre d'utilisateurs
- `+o <nickname>`: Donner/retirer le statut d'opérateur

**Exemples:**
```irc
MODE #salon +i              # Active invite-only
MODE #salon +k secret123    # Définit une clé
MODE #salon +l 10           # Limite à 10 utilisateurs
MODE #salon +o Alice        # Fait d'Alice un opérateur
MODE #salon -i              # Désactive invite-only
MODE #salon +it             # Active invite-only ET topic protégé
```

### Commandes Opérateur

#### KICK
```irc
KICK #<canal> <nickname> :[<raison>]
```
Expulse un utilisateur du canal (opérateur uniquement).

#### INVITE
```irc
INVITE <nickname> #<canal>
```
Invite un utilisateur dans un canal en mode +i (opérateur uniquement).

### Messagerie

#### PRIVMSG
```irc
PRIVMSG <cible> :<message>
```
Envoie un message à un utilisateur ou un canal.

**Exemples:**
```irc
PRIVMSG #salon :Bonjour à tous!
PRIVMSG Alice :Message privé pour Alice
```

### Utilitaires

#### PING/PONG
```irc
PING :<serveur>
```
Teste la connectivité. Le serveur répond automatiquement avec PONG.

#### QUIT
```irc
QUIT :[<message>]
```
Déconnexion propre avec message optionnel.

#### WHO
```irc
WHO [<masque>]
```
Liste les utilisateurs correspondants.

## 📝 Codes de Réponse IRC

Le serveur implémente les codes de réponse numériques standards:

### Réponses de Succès
- `001 RPL_WELCOME`: Message de bienvenue
- `221 RPL_UMODEIS`: Mode utilisateur actuel
- `315 RPL_ENDOFWHO`: Fin de liste WHO
- `324 RPL_CHANNELMODEIS`: Modes du canal
- `331 RPL_NOTOPIC`: Aucun sujet défini
- `332 RPL_TOPIC`: Sujet du canal
- `341 RPL_INVITING`: Invitation envoyée
- `353 RPL_NAMREPLY`: Liste de pseudos
- `366 RPL_ENDOFNAMES`: Fin de liste NAMES

### Codes d'Erreur
- `401 ERR_NOSUCHNICK`: Pseudo inexistant
- `403 ERR_NOSUCHCHANNEL`: Canal inexistant
- `404 ERR_CANNOTSENDTOCHAN`: Envoi impossible
- `411 ERR_NORECIPIENT`: Destinataire manquant
- `412 ERR_NOTEXTTOSEND`: Texte manquant
- `421 ERR_UNKNOWNCOMMAND`: Commande inconnue
- `431 ERR_NONICKNAMEGIVEN`: Pseudo manquant
- `432 ERR_ERRONEUSNICKNAME`: Pseudo invalide
- `433 ERR_NICKNAMEINUSE`: Pseudo déjà utilisé
- `441 ERR_USERNOTINCHANNEL`: Utilisateur pas dans le canal
- `442 ERR_NOTONCHANNEL`: Vous n'êtes pas dans ce canal
- `443 ERR_USERONCHANNEL`: Utilisateur déjà dans le canal
- `451 ERR_NOTREGISTERED`: Non enregistré
- `461 ERR_NEEDMOREPARAMS`: Paramètres insuffisants
- `462 ERR_ALREADYREGISTRED`: Déjà enregistré
- `464 ERR_PASSWDMISMATCH`: Mot de passe incorrect
- `471 ERR_CHANNELISFULL`: Canal plein
- `473 ERR_INVITEONLYCHAN`: Canal sur invitation
- `475 ERR_BADCHANNELKEY`: Mauvaise clé de canal
- `482 ERR_CHANOPRIVSNEEDED`: Privilèges opérateur requis
- `501 RPL_UMODEUNKNOWNFLAG`: Mode inconnu
- `502 ERR_USERSDONTMATCH`: Ne peut modifier les modes d'autrui

## 🔍 Détails d'Implémentation

### Gestion des Connexions
- **Modèle I/O**: `epoll` (Linux natif) pour multiplexage
- **Non-blocking**: Toutes les opérations réseau sont non-bloquantes
- **Buffers**: Chaque client a des buffers de lecture/écriture indépendants

### Parsing des Commandes
- **BNF Parser**: Validation stricte du protocole IRC selon RFC
- **Extraction**: Séparation automatique commande/paramètres/trailing

### Gestion des Canaux
- **Création dynamique**: Les canaux sont créés au premier JOIN
- **Destruction automatique**: Un canal vide est supprimé automatiquement
- **Opérateurs**: Le créateur du canal devient automatiquement opérateur

### Sécurité
- **Validation**: Tous les pseudos et noms de canaux sont validés
- **Authentification**: PASS obligatoire si un mot de passe serveur est défini
- **Isolation**: Chaque client est isolé jusqu'à authentification complète

## 🐛 Gestion des Erreurs

Le serveur gère proprement:
- Déconnexions brutales (connexion perdue)
- Messages malformés (parsing échoué)
- Commandes invalides ou incomplètes
- Conflits de ressources (pseudo/canal en cours d'utilisation)
- Permissions insuffisantes
- Limites de canaux (full, clé incorrecte, invite-only)

## 🎓 Apprentissages Clés du Projet

Ce projet permet d'apprendre:
- **Programmation réseau**: Sockets TCP/IP, multiplexage I/O
- **Protocoles applicatifs**: RFC 1459, communication client-serveur
- **Architecture logicielle**: Séparation des responsabilités, gestion d'état
- **C++98**: Respect des standards, compilation stricte
- **Parsing**: Grammaires formelles, extraction de données structurées
- **Gestion de ressources**: RAII, mémoire, descripteurs de fichiers

## 📚 Resources

### IRC Protocol Documentation
- [RFC 1459 - Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459) - Original IRC protocol specification
- [RFC 2812 - IRC Client Protocol](https://tools.ietf.org/html/rfc2812) - Updated IRC client protocol
- [Modern IRC Documentation](https://modern.ircdocs.horse/) - Contemporary IRC documentation

### IRC Client de Référence
- **irssi**: Client IRC terminal utilisé pour la correction 42
- **nc (netcat)**: Pour les tests automatisés du protocole brut

### C++ and Network Programming
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - TCP/IP sockets tutorial
- C++ 98 Standard Reference
- [epoll man page](https://man7.org/linux/man-pages/man7/epoll.7.html) - Linux I/O multiplexing

### AI Usage in This Project

**Tasks Where AI Was Used:**
1. **Documentation Generation**: AI assisted in creating comprehensive README, test suite documentation, and project analysis reports
2. **Code Review and Analysis**: AI helped identify potential issues and suggest improvements in code structure
3. **Error Message Formatting**: AI provided suggestions for IRC numeric reply messages and error handling patterns
4. **Testing Scenarios**: AI helped brainstorm edge cases and comprehensive test scenarios

**Parts Developed Without AI:**
- Core server architecture and design decisions
- IRC protocol implementation (PASS, NICK, USER, JOIN, PART, MODE, etc.)
- epoll event loop and non-blocking I/O management
- Client and Channel management logic
- All command handlers and parsing logic

**Validation Process:**
- All AI-generated content was reviewed, tested, and adapted to project-specific requirements
- Code functionality was verified through manual testing with real IRC clients (irssi, WeeChat)
- Documentation accuracy was checked against RFC 1459 and project implementation

**Note**: The AI was used as a productivity tool for documentation and analysis, not as a code generator for core functionality. All critical components were implemented with full understanding and can be explained in detail during evaluation.

## 🧪 Tests

Consultez les fichiers suivants pour les tests:
- `MANUAL_TESTS.md`: Guide de tests manuels avec irssi
- `run_tests.sh`: Script de tests automatisés (`./run_tests.sh -v`)

## 👥 Auteurs

- **ezeppa** - [ezeppa@student.42.fr](mailto:ezeppa@student.42.fr)
- **vbonnard** - [vbonnard@student.42.fr](mailto:vbonnard@student.42.fr)

Projet réalisé dans le cadre du cursus de l'école 42.

## 📄 Licence

Ce projet est à des fins éducatives uniquement.

---

**Note**: Ce serveur est conçu pour l'apprentissage et ne doit pas être utilisé en production. Il ne gère pas toutes les extensions modernes d'IRC et n'a pas été testé pour la sécurité en environnement hostile.
