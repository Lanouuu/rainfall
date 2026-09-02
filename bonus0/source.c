void p(char *dest, char *prompt)
{
    char buf[4104];              // 0x1008 = 4104 octets, buffer local sur la pile

    puts(prompt);                // affiche le prompt (DAT_080486a0)
    read(0, buf, 0x1000);        // lit jusqu'à 4096 octets depuis stdin
    *strchr(buf, '\n') = '\0';   // trouve le premier '\n' et le remplace par '\0'
    strncpy(dest, buf, 20);      // copie au plus 20 octets de buf vers dest
}

void pp(char *param_1)
{
    char local_34[20];   // premier buffer (20 octets)
    char local_20[20];   // second buffer (20 octets), juste après local_34 en mémoire
    size_t len;

    p(local_34, DAT_080486a0);   // 1er input utilisateur -> local_34
    p(local_20, DAT_080486a0);   // 2e input utilisateur  -> local_20

    strcpy(param_1, local_34);
    // strcpy s'arrête au premier octet 0x00 rencontré.
    // Si local_34 n'a PAS été null-terminé par p() (input >= 20 caractères),
    // strcpy continue de lire la pile AU-DELA des 20 octets de local_34,
    // potentiellement jusque dans local_20 ou plus loin !

    len = strlen(param_1);       // équivalent du repnz scasb de l'assembleur

    param_1[len]     = ' ';      // écrase le '\0' de fin par un espace
    param_1[len + 1] = '\0';     // et remet un '\0' juste après

    strcat(param_1, local_20);   // concatène le 2e input à la suite
}

int main(void)
{
    char buf[54];   // buffer final

    pp(buf);        // construit "input1 input2" dans buf
    puts(buf);      // affiche le résultat
    return 0;
}
