# Guide de tests manuels (ft_irc)

Ce document décrit comment tester manuellement le serveur avec **irssi**, **nc** et le **bot bonus**.

## 1) Prérequis

- Être dans le dossier du projet
- Avoir `irssi` installé (pour les tests complets)
- Compiler le projet

```bash
make
```

Lancement du serveur :

```bash
./ircserv 6667 superpass
```

Le serveur écoute sur `127.0.0.1:6667`.

---

## 2) Démarrage rapide IRSSI (1 client)

Dans un autre terminal:

```bash
irssi
```

Dans IRSSI:

```text
/connect 127.0.0.1 6667 superpass
```

Vérifier que la connexion passe (message de bienvenue).

> Note: selon version/config IRSSI, `/user` peut être ignoré après connexion car IRSSI envoie déjà USER automatiquement.

Déconnexion:

```text
/quit
```

---

## 2b) Tests avec nc (protocole brut)

Pour tester avec `nc` (comme lors de l'évaluation) :

```bash
nc 127.0.0.1 6667
```

Puis taper les commandes (chaque ligne terminée par Entrée) :

```text
PASS superpass
NICK bob
USER bob 0 * :Bob User
JOIN #test
PRIVMSG #test :Hello from nc
```

> **Note :** Utiliser `nc -C` si disponible pour envoyer des lignes en CRLF (`\r\n`), requis par le protocole IRC.

---

## 3) Tests simples (commande par commande)

Ces tests couvrent les commandes implémentées côté serveur:

- `PASS`, `NICK`, `USER`, `CAP`, `QUIT`
- `JOIN`, `PART`, `TOPIC`, `NAMES`
- `MODE` (user `+i`, channel `+i/+t/+k/+l/+o`)
- `KICK`, `INVITE`
- `PRIVMSG`, `NOTICE`

### A. NICK

```text
/nick bob
```

Attendu: changement de nick accepté.

Test nick invalide:

```text
/nick 123bad
```

Attendu: erreur `432 Erroneous nickname`.

### B. JOIN / PART / NAMES

```text
/join #test
/names #test
/part #test :bye
```

Attendu:
- JOIN réussi.
- NAMES retourne la liste des users (avec `@` pour op).
- PART retire le client du salon.

### C. TOPIC

```text
/join #topicroom
/topic #topicroom
/topic #topicroom :Bienvenue dans le salon
/topic #topicroom
```

Attendu:
- Avant définition: `331 No topic is set`.
- Après définition: topic visible via `332`.

### D. MODE (user + channel)

Mode utilisateur:

```text
/mode <votre_nick>
/mode <votre_nick> +i
/mode <votre_nick>
/mode <votre_nick> -i
```

Mode channel:

```text
/join #modes
/mode #modes
/mode #modes +i
/mode #modes +t
/mode #modes +k secretkey
/mode #modes +l 2
/mode #modes
/mode #modes -k
/mode #modes -l
/mode #modes -i
/mode #modes -t
```

Attendu:
- Les modes changent et sont broadcast aux membres.
- Les modes `+k` et `+l` demandent un paramètre.

### E. PRIVMSG

Message channel:

```text
/join #chat
/msg #chat hello-channel
```

Message privé:

```text
/msg bob hello-bob
```

Attendu:
- Sur channel: tous les membres sauf l'émetteur reçoivent.
- En privé: seul le destinataire reçoit.

### F. Transfert d'opérateur (PART/QUIT/KICK)

Quand le dernier op quitte un channel, le premier membre restant devient op automatiquement :

1. `alice` crée `#transfer` et devient op
2. `bob` rejoint
3. `alice` quitte : `/part #transfer`
4. Vérifier : `bob` doit être promu op (message `MODE #transfer +o bob`)

---

## 4) Scénarios complets multi-terminaux IRSSI

Ouvrir **3 terminaux** IRSSI:

- Terminal A: `alice`
- Terminal B: `bob`
- Terminal C: `charlie`

Connexion de chaque client:

```text
/nick <nick>
/connect 127.0.0.1 6667 superpass
```

## Scénario 1: salon public + messagerie privée

1. `alice`: `/join #team`
2. `bob`: `/join #team`
3. `charlie`: `/join #team`
4. `alice`: `/msg #team Bonjour à tous`
5. `bob`: `/msg alice Salut Alice`

Validation:
- Message channel reçu par les autres membres.
- Message privé visible uniquement par la cible.

## Scénario 2: droits opérateur, topic, kick

1. `alice` crée `#ops` (elle devient opératrice automatiquement): `/join #ops`
2. `bob` et `charlie` rejoignent: `/join #ops`
3. `alice` fixe topic: `/topic #ops :Règles du canal`
4. `alice` active topic restreint: `/mode #ops +t`
5. `bob` essaie de changer topic: `/topic #ops :nouveau topic` (doit échouer)
6. `alice` donne op à bob: `/mode #ops +o bob`
7. `bob` change topic: `/topic #ops :Topic par bob` (doit réussir)
8. `bob` kick `charlie`: `/kick #ops charlie :cleanup`

Validation:
- Restriction topic fonctionne.
- Promotion opérateur fonctionne.
- Kick expulse bien la cible du channel.

## Scénario 3: invite-only + clé + limite

1. `alice`: `/join #secure`
2. `alice`: `/mode #secure +i`
3. `alice`: `/mode #secure +k key42`
4. `alice`: `/mode #secure +l 2`
5. `bob` tente `/join #secure` (doit échouer)
6. `alice` invite `bob`: `/invite bob #secure`
7. `bob` rejoint avec clé: `/join #secure key42` (doit réussir)
8. `charlie` tente `/join #secure key42` (doit échouer, channel plein)

Validation:
- Invite-only appliqué.
- Clé channel appliquée.
- Limite de membres appliquée.

## Scénario 4: robustesse erreurs

Depuis n'importe quel client:

```text
/part #doesnotexist
/topic #doesnotexist
/mode #doesnotexist
/msg #doesnotexist test
/invite nobody #team
/kick #team nobody
```

Validation:
- Le serveur répond avec des erreurs IRC cohérentes (`403`, `401`, `441`, etc.)
- Le serveur ne crash pas.

---

## 5) Validation côté console serveur

Dans le terminal du serveur:

```text
help
clients
channels
client alice
log show
log info off
log info on
```

Validation:
- Les commandes console fonctionnent.
- Les filtres de logs se mettent bien à jour.

Arrêt propre du serveur:

```text
quit
```

---

## 6) Critères de réussite globaux

Le test est validé si:

- Le serveur accepte les connexions avec mot de passe correct.
- Les commandes IRC implémentées répondent sans crash.
- Les règles de droits (operator, invite-only, key, limit, topic-restricted) sont respectées.
- Les échanges privés et channel fonctionnent en multi-clients.
- Les erreurs IRC attendues sont renvoyées pour les cas invalides.
- La commande `quit` console ferme proprement le serveur.

---

## 7) Bonus : Bot IRC

Dans un terminal séparé, lancer le bot :

```bash
./ircbot 127.0.0.1 6667 superpass #test
```

Le bot rejoint `#test` et envoie un message aléatoire toutes les 10 secondes. Rejoindre le même channel avec irssi ou nc pour voir les messages.

---

## 7b) Bonus : transfert de fichiers (DCC)

Le transfert **n’est pas hébergé par le serveur** : le serveur relaie seulement les messages CTCP (`\x01DCC SEND ...\x01`). Les deux clients ouvrent une connexion TCP directe entre eux pour envoyer le fichier.

**Prérequis :** deux instances **irssi** (ou clients compatibles DCC), même machine ou réseau où les ports TCP entre machines sont joignables.

1. Terminal A : `./ircserv 6667 superpass`
2. Terminal B : `irssi` → `/connect 127.0.0.1 6667 superpass` → `/nick alice`
3. Terminal C : `irssi` → `/connect 127.0.0.1 6667 superpass` → `/nick bob`
4. Les deux : `/join #dcc`
5. Depuis **alice** : `/dcc send bob /chemin/vers/un_petit_fichier.txt` (chemin réel sur la machine)
6. **bob** doit voir une proposition d’acceptation DCC (`/dcc get` selon la config irssi)

Si le parseur rejetait le message (avant correctif), aucun `DCC SEND` n’arrivait chez le destinataire.

---

## 8) Astuces utiles IRSSI

- Changer de salon/fenêtre: `Alt` + numéro (ou `/window goto <n>`)
- Afficher fenêtres: `/window list`
- Envoyer une commande brute IRC: `/quote ...`
- Quitter IRSSI: `/quit`
