# lem-ipc

*[English version](README.md)*

Une arène de combat multi-processus où chaque joueur est un processus distinct.
Les joueurs s'affrontent sur un plateau 2D partagé ; les équipes se battent
jusqu'à ce qu'il n'en reste qu'une. Il n'y a **aucun `fork`** — chaque joueur est
une invocation indépendante du même binaire, et la seule chose qu'ils partagent
est un espace de noms IPC System V.

C'est un projet de programmation système 42 construit autour des trois
primitives IPC System V : la **mémoire partagée** (le plateau), un **sémaphore**
(exclusion mutuelle) et une **file de messages** (coordination d'équipe).

## Règles du jeu

- Le plateau est une grille carrée. Chaque case non vide contient un numéro
  d'équipe, et une case accueille au plus un joueur.
- Un joueur **meurt** lorsqu'au moins deux ennemis **de la même équipe** lui sont
  adjacents — les huit cases voisines, diagonales comprises. Deux ennemis
  d'équipes *différentes* ne suffisent pas.
- Les joueurs voient des numéros d'équipe, pas des identités : impossible de
  distinguer deux membres d'une même équipe.
- La **dernière équipe en vie gagne**.

Le premier processus lancé crée les ressources partagées et le plateau ; le
dernier à quitter les détruit.

## Compilation

```sh
make
```

Prérequis : `gcc` et les en-têtes de développement ncurses en mode caractères
larges (`libncurses-dev` sur Debian/Ubuntu). La compilation ne produit aucun
avertissement sous `-Wall -Wextra -Werror`.

Le `Makefile` racine construit la `libft` en appelant `libft/Makefile`, puis
compile le projet et le lie à `libft.a` via `-L libft -lft`.

Autres cibles : `make clean`, `make fclean`, `make re`.

## Utilisation

```sh
./lemipc <equipe> [options]
```

`<equipe>` est un entier de 1 à 16 qui identifie l'équipe du joueur. Il est
**obligatoire** pour un joueur normal, et **omis** pour les spectateurs et la
relecture de partie.

### Options

| Option | Description |
|--------|-------------|
| `--map-size <n>` | Taille du plateau, premier joueur uniquement (min 5, max 25, défaut 10). |
| `--ai <niveau>` | Niveau d'IA : `1` aléatoire, `2` poursuite, `3` coordonnée (défaut 3). |
| `--human` | Contrôler le joueur manuellement au clavier. |
| `--spectator` | Observer la partie en lecture seule ; aucune équipe requise. |
| `--walls` | Générer des obstacles sur la carte, premier joueur uniquement. |
| `--replay <fichier>` | Rejouer une partie enregistrée depuis un fichier `.log`. |
| `--verbose` | Afficher les actions de chaque joueur sur la sortie standard. |
| `--help` | Afficher la liste d'aide. |

Notes :

- `--map-size` et `--walls` ne prennent effet que pour le **premier** joueur,
  puisqu'ils façonnent le plateau que tous les autres rejoignent.
- `--human`, `--spectator` et `--replay` sont mutuellement exclusifs — aucune de
  ces trois options ne peut être combinée avec une autre.
- Un joueur humain ou un spectateur ouvre un affichage ncurses ; les joueurs IA
  tournent sans affichage.
- Les murs sont du décor : ils bloquent les déplacements et le pathfinding, et
  sont enregistrés dans le fichier de replay. Ils ne tuent jamais.

### Contrôles

**Joueur humain :** `W` `A` `S` `D` pour se déplacer, `Q` pour quitter.

**Spectateur :** `Q` pour quitter.

**Relecture :** `ESPACE` lecture/pause, `←` / `→` reculer/avancer d'un événement
(en pause), `Q` pour quitter.

## Exemples

Le plus rapide pour voir une partie complète est le script fourni, qui lance
plusieurs équipes, des murs et un spectateur d'un coup :

```sh
./test.sh
```

À la main, lancez d'abord le premier joueur (le créateur) pour qu'il gagne la
course à la mise en place du plateau :

```sh
# le créateur pose le plateau, puis d'autres joueurs rejoignent
./lemipc 1 --walls --map-size 15 &
sleep 0.3
./lemipc 1 & ./lemipc 1 &
./lemipc 2 & ./lemipc 2 & ./lemipc 2 &

# observer la partie
./lemipc --spectator
```

Jouer manuellement contre l'IA :

```sh
./lemipc 1 &        # un coéquipier ou un adversaire IA
./lemipc 2 --human  # vous
```

Rejouer une partie enregistrée :

```sh
./lemipc --replay replay/game-20260615-193012.log
```

À sa sortie, le spectateur affiche un résumé de la partie :

```
lemipc: winning team: 1
lemipc: total turns: 70
lemipc: the team with most kills: 1 (4)
lemipc: game duration: 10565 ms
```

## Mode verbeux et affichage

La sortie verbeuse est écrite sur **stdout**, une ligne par action, préfixée par
l'équipe et le PID afin de distinguer les processus concurrents :

```
[team 1 | pid 12345] joined at (3, 7)
[team 2 | pid 12346] moved (3,7) -> (4,7)
[team 1 | pid 12345] left
```

Un joueur écrit `left` aussi bien lorsqu'il est tué que lorsqu'il quitte de
lui-même.

L'affichage ncurses (`--human`, `--spectator`) dessine directement dans le
terminal : la **sortie verbeuse et un affichage dans le même terminal se
perturbent** mutuellement. Pour utiliser les deux, redirigez stdout vers un
fichier — l'affichage reste dans le terminal et les logs partent dans le
fichier :

```sh
./lemipc 1 --human --verbose > game.log
```

Vous pouvez ensuite suivre les logs en direct depuis un autre terminal avec
`tail -f game.log`. Sinon, lancez les processus verbeux et le spectateur dans
des terminaux séparés.

## Système de relecture

Chaque partie est enregistrée automatiquement dans
`replay/game-<date>-<heure>.log`. Le fichier stocke le plateau initial (taille et
disposition des murs) suivi d'un flux ordonné et horodaté d'événements `JOIN`,
`MOVE` et `QUIT` :

```
MAP 10
BOARD 0000000000000000000000000000000000...
0 0 JOIN 1 2 0
403 1 JOIN 2 7 3
1005 2 MOVE 1 2 0 1 0
```

Le mode relecture relit le fichier, reconstruit le plateau étape par étape et
permet de parcourir la partie en avant comme en arrière. Le parseur valide
l'en-tête, l'ordre des événements, les identifiants, les numéros d'équipe et la
légalité des déplacements : les fichiers mal formés ou corrompus sont rejetés
d'emblée plutôt que de faire planter le programme.

## Architecture

- La **mémoire partagée** contient un en-tête (taille de la carte, nombre de
  joueurs, horodatages, statistiques de kills, drapeaux d'exécution, un tampon
  circulaire de logs) suivi du plateau brut.
- Le **sémaphore** protège chaque lecture/écriture du plateau et chaque mise à
  jour de statistiques, pour que les nombreux processus ne se marchent jamais
  dessus. Il utilise le motif d'initialisation robuste par `sem_otime` afin de
  fermer la fenêtre de course entre création et attachement.
- La **file de messages** transporte les coordonnées de cible par équipe, le
  numéro d'équipe servant de type de message pour donner à chaque équipe son
  propre canal virtuel. C'est ce qui alimente l'IA coordonnée de niveau 3.
- L'**IA** existe en trois niveaux : marche aléatoire, poursuite de l'ennemi le
  plus proche, et coordination par BFS multi-sources partagée au sein de
  l'équipe.
- Les **signaux** (`SIGINT`, `SIGTERM`) positionnent un drapeau d'arrêt global
  pour que chaque processus — partie ou relecture — démonte son affichage et
  libère ses ressources proprement au lieu de laisser fuir des objets IPC.

Si une exécution est interrompue brutalement et laisse des objets IPC derrière
elle, vous pouvez les inspecter et les nettoyer avec `ipcs` et `ipcrm -a`, puis
supprimer le fichier de verrou `/tmp/lemipc`.

## Organisation du projet

```
include/        en-têtes publics
src/            cœur : ipc, board, player, game, ai, bfs, display, signals…
src/replay/     enregistreur et lecteur de relecture
libft/          libc maison (Makefile dédié)
ft_printf/      printf maison
get_next_line/  lecteur de lignes maison
parser/         parseur d'options CLI
test.sh         lance une partie de démonstration complète
```
