# RainFall

RainFall est un projet de sécurité informatique de **42** basé sur l'exploitation de binaires ELF en **i386 (32 bits)**.

Le but est de trouver une vulnérabilité dans chaque binaire afin d'obtenir les privilèges de l'utilisateur du niveau suivant et récupérer son fichier `.pass`.

## Connexion

La VM fournit une adresse IP permettant de se connecter en SSH sur le port `4242`.

```bash
ssh level0@<IP> -p 4242
```

Identifiants initiaux :

```text
login    : level0
password : level0
```

Une fois connecté, l'objectif est de passer :

```text
level0 → level1 → level2 → ... → level9
```

En récupérant à chaque fois :

```bash
cat /home/user/levelX/.pass
```

## SUID

Les binaires sont exécutés avec les privilèges de leur propriétaire grâce au bit **SUID**.

Exemple :

```text
-rwsr-s---+ 1 level1 users ... level0
```

Le programme `level0` sera donc exécuté avec l'EUID de `level1`.

## Outils

Les principaux outils utilisés pour analyser les binaires :

```bash
file
strings
gdb
objdump
nm
ltrace
```

L'analyse se fait principalement avec `gdb` et l'assembleur afin de comprendre le fonctionnement du programme et identifier la vulnérabilité.

## Niveaux

```text
| Niveau |
| ------ |
| level0 |
| level1 |
| level2 |
| level3 |
| level4 |
| level5 |
| level6 |
| level7 |
| level8 |
| level9 |
```

## Bonus

Après `level9`, les niveaux bonus sont :

```text
bonus0
bonus1
bonus2
bonus3
```

Le dernier utilisateur est `end`.

La partie bonus n'est évaluée que si tous les niveaux obligatoires sont validés.

## Organisation

Chaque niveau contient :

```text
levelX/
├── flag
├── source
├── readme
└── Ressources/
```

* `source` : reconstruction du code du binaire.
* `readme` : étapes de résolution.
* `Ressources/` : fichiers nécessaires pour l'évaluation.

Aucun binaire ne doit être présent dans le repository.
