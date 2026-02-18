# Tests Manuels du Serveur IRC

Ce document contient tous les tests manuels à effectuer pour valider le bon fonctionnement du serveur IRC selon les normes RFC.

## Préparation

### Étape 1 : Lancer le serveur

```bash
cd /home/ezeppa/Desktop/Workspace/42_ft_irc
make
./ircserv
```

**Résultat attendu :**
```
IRC Project Initialisation
IRC Project Running
```

Le serveur écoute sur `192.168.1.26:6667` avec le mot de passe `1234`.

### Étape 2 : Ouvrir plusieurs terminaux irssi

Dans des terminaux séparés :

```bash
# Terminal 1 - Alice
irssi

# Terminal 2 - Bob
irssi

# Terminal 3 - Charlie
irssi
```

---

## 1. Tests d'Authentification

### Test 1.1 : Connexion avec mot de passe correct

**Terminal : Alice**
```irc
/connect localhost 6667 1234
```

**Résultat attendu :**
```
-!- Irssi: Looking up localhost
-!- Irssi: Connecting to localhost [127.0.0.1] port 6667
Waiting for CAP LS response...
-!- Irssi: Connection to localhost established
-!- Capabilities supported: 
```

**Logs serveur attendus :**
```
[CAP] Client fd=X sub=LS
[CAP LS] Sending: :MyServer CAP * LS :
[PASS] Client fd=X pass=1234
[PASS] Password accepted
```

---

### Test 1.2 : Définir le nickname

**Terminal : Alice**
```irc
/nick Alice
```

**Résultat attendu :**
```
-!- You're now known as Alice
```

**Logs serveur attendus :**
```
[NICK] Client fd=X nick=Alice
[NICK] Nickname accepted: Alice
```

---

### Test 1.3 : Enregistrement complet (automatique après USER)

Irssi envoie automatiquement la commande USER. Le serveur doit envoyer le message de bienvenue.

**Résultat attendu :**
```
-!- Welcome to the Internet Relay Network Alice!alice@192.168.1.26
```

**Logs serveur attendus :**
```
[USER] Client fd=X user=alice realname=alice
[USER] User info set
[tryRegister] Registration complete! Sending welcome
[WELCOME] Sending to fd=X: :MyServer 001 Alice :Welcome to the Internet Relay Network Alice!alice@192.168.1.26
```

✅ **Vérification :** Alice est maintenant enregistrée et peut utiliser toutes les commandes.

---

### Test 1.4 : Connexion de Bob et Charlie

Répéter les étapes 1.2 et 1.3 pour Bob et Charlie.

**Terminal : Bob**
```irc
/connect localhost 6667 1234
/nick Bob
```

**Terminal : Charlie**
```irc
/connect localhost 6667 1234
/nick Charlie
```

✅ **Vérification :** Trois clients sont maintenant connectés (Alice, Bob, Charlie).

---

### Test 1.5 : Nickname déjà utilisé

**Terminal : Charlie**
```irc
/nick Alice
```

**Résultat attendu :**
```
-!- Nick Alice is already in use
```

**Logs serveur attendus :**
```
[NICK] Client fd=X nick=Alice
[NICK] Nickname in use: Alice
```

✅ **Vérification :** Le serveur refuse le nickname en double.

---

### Test 1.6 : PING/PONG automatique

Irssi envoie automatiquement des PING. Vérifier dans les logs serveur.

**Logs serveur attendus (périodiquement) :**
```
[PARSE] Command: PING Args: [MyServer]
[WRITE] fd=X send: :MyServer PONG MyServer :MyServer
```

✅ **Vérification :** Le serveur répond aux PING.

---

## 2. Tests de Canaux

### Test 2.1 : Créer un canal

**Terminal : Alice**
```irc
/join #general
```

**Résultat attendu :**
```
-!- Alice [alice@192.168.1.26] has joined #general
-!- Topic for #general: 
-!- Topic set by 
-!- Channel #general
-!- [Users #general]
-!- [@Alice]
-!- Irssi: #general: Total of 1 nicks [1 ops, 0 halfops, 0 voices, 0 normal]
-!- Channel #general created
```

**Logs serveur attendus :**
```
[PARSE] Command: JOIN Args: [#general]
[WRITE] fd=X send: :Alice!alice@192.168.1.26 JOIN :#general
[WRITE] fd=X send: :MyServer 331 Alice #general :No topic is set
[WRITE] fd=X send: :MyServer 353 Alice = #general :@Alice
[WRITE] fd=X send: :MyServer 366 Alice #general :End of NAMES list
```

✅ **Vérification :** 
- Alice a créé le canal #general
- Alice a le statut d'opérateur (@ devant le nom)

---

### Test 2.2 : Bob rejoint le canal

**Terminal : Bob**
```irc
/join #general
```

**Résultat attendu Alice :**
```
-!- Bob [bob@192.168.1.26] has joined #general
```

**Résultat attendu Bob :**
```
-!- Bob [bob@192.168.1.26] has joined #general
-!- Topic for #general: 
-!- [Users #general]
-!- [@Alice] [Bob]
-!- Irssi: #general: Total of 2 nicks [1 ops, 0 halfops, 0 voices, 1 normal]
```

✅ **Vérification :**
- Bob est membre du canal
- Bob n'est PAS opérateur
- Les deux clients voient l'arrivée de Bob

---

### Test 2.3 : Liste des utilisateurs (NAMES)

**Terminal : Alice ou Bob**
```irc
/names #general
```

**Résultat attendu :**
```
-!- [Users #general]
-!- [@Alice] [Bob]
```

✅ **Vérification :** Liste correcte avec @ pour l'opérateur.

---

### Test 2.4 : Définir un topic

**Terminal : Alice (opérateur)**
```irc
/topic #general Bienvenue sur le canal général !
```

**Résultat attendu (tous les membres) :**
```
-!- Alice changed the topic of #general to: Bienvenue sur le canal général !
```

**Logs serveur attendus :**
```
[PARSE] Command: TOPIC Args: [#general] [Bienvenue sur le canal général !]
```

✅ **Vérification :** Le topic est changé pour tous les membres.

---

### Test 2.5 : Voir le topic

**Terminal : Charlie (pas encore dans le canal)**
```irc
/join #general
```

**Résultat attendu :**
```
-!- Topic for #general: Bienvenue sur le canal général !
```

✅ **Vérification :** Le topic s'affiche à la connexion.

---

### Test 2.6 : Quitter un canal

**Terminal : Charlie**
```irc
/part #general Au revoir !
```

**Résultat attendu (tous les membres) :**
```
-!- Charlie [charlie@192.168.1.26] has left #general [Au revoir !]
```

**Logs serveur attendus :**
```
[PARSE] Command: PART Args: [#general] [Au revoir !]
```

✅ **Vérification :** Charlie a quitté le canal.

---

## 3. Tests de Messages

### Test 3.1 : Message au canal

**Terminal : Alice**
```irc
/join #general
```

Puis :
```irc
Bonjour tout le monde !
```

**Résultat attendu chez Bob :**
```
<Alice> Bonjour tout le monde !
```

**Résultat attendu chez Alice :**
```
(le message n'apparaît pas, il n'est pas renvoyé à l'expéditeur)
```

**Logs serveur attendus :**
```
[PARSE] Command: PRIVMSG Args: [#general] [Bonjour tout le monde !]
```

✅ **Vérification :** Le message est diffusé à tous sauf l'expéditeur.

---

### Test 3.2 : Message privé

**Terminal : Alice**
```irc
/msg Bob Hello Bob en privé !
```

**Résultat attendu chez Bob :**
```
*Alice* Hello Bob en privé !
```

**Résultat attendu chez Alice :**
```
-> *Bob* Hello Bob en privé !
```

**Logs serveur attendus :**
```
[PARSE] Command: PRIVMSG Args: [Bob] [Hello Bob en privé !]
```

✅ **Vérification :** Message privé reçu uniquement par Bob.

---

### Test 3.3 : Message à un utilisateur inexistant

**Terminal : Alice**
```irc
/msg NonExistant Test
```

**Résultat attendu :**
```
-!- No such nick/channel: NonExistant
```

✅ **Vérification :** Erreur claire.

---

## 4. Tests de Modes de Canal

### Test 4.1 : Mode +i (invite-only)

**Terminal : Alice (opérateur de #general)**
```irc
/mode #general +i
```

**Résultat attendu (tous les membres) :**
```
-!- mode/#general [+i] by Alice
```

**Logs serveur attendus :**
```
[PARSE] Command: MODE Args: [#general] [+i]
```

✅ **Vérification :** Le canal est maintenant en invite-only.

---

### Test 4.2 : Vérifier +i bloque les JOIN

**Terminal : Charlie (pas dans le canal)**
```irc
/join #general
```

**Résultat attendu :**
```
-!- #general Cannot join channel (+i) - you must be invited
```

**Logs serveur attendus :**
```
[PARSE] Command: JOIN Args: [#general]
(exception ERR_INVITEONLYCHAN)
```

✅ **Vérification :** Charlie ne peut pas rejoindre sans invitation.

---

### Test 4.3 : Inviter un utilisateur

**Terminal : Alice**
```irc
/invite Charlie #general
```

**Résultat attendu Alice :**
```
-!- Inviting Charlie to #general
```

**Résultat attendu Charlie :**
```
-!- Alice invites you to #general
```

**Logs serveur attendus :**
```
[PARSE] Command: INVITE Args: [Charlie] [#general]
```

✅ **Vérification :** Charlie est invité.

---

### Test 4.4 : Join après invitation

**Terminal : Charlie**
```irc
/join #general
```

**Résultat attendu :**
```
-!- Charlie [charlie@192.168.1.26] has joined #general
```

✅ **Vérification :** Charlie peut rejoindre après invitation.

---

### Test 4.5 : Mode +t (topic restricted)

**Terminal : Alice**
```irc
/mode #general +t
```

**Résultat attendu :**
```
-!- mode/#general [+t] by Alice
```

✅ **Vérification :** Seuls les opérateurs peuvent changer le topic maintenant.

---

### Test 4.6 : Bob tente de changer le topic (non-opérateur)

**Terminal : Bob**
```irc
/topic #general Nouveau topic par Bob
```

**Résultat attendu :**
```
-!- You're not channel operator
```

✅ **Vérification :** Bob ne peut pas changer le topic avec +t actif.

---

### Test 4.7 : Mode +k (clé de canal)

**Terminal : Alice**
```irc
/mode #general +k secretpass
```

**Résultat attendu :**
```
-!- mode/#general [+k secretpass] by Alice
```

---

### Test 4.8 : Vérifier que +k nécessite le mot de passe

**Terminal : Charlie**
```irc
/part #general
/join #general
```

**Résultat attendu :**
```
-!- #general Cannot join channel (+k) - bad key
```

---

### Test 4.9 : Join avec la bonne clé

**Terminal : Charlie**
```irc
/join #general secretpass
```

**Résultat attendu :**
```
-!- Charlie [charlie@192.168.1.26] has joined #general
```

✅ **Vérification :** Charlie peut rejoindre avec la bonne clé.

---

### Test 4.10 : Mode +l (limite d'utilisateurs)

D'abord, s'assurer qu'il y a seulement Alice et Bob dans #general.

**Terminal : Alice**
```irc
/mode #general +l 2
```

**Résultat attendu :**
```
-!- mode/#general [+l 2] by Alice
```

---

### Test 4.11 : Vérifier la limite

**Terminal : Charlie**
```irc
/join #general secretpass
```

**Résultat attendu :**
```
-!- #general Cannot join channel (+l) - channel is full
```

✅ **Vérification :** Le canal refuse les nouveaux membres si la limite est atteinte.

---

### Test 4.12 : Augmenter la limite

**Terminal : Alice**
```irc
/mode #general +l 5
```

**Terminal : Charlie**
```irc
/join #general secretpass
```

**Résultat attendu :**
```
-!- Charlie [charlie@192.168.1.26] has joined #general
```

✅ **Vérification :** Charlie peut rejoindre après augmentation de la limite.

---

### Test 4.13 : Mode +o (donner opérateur)

**Terminal : Alice**
```irc
/mode #general +o Bob
```

**Résultat attendu :**
```
-!- mode/#general [+o Bob] by Alice
```

---

### Test 4.14 : Vérifier que Bob est opérateur

**Terminal : Bob**
```irc
/names #general
```

**Résultat attendu :**
```
-!- [@Alice] [@Bob] [Charlie]
```

✅ **Vérification :** Bob a maintenant le @.

---

### Test 4.15 : Bob peut maintenant changer le topic

**Terminal : Bob**
```irc
/topic #general Topic changé par Bob
```

**Résultat attendu :**
```
-!- Bob changed the topic of #general to: Topic changé par Bob
```

✅ **Vérification :** Bob peut modifier le topic maintenant qu'il est opérateur.

---

### Test 4.16 : Retirer l'opérateur

**Terminal : Alice**
```irc
/mode #general -o Bob
```

**Résultat attendu :**
```
-!- mode/#general [-o Bob] by Alice
```

**Vérification :**
```irc
/names #general
```
Résultat : `[@Alice] [Bob] [Charlie]`

✅ **Vérification :** Bob n'est plus opérateur.

---

### Test 4.17 : Retirer les modes

**Terminal : Alice**
```irc
/mode #general -itk
```

**Résultat attendu :**
```
-!- mode/#general [-itk] by Alice
```

---

### Test 4.18 : Voir tous les modes du canal

**Terminal : Alice ou Bob**
```irc
/mode #general
```

**Résultat attendu :**
```
-!- mode/#general [+l]
```

(Seul +l reste actif avec la limite de 5)

✅ **Vérification :** Les modes du canal sont affichés correctement.

---

## 5. Tests d'Opérateurs

### Test 5.1 : KICK (expulser un utilisateur)

**Terminal : Alice (opérateur)**
```irc
/kick #general Charlie Comportement inapproprié
```

**Résultat attendu (tous les membres) :**
```
-!- Charlie was kicked from #general by Alice [Comportement inapproprié]
```

**Logs serveur attendus :**
```
[PARSE] Command: KICK Args: [#general] [Charlie] [Comportement inapproprié]
```

✅ **Vérification :** Charlie est expulsé du canal.

---

### Test 5.2 : Charlie ne peut plus envoyer de messages

**Terminal : Charlie**
```irc
Hello
```

**Résultat attendu :**
```
(Le message n'est pas envoyé, car Charlie n'est plus dans le canal)
```

✅ **Vérification :** Charlie n'est plus membre.

---

### Test 5.3 : KICK par un non-opérateur (devrait échouer)

**Terminal : Bob (non-opérateur maintenant)**

Charlie rejoint à nouveau :
```irc
# Terminal Charlie
/join #general
```

Puis Bob essaie de kick :
```irc
# Terminal Bob
/kick #general Charlie Test
```

**Résultat attendu :**
```
-!- You're not channel operator
```

✅ **Vérification :** Seuls les opérateurs peuvent utiliser KICK.

---

## 6. Modes Utilisateur

### Test 6.1 : Mode +i utilisateur (invisible)

**Terminal : Alice**
```irc
/mode Alice +i
```

**Résultat attendu :**
```
-!- mode/Alice [+i] by Alice
```

---

### Test 6.2 : Voir son mode

**Terminal : Alice**
```irc
/mode Alice
```

**Résultat attendu :**
```
-!- Your user mode is [+i]
```

✅ **Vérification :** Le mode utilisateur fonctionne.

---

## 7. Tests de Déconnexion

### Test 7.1 : QUIT avec message

**Terminal : Charlie**
```irc
/quit Au revoir tout le monde !
```

**Résultat attendu (tous les clients dans #general) :**
```
-!- Charlie [charlie@192.168.1.26] has quit [Au revoir tout le monde !]
```

**Logs serveur attendus :**
```
[PARSE] Command: QUIT Args: [Au revoir tout le monde !]
[WRITE] fd=X send: :Charlie!charlie@192.168.1.26 QUIT :Au revoir tout le monde !
```

✅ **Vérification :** Charlie se déconnecte proprement.

---

## 8. Scénarios Complexes

### Scénario 8.1 : Canal privé avec gestion complète

**Objectif :** Créer un canal privé avec clé, limite, topic restreint, et invitations.

**Setup :**

1. **Alice crée #private**
   ```irc
   /join #private
   ```

2. **Alice configure le canal**
   ```irc
   /mode #private +itk vippass
   /mode #private +l 3
   /topic #private Canal VIP - Accès restreint
   ```

3. **Bob tente de rejoindre (devrait échouer)**
   ```irc
   # Terminal Bob
   /join #private vippass
   ```
   Résultat : `Cannot join channel (+i) - you must be invited`

4. **Alice invite Bob**
   ```irc
   # Terminal Alice
   /invite Bob #private
   ```

5. **Bob rejoint avec la clé**
   ```irc
   # Terminal Bob
   /join #private vippass
   ```
   Résultat : ✅ Bob est dans #private

6. **Alice donne l'opérateur à Bob**
   ```irc
   /mode #private +o Bob
   ```

7. **Charlie tente de rejoindre (limite atteinte)**
   
   Attendre qu'un troisième utilisateur rejoigne, puis :
   ```irc
   # Terminal d'un 4ème utilisateur
   /join #private vippass
   ```
   Résultat : `Cannot join channel (+l) - channel is full`

✅ **Vérifications :**
- +i empêche le join direct
- +k nécessite le mot de passe
- +t empêche les non-ops de changer le topic
- +l limite le nombre d'utilisateurs
- INVITE permet le join malgré +i
- +o donne les privilèges d'opérateur

---

### Scénario 8.2 : Messages multiples et privés

1. **Créer #discussion**
   ```irc
   # Alice, Bob, Charlie tous dans #discussion
   ```

2. **Conversation publique**
   ```irc
   # Alice
   Bonjour à tous !
   
   # Bob
   Salut Alice !
   
   # Charlie
   Hello !
   ```

3. **Message privé pendant la conversation**
   ```irc
   # Alice
   /msg Bob Rendez-vous en privé dans #secret ?
   
   # Bob
   /msg Alice D'accord !
   ```

4. **Créer #secret**
   ```irc
   # Alice
   /join #secret
   /mode #secret +ik secretkey
   /invite Bob #secret
   
   # Bob
   /join #secret secretkey
   ```

5. **Discussion privée**
   ```irc
   # Alice dans #secret
   On peut parler tranquillement ici
   ```

6. **Charlie ne voit rien**
   ```irc
   # Charlie dans #discussion continue de voir les messages publics
   ```

✅ **Vérifications :**
- Messages publics visibles par tous dans le canal
- Messages privés visibles uniquement par le destinataire
- Canaux multiples fonctionnent indépendamment

---

## 9. Tests de Robustesse

### Test 9.1 : Commande inconnue

**Terminal : Alice**
```irc
/raw UNKNOWNCMD test
```

**Résultat attendu :**
```
:MyServer 421 Alice UNKNOWNCMD :Unknown command
```

✅ **Vérification :** Le serveur gère les commandes inconnues.

---

### Test 9.2 : Paramètres manquants

**Terminal : Alice**
```irc
/raw PRIVMSG
```

**Résultat attendu :**
```
:MyServer 461 Alice PRIVMSG :Not enough parameters
```

✅ **Vérification :** Le serveur vérifie les paramètres.

---

### Test 9.3 : Canal inexistant

**Terminal : Alice**
```irc
/topic #nonexistant test
```

**Résultat attendu :**
```
-!- #nonexistant: No such channel
```

✅ **Vérification :** Erreur claire.

---

## 10. Checklist Finale

Après avoir effectué tous les tests, vérifier :

- [ ] 1. Authentification (PASS, NICK, USER) fonctionne
- [ ] 2. Message de bienvenue (001) reçu
- [ ] 3. Création de canaux fonctionne
- [ ] 4. Premier utilisateur devient opérateur automatiquement
- [ ] 5. JOIN/PART fonctionnent correctement
- [ ] 6. TOPIC peut être vu et modifié
- [ ] 7. NAMES liste correctement les utilisateurs avec @
- [ ] 8. PRIVMSG canal fonctionne (broadcast sauf expéditeur)
- [ ] 9. PRIVMSG privé fonctionne
- [ ] 10. Mode +i (invite-only) bloque les joins
- [ ] 11. INVITE permet de rejoindre un canal +i
- [ ] 12. Mode +t (topic restricted) fonctionne
- [ ] 13. Mode +k (clé) nécessite le bon mot de passe
- [ ] 14. Mode +l (limite) refuse les utilisateurs en trop
- [ ] 15. Mode +o (operator) donne les privilèges
- [ ] 16. Mode -o retire les privilèges
- [ ] 17. KICK expulse un utilisateur (opérateur uniquement)
- [ ] 18. Mode utilisateur +i fonctionne
- [ ] 19. QUIT déconnecte proprement
- [ ] 20. PING/PONG automatique fonctionne
- [ ] 21. Plusieurs canaux simultanés fonctionnent
- [ ] 22. Gestion d'erreurs (commandes inconnues, paramètres manquants)
- [ ] 23. Nicknames en double refusés
- [ ] 24. Nicknames invalides refusés
- [ ] 25. Logs serveur cohérents et complets

---

## Notes

### Tips pour les tests

1. **Garder les logs serveur visibles** pour voir les commandes parsées
2. **Tester dans l'ordre** pour éviter les états incohérents
3. **Nettoyer entre chaque scénario** en quittant tous les canaux
4. **Vérifier les deux côtés** : ce que l'expéditeur voit ET ce que le destinataire voit

### Commandes irssi utiles

```irc
/connect localhost 6667 1234    # Connexion
/nick NomUtilisateur             # Changer de nom
/join #canal [clé]               # Rejoindre un canal
/part #canal [message]           # Quitter un canal
/msg cible message               # Message privé ou canal
/topic #canal [nouveau topic]    # Voir/changer topic
/mode #canal [+/-modes] [params] # Voir/changer modes
/names #canal                    # Liste des utilisateurs
/invite utilisateur #canal       # Inviter
/kick #canal utilisateur raison  # Expulser
/quit [message]                  # Déconnexion
/raw COMMANDE params             # Envoyer commande brute
```

### Codes d'erreur RFC testés

- `001` RPL_WELCOME - Message de bienvenue
- `221` RPL_UMODEIS - Mode utilisateur
- `324` RPL_CHANNELMODEIS - Mode canal
- `331` RPL_NOTOPIC - Pas de topic
- `332` RPL_TOPIC - Topic du canal
- `341` RPL_INVITING - Invitation envoyée
- `353` RPL_NAMREPLY - Liste des noms
- `366` RPL_ENDOFNAMES - Fin de liste
- `401` ERR_NOSUCHNICK - Utilisateur inexistant
- `403` ERR_NOSUCHCHANNEL - Canal inexistant
- `421` ERR_UNKNOWNCOMMAND - Commande inconnue
- `432` ERR_ERRONEUSNICKNAME - Nickname invalide
- `433` ERR_NICKNAMEINUSE - Nickname utilisé
- `442` ERR_NOTONCHANNEL - Pas sur le canal
- `461` ERR_NEEDMOREPARAMS - Pas assez de paramètres
- `471` ERR_CHANNELISFULL - Canal plein
- `473` ERR_INVITEONLYCHAN - Canal invite-only
- `475` ERR_BADCHANNELKEY - Mauvaise clé
- `482` ERR_CHANOPRIVSNEEDED - Pas opérateur

---

## Résumé des Résultats

Après avoir effectué tous les tests, remplir ce tableau :

| Catégorie | Tests Passed | Tests Failed | Notes |
|-----------|--------------|--------------|-------|
| Authentification | /6 | /6 | |
| Canaux | /6 | /6 | |
| Messages | /3 | /3 | |
| Modes Canal | /18 | /18 | |
| Opérateurs | /3 | /3 | |
| Modes Utilisateur | /2 | /2 | |
| Déconnexion | /1 | /1 | |
| Scénarios | /2 | /2 | |
| Robustesse | /3 | /3 | |
| **TOTAL** | /44 | /44 | |

---

**Bon courage pour les tests ! 🚀**
