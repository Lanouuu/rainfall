#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Variable globale : code de langue (0 = anglais par défaut, 1 = finnois, 2 = néerlandais) */
int language = 0;

/*
 * greetuser() n'a pas de paramètre "officiel" en C, mais main() triche :
 * juste avant l'appel, il recopie le buffer contenant "prenom+nom" sur la pile,
 * exactement à l'endroit où se trouvera [ebp+0x8] une fois dans greetuser.
 * Résultat : greetuser lit cette zone comme si c'était un argument classique.
 * On le modélise ici en lui passant explicitement une chaîne.
 */
void greetuser(const char *fullname)
{
    char greeting[76]; /* buffer local (0x58 octets alloués par le prologue) */

    if (language == 1) {
        /* "fi" -> finnois : "Hyvää päivää " (UTF-8, d'où les octets 0xC3 0xA4 pour 'ä') */
        strcpy(greeting, "Hyvää päivää ");      // 13 caracteres
    }
    else if (language == 2) {
        /* "nl" -> néerlandais */
        strcpy(greeting, "Goedemiddag! ");
    }
    else {
        /* valeur par défaut (0) -> anglais */
        strcpy(greeting, "Hello ");
    }

    strcat(greeting, fullname);
    puts(greeting);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        return 1;
    }

    /* Buffer unique de 0x4c (76) octets, mis à zéro, qui contiendra
     * prénom (40 octets) suivi directement du nom (32 octets) */
    char fullname[0x4c];
    memset(fullname, 0, sizeof(fullname));

    char *firstname = fullname;        /* 40 octets : 0x28 */
    char *lastname  = fullname + 0x28; /* 32 octets : 0x20, juste après */

    strncpy(firstname, argv[1], 0x28); /* copie argv[1] -> 40 caractères max */
    strncpy(lastname,  argv[2], 0x20); /* copie argv[2] -> 32 caractères max */

    /* Détection de la langue via la variable d'environnement LANG */
    char *lang_env = getenv("LANG");
    if (lang_env != NULL) {
        if (memcmp(lang_env, "fi", 2) == 0) {
            language = 1;
        }
        else if (memcmp(lang_env, "nl", 2) == 0) {
            language = 2;
        }
        /* sinon : language reste à 0 (anglais) */
    }

    /* Le fullname[0x4c] complet est "poussé" sur la pile juste avant
       l'appel, pour que greetuser() le récupère comme pseudo-argument */
    greetuser(fullname);

    return 0;
}

// 40 + 32 = 72 
// 72 + 13 = 85
// 