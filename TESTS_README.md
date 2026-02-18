# Tests IRC Server

Ce document décrit les tests implémentés pour valider le fonctionnement du serveur IRC selon les normes RFC.

## Structure des Tests

### 1. Tests d'Authentification

#### PASS Command
- ✓ Accepte le mot de passe correct
- ✓ Rejette un mot de passe incorrect
- ✓ Rejette PASS après l'enregistrement

#### NICK Command
- ✓ Accepte un nickname valide
- ✓ Rejette un nickname invalide (commence par un chiffre)
- ✓ Rejette un nickname vide
- ✓ Rejette un nickname déjà utilisé

#### USER Command
- ✓ Accepte les informations utilisateur valides
- ✓ Rejette USER après l'enregistrement

#### Registration Flow
- ✓ PASS → NICK → USER déclenche l'enregistrement complet
- ✓ Envoie le message de bienvenue (001)

### 2. Tests de Canaux

#### JOIN Command
- ✓ Crée un nouveau canal
- ✓ Premier utilisateur devient opérateur
- ✓ Permet à d'autres utilisateurs de rejoindre
- ✓ Rejette les noms de canaux invalides (sans #)
- ✓ Respecte le mode invite-only (+i)
- ✓ Vérifie la clé du canal (+k)
- ✓ Respecte la limite d'utilisateurs (+l)

#### PART Command
- ✓ Permet de quitter un canal
- ✓ Supprime l'utilisateur de la liste des membres
- ✓ Rejette PART sur un canal non-existant

#### TOPIC Command
- ✓ Permet à un opérateur de définir le topic
- ✓ Empêche un non-opérateur de modifier le topic avec +t
- ✓ Permet à tous de voir le topic
- ✓ Broadcast le changement de topic à tous les membres

#### NAMES Command
- ✓ Liste tous les membres du canal
- ✓ Marque les opérateurs avec @
- ✓ Envoie RPL_NAMREPLY et RPL_ENDOFNAMES

### 3. Tests de Messages

#### PRIVMSG Command
- ✓ Envoie un message privé à un utilisateur
- ✓ Envoie un message à un canal
- ✓ Rejette les messages vers des utilisateurs inexistants
- ✓ Rejette les messages vers des canaux où l'utilisateur n'est pas membre
- ✓ Format correct du message (avec fullmask)

### 4. Tests d'Opérateurs

#### KICK Command
- ✓ Permet à un opérateur d'expulser un utilisateur
- ✓ Empêche un non-opérateur d'expulser
- ✓ Supprime l'utilisateur du canal
- ✓ Broadcast le message KICK à tous les membres

#### INVITE Command
- ✓ Permet d'inviter un utilisateur
- ✓ Ajoute l'utilisateur à la liste d'invitation
- ✓ Rejette l'invitation d'un utilisateur déjà dans le canal
- ✓ Envoie la notification d'invitation

### 5. Tests de Modes

#### User Modes
- ✓ Définit le mode +i (invisible)
- ✓ Permet de voir ses propres modes
- ✓ Empêche de modifier les modes d'autres utilisateurs

#### Channel Modes
- ✓ **+i** (invite-only) : restreint l'accès au canal
- ✓ **+t** (topic restricted) : seuls les opérateurs peuvent changer le topic
- ✓ **+k** (key) : définit un mot de passe pour le canal
- ✓ **+l** (limit) : limite le nombre d'utilisateurs
- ✓ **+o** (operator) : donne/retire le statut d'opérateur
- ✓ Broadcast les changements de mode à tous les membres
- ✓ Empêche les non-opérateurs de changer les modes

### 6. Tests d'Intégration

#### Full Workflow
- ✓ Enregistrement complet de 2 clients
- ✓ Création d'un canal par le premier client
- ✓ Configuration des modes du canal (+i, +t, +k)
- ✓ Invitation du second client
- ✓ Jonction du second client avec la clé
- ✓ Définition du topic
- ✓ Envoi de messages au canal
- ✓ Envoi de messages privés
- ✓ Attribution du statut d'opérateur

## Exécution des Tests

### Méthode 1 : Script automatique
```bash
chmod +x run_tests.sh
./run_tests.sh
```

### Méthode 2 : Manuelle
```bash
# Compiler les tests
make test

# Exécuter les tests
./test_runner
```

### Méthode 3 : Via CMake
```bash
# Nettoyer et recompiler
make fclean
cmake .
make test

# Exécuter
./test_runner
```

## Couverture des Tests

Les tests couvrent :
- ✅ Toutes les commandes d'authentification (PASS, NICK, USER, CAP)
- ✅ Toutes les commandes de gestion de canaux (JOIN, PART, TOPIC, NAMES)
- ✅ Les messages privés et de canal (PRIVMSG)
- ✅ Les commandes d'opérateur (KICK, INVITE)
- ✅ Tous les modes de canal (+i, +t, +k, +l, +o)
- ✅ Les modes utilisateur (+i)
- ✅ La gestion des erreurs et exceptions
- ✅ Les workflows complets d'utilisation

## Normes RFC Testées

Les tests vérifient la conformité avec :
- **RFC 1459** : Internet Relay Chat Protocol
- **RFC 2812** : Internet Relay Chat: Client Protocol

### Codes de Réponse Testés
- `001` RPL_WELCOME
- `221` RPL_UMODEIS
- `324` RPL_CHANNELMODEIS
- `331` RPL_NOTOPIC
- `332` RPL_TOPIC
- `341` RPL_INVITING
- `353` RPL_NAMREPLY
- `366` RPL_ENDOFNAMES
- `401` ERR_NOSUCHNICK
- `403` ERR_NOSUCHCHANNEL
- `404` ERR_CANNOTSENDTOCHAN
- `411` ERR_NORECIPIENT
- `412` ERR_NOTEXTTOSEND
- `431` ERR_NONICKNAMEGIVEN
- `432` ERR_ERRONEUSNICKNAME
- `433` ERR_NICKNAMEINUSE
- `441` ERR_USERNOTINCHANNEL
- `442` ERR_NOTONCHANNEL
- `443` ERR_USERONCHANNEL
- `451` ERR_NOTREGISTERED
- `461` ERR_NEEDMOREPARAMS
- `462` ERR_ALREADYREGISTRED
- `464` ERR_PASSWDMISMATCH
- `471` ERR_CHANNELISFULL
- `472` ERR_UNKNOWNMODE
- `473` ERR_INVITEONLYCHAN
- `475` ERR_BADCHANNELKEY
- `482` ERR_CHANOPRIVSNEEDED
- `502` ERR_USERSDONTMATCH

## Structure des Fichiers de Test

```
test/
├── main_test.cpp              # Point d'entrée principal
├── client/
│   └── client_test.cpp       # Tests unitaires Client
├── channel/
│   └── channel_test.cpp      # Tests unitaires Channel
├── server/
│   └── server_test.cpp       # Tests unitaires Server
├── commands/
│   └── commands_test.cpp     # Tests des commandes IRC
└── integration_test.cpp      # Tests d'intégration
```

## Résultats Attendus

Tous les tests doivent passer avec succès :
```
╔════════════════════════════════════════╗
║  ✓ ALL COMMAND TESTS PASSED!          ║
╚════════════════════════════════════════╝
```

## Tests Manuels avec Irssi

En complément des tests automatisés, vous pouvez tester manuellement avec irssi :

```bash
# Lancer le serveur
./ircserv

# Dans un autre terminal, connecter irssi
irssi
/connect localhost 6667 1234
/nick TestUser
/join #test
/topic #test Nouveau topic
/mode #test +i
/invite OtherUser #test
/msg OtherUser Hello!
/part #test
```

## Bogues Connus

Aucun bogue connu à ce jour. Si vous en trouvez, veuillez les signaler.

## Contribution

Pour ajouter de nouveaux tests :
1. Ajoutez vos fonctions de test dans le fichier approprié
2. Appelez-les depuis `run_commands_tests()` ou la fonction principale
3. Utilisez `assert()` pour les vérifications
4. Affichez des messages de confirmation avec `std::cout`
