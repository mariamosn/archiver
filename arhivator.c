// Maria Moșneag 313CA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define __USE_XOPEN
#define _GNU_SOURCE
#include <time.h>

#define MAX_LINE 512

typedef union record {
    char charptr[512];
    struct header {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        char mtime[12];
        char chksum[8];
        char typeflag;
        char linkname[100];
        char magic[8];
        char uname[32];
        char gname[32];
        char devmajor[8];
        char devminor[8];
    } header;
} record;

// funcția completează câmpul name cu numele fișierului curent
void name(char *line, char *name) {
    char *info, *copy;
    if (!(copy = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    strcpy(copy, line);
    info = strtok(copy, " \n");
    for (int i = 1; i <= 8; i++) {
        info = strtok(NULL, " \n");
    }
    strcpy(name, info);
    free(copy);
}

// funcția completează câmpul mode cu permisiunile fișierului curent
void permission(char *line, char *mode) {
    char *info, *copy;
    if (!(copy = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    strcpy(copy, line);
    int i, j, x;
    info = strtok(copy, " \n");
    for (i = 0; i < 5; i++) {
        mode[i] = '0';
    }
    for (i = 0; i < 3; i++) {
        x = 0;
        for (j = 0; j < 3; j++) {
            if (info[1 + i * 3 + j] != '-') {
                x += pow(2, 2 - j);
            }
        }
        mode[4 + i] = x + '0';
    }
    free(copy);
}

// funcția completează câmpurile uname și uid
void user(char *line, char *uid, char *name) {
    char *info, *usr, *copy;
    int ok = 0, i;
    FILE *users;
    if (!(usr = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    if (!(copy = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    strcpy(copy, line);
    // aflăm username-ul
    info = strtok(copy, " \n");
    info = strtok(NULL, " \n");
    info = strtok(NULL, " \n");
    strcpy(name, info);
    if ((users = fopen("usermap.txt", "rt")) == NULL) {
        printf("> Failed!\n");
        exit(1);
    }
    // aflăm uid-ul și calculăm valoarea acestuia în octal
    while (fgets(usr, MAX_LINE, users) && !ok) {
        if (strstr(usr, info)) {
            info = strtok(usr, ":");
            info = strtok(NULL, ":");
            info = strtok(NULL, ":");
            ok = 1;
        }
    }
    ok = atoi(info);
    sprintf(info, "%o", ok);
    ok = atoi(info);
    for (i = 6; i >= 0; i--) {
        uid[i] = '0' + ok % 10;
        ok /= 10;
    }
    fclose(users);
    free(usr);
    free(copy);
}

// funcția completează câmpurile gid și gname
void group(char *line, char *gid, char *name) {
    char *info, *usr, *copy;
    int ok = 0, i;
    FILE *users;
    if (!(usr = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    if (!(copy = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    // aflăm numele grupului
    strcpy(copy, line);
    info = strtok(copy, " \n");
    for (int i = 1; i <= 3; i++) {
        info = strtok(NULL, " \n");
    }
    strcpy(name, info);
    if ((users = fopen("usermap.txt", "rt")) == NULL) {
        printf("> Failed!\n");
        exit(1);
    }
    // aflăm gid-ul și transformăm valoarea acestuia în octal
    while (fgets(usr, MAX_LINE, users) && !ok) {
        if (strstr(usr, info)) {
            info = strtok(usr, ":");
            for (i = 1; i <= 3; i++) {
                info = strtok(NULL, ":");
            }
            ok = 1;
        }
    }
    ok = atoi(info);
    sprintf(info, "%o", ok);
    ok = atoi(info);
    for (i = 6; i >= 0; i--) {
        gid[i] = '0' + ok % 10;
        ok /= 10;
    }
    fclose(users);
    free(usr);
    free(copy);
}

// funcția completează câmpul size cu dimensiunea fișierului curent
void size(char *line, char *size) {
    char *copy, *info;
    int sz, ok, i;
    if (!(copy = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    strcpy(copy, line);
    info = strtok(copy, " \n");
    for (int i = 1; i <= 4; i++) {
        info = strtok(NULL, " \n");
    }
    sz = atoi(info);
    // calculăm valoarea în octal
    sprintf(info, "%o", sz);
    for (i = 0; i < 11; i++) {
        size[i] = '0';
    }
    ok = strlen(info);
    for (i = 10; i > 0 && ok; i--) {
        size[i] = info[ok - 1];
        ok--;
    }
    free(copy);
}

// funcția completează câmpul mtime
void timestamp(char *line, char *mtime) {
    struct tm time, start;
    char *info, *copy;
    int i;
    if (!(copy = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    strcpy(copy, line);
    info = strtok(copy, " \n");
    for (i = 1; i <= 5; i++) {
        info = strtok(NULL, " \n");
    }
    // în variabila start de tip tm reținem informațiile
    // corespunzătoare datei 01.01.1970 00:00:00
    start.tm_sec = 0;
    start.tm_min = 0;
    start.tm_hour = 0;
    start.tm_mday = 1;
    start.tm_mon = 0;
    start.tm_year = 1970 - 1900;
    start.tm_isdst = -1;
    // în variabila time reținem informațiile referitoare la
    // data ultimei modificări asupra fișierului curent
    strptime(info, "%Y-%m-%d", &time);
    info = strtok(NULL, " \n");
    strptime(info, "%T", &time);
    // calculăm diferența dată de fusul orar
    info = strtok(NULL, " \n");
    i = atoi(info);
    i /= 100;
    start.tm_hour += i;
    time.tm_isdst = -1;
    // calculăm diferența de secunde dintre cele două date
    sprintf(mtime, "%o", (int) difftime(mktime(&time), mktime(&start)));
    mtime[11] = '\0';
    free(copy);
}

// funcția completează câmpurile devminor și devmajor
void dev(char *min, char *maj) {
    int i;
    for (i = 0; i < 7; i++) {
        min[i] = maj[i] = '0';
    }
    min[7] = maj[7] = '\0';
}

// funcția completează câmpul chksum
void chksum(record head, char *chk) {
    int s = 0, i;
    // completez câmpul chksum cu ' '
    for (i = 0; i < 8; i++) {
        *(chk + i) = ' ';
        head.header.chksum[i] = ' ';
    }
    char *ptr = (char *) &head.charptr;
    for (i = 0; i < MAX_LINE; i++) {
        s += ptr[i];
    }
    sprintf(chk, "0%o", s);
    chk[6] = '\0';
    chk[7] = ' ';
}

// funcția crează arhiva archive_name care va conține fișierele din
// directorul path_to_dir
void create(char *archive_name, char *path_to_dir) {
    FILE *tar, *users, *ls, *in;
    int i, ok;
    char line[MAX_LINE + 1], *copy, *path;
    record head = {0};
    if (!(copy = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    if (!(path = calloc(MAX_LINE, sizeof(char)))) {
        printf("> Failed!\n");
        exit(1);
    }
    // prin tar vom scrie fișierul binar care va reprezenta arhiva
    if ((tar = fopen(archive_name, "wb")) == NULL) {
        printf("> Failed!\n");
        exit(1);
    }
    // prin ls vom citi informațiile din files.txt
    if ((ls = fopen("files.txt", "rt")) == NULL) {
        printf("> Failed!\n");
        exit(1);
    }
    // parcurgem fiecare linie din files.txt
    // fiecare linie conține informațiile specifice unui fișier
    while (fgets(line, MAX_LINE, ls)) {
        // pentru fiecare fișier din directorul de arhivat
        // vom completa informațiile în header
        name(line, head.header.name);
        permission(line, head.header.mode);
        user(line, head.header.uid, head.header.uname);
        group(line, head.header.gid, head.header.gname);
        size(line, head.header.size);
        timestamp(line, head.header.mtime);
        head.header.typeflag = '0';
        // câmpul linkname conține aceeași informație ca name
        name(line, head.header.linkname);
        strcpy(head.header.magic, "GNUtar \0");
        dev(head.header.devminor, head.header.devmajor);
        chksum(head, head.header.chksum);
        // informațiile din header sunt scrise în fișierul arhivei
        fwrite(&head, sizeof(record), 1, tar);
        // formăm calea spre fișierul din directorul de arhivat
        strcpy(path, path_to_dir);
        strcat(path, head.header.name);
        if ((in = fopen(path, "rb")) == NULL) {
            printf("> Failed!\n");
            exit(1);
        }
        // copiem informația din fișier în arhivă, în continuarea headerului
        do {
            for (i = 0; i < MAX_LINE; i++) {
                line[i] = '\0';
            }
            ok = fread(&line, MAX_LINE, 1, in);
            fwrite(&line, MAX_LINE, 1, tar);
        } while (ok);
        fclose(in);
    }
    // la finalul arhivei mai adăugăm încă un bloc de 512 bytes, conținând '\0'
    for (i = 0; i < MAX_LINE; i++) {
        line[i] = '\0';
    }
    fwrite(&line, MAX_LINE, 1, tar);
    printf("> Done!\n");
    fclose(tar);
    fclose(ls);
    free(copy);
    free(path);
}

// funcția listează fișierele din arhiva archive_name
void list(char *archive_name) {
    FILE *in;
    if ((in = fopen(archive_name, "rb")) == NULL) {
        printf("> File not found!\n");
        exit(1);
    }
    record head;
    int ok, sz, i;
    do {
        // citim informațiile din header
        ok = fread(&head, MAX_LINE, 1, in);
        if (ok) {
            // aflăm dimensiunea fișierului
            sscanf(head.header.size, "%o", &sz);
            if (strcmp(head.header.name, "\0")) {
                // afișăm numele fișierului curent
                printf("> %s\n", head.header.name);
            }
            if (sz % MAX_LINE) {
                sz /= MAX_LINE;
                sz++;
                sz *= MAX_LINE;
            }
            // mutăm cursorul până la următorul fișier
            fseek(in, sz, SEEK_CUR);
        }
    } while (ok);
}

// funcția extrage fișierul file_name din arhiva archive_name
void extract(char *file_name, char *archive_name) {
    FILE *in, *out;
    if ((in = fopen(archive_name, "rb")) == NULL) {
        printf("> File not found!\n");
        exit(1);
    }
    char name[MAX_LINE];
    name[0] = '\0';
    // formăm numele fișierului extras
    strcpy(name, "extracted_");
    strcat(name, file_name);
    if ((out = fopen(name, "wb")) == NULL) {
        exit(1);
    }
    char *block;
    record head;
    int ok, sz, i;
    if (!(block = calloc(MAX_LINE, sizeof(char)))) {
        exit(1);
    }
    do {
        // citim informațiile din header
        ok = fread(&head, MAX_LINE, 1, in);
        if (ok) {
            sscanf(head.header.size, "%o", &sz);
            // copiem informațiile din arhivă care reprezintă conținutul
            // fișierului în fișierul extras
            if (strcmp(head.header.name, file_name) == 0) {
                while (sz >= MAX_LINE) {
                    fread(block, MAX_LINE, 1, in);
                    fwrite(block, MAX_LINE, 1, out);
                    sz -= MAX_LINE;
                }
                fread(block, sz, 1, in);
                fwrite(block, sz, 1, out);
            } else {
                if (sz % MAX_LINE) {
                    sz /= MAX_LINE;
                    sz++;
                    sz *= MAX_LINE;
                }
                fseek(in, sz, SEEK_CUR);
            }
        }
    } while (ok);
    printf("> File extracted!\n");
    free(block);
}

int main() {
    char line[MAX_LINE], *cmd, *archive_name, *file_name, *path_to_dir, *aux;
    int i;
    while (1) {
        fgets(line, MAX_LINE, stdin);
        cmd = strtok(line, " \n");
        // instrucțiunile specifice comenzii "exit"
        if (strcmp(cmd, "exit") == 0) {
            return 0;
        // instrucțiunile specifice comenzii "create"
        } else if (strcmp(cmd, "create") == 0) {
            archive_name = strtok(NULL, " \n");
            path_to_dir = strtok(NULL, " \n");
            aux = strtok(NULL, " \n");
            if (archive_name == NULL || path_to_dir == NULL || aux) {
                printf("> Wrong command!\n");
            } else {
                create(archive_name, path_to_dir);
            }
        // instrucțiunile specifice comenzii "list"
        } else if (strcmp(cmd, "list") == 0) {
            archive_name = strtok(NULL, " \n");
            aux = strtok(NULL, " \n");
            if (archive_name == NULL || aux) {
                printf("> Wrong command!\n");
            } else {
                list(archive_name);
            }
        // instrucțiunile specifice comenzii "extract"
        } else if (strcmp(cmd, "extract") == 0) {
            file_name = strtok(NULL, " \n");
            archive_name = strtok(NULL, " \n");
            aux = strtok(NULL, " \n");
            if (archive_name == NULL || file_name == NULL || aux) {
                printf("> Wrong command!\n");
            } else {
                extract(file_name, archive_name);
            }
        } else {
            printf("> Wrong command!\n");
        }
    }
    return 1;
}
