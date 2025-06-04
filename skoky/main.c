#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VSTUPNI_SOUBOR "SKOKY.TXT"
#define MAX_RADKA 200

typedef struct {
    char jmeno[20];
    char prijmeni[30];
    char datumNarozeni[11];  // DD.MM.RRRR
    char stat[4];
    int delkaSkoku;
    char tym[4];             // "ANO"/"NE"
} ZAVODNIK;

int nactiSoubor(ZAVODNIK **zavodnici) {
    FILE *f = fopen(VSTUPNI_SOUBOR, "r");
    if (!f) {
        printf("Chyba pri otevirani souboru %s.\n", VSTUPNI_SOUBOR);
        return 0;
    }

    char radek[MAX_RADKA];
    int pocet = 0;
    *zavodnici = NULL;

    while (fgets(radek, MAX_RADKA, f)) {
        if (pocet == 0) {  // preskocime hlavicku
            pocet++;
            continue;
        }

        radek[strcspn(radek, "\r\n")] = '\0';

        ZAVODNIK z;
        char *token = strtok(radek, ";");
        if (!token) continue;
        strcpy(z.jmeno, token);

        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(z.prijmeni, token);

        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(z.datumNarozeni, token);

        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(z.stat, token);

        token = strtok(NULL, ";");
        if (!token) continue;
        z.delkaSkoku = atoi(token);

        token = strtok(NULL, ";");
        if (!token) continue;
        strcpy(z.tym, token);

        ZAVODNIK *tmp = realloc(*zavodnici, (pocet - 1 + 1) * sizeof(ZAVODNIK));
        if (!tmp) {
            printf("Chyba alokace pameti.\n");
            free(*zavodnici);
            fclose(f);
            return 0;
        }

        *zavodnici = tmp;
        (*zavodnici)[pocet - 1] = z;
        pocet++;
    }

    fclose(f);
    return pocet - 1;
}

void vypisStatistiky(ZAVODNIK *z, int n) {
    int vTymu = 0, bezTymu = 0;
    int maxDelka = 0;
    char nejJmeno[20], nejPrijmeni[30], nejStat[4];
    int soucetD = 0;

    for (int i = 0; i < n; i++) {
        if (strcmp(z[i].tym, "ANO") == 0) {
            vTymu++;
            soucetD += z[i].delkaSkoku;
        } else {
            bezTymu++;
        }

        if (z[i].delkaSkoku > maxDelka) {
            maxDelka = z[i].delkaSkoku;
            strcpy(nejJmeno, z[i].jmeno);
            strcpy(nejPrijmeni, z[i].prijmeni);
            strcpy(nejStat, z[i].stat);
        }
    }

    printf("Pocet zavodniku: %d\n", n);
    printf("Pocet zavodniku nezarazenych v tymech: %d\n", bezTymu);
    printf("Nejdelsi skok: %d m (%s %s z %s)\n", maxDelka, nejJmeno, nejPrijmeni, nejStat);
    if (vTymu > 0) {
        printf("Prumerna delka skoku zavodniku v tymech: %.2f m\n\n", (float)soucetD / vTymu);
    }
}

void ulozCesi(ZAVODNIK *z, int n) {
    FILE *f = fopen("skoky.html", "w");
    if (!f) {
        printf("Nelze otevrit vystupni soubor skoky.html\n");
        return;
    }

    fprintf(f, "<!DOCTYPE html>\n<html lang=\"cs\">\n<head>\n");
    fprintf(f, "<meta charset=\"UTF-8\">\n<title>Vysledky - Cesi</title>\n");
    fprintf(f, "<style>\n");
    fprintf(f, "body { font-family: Arial, sans-serif; }\n");
    fprintf(f, "table { border-collapse: collapse; width: 80%%; margin: 20px auto; }\n");
    fprintf(f, "th, td { border: 1px solid #444; padding: 8px; text-align: center; }\n");
    fprintf(f, "th { background-color: #f2f2f2; }\n");
    fprintf(f, "caption { font-size: 1.5em; margin-bottom: 10px; }\n");
    fprintf(f, "</style>\n</head>\n<body>\n");

    fprintf(f, "<table>\n<caption>Tym Ceske republiky</caption>\n");
    fprintf(f, "<tr><th>Poradi</th><th>Jmeno</th><th>Prijmeni</th><th>Datum narozeni</th><th>Delka skoku</th></tr>\n");

    int poradi = 1;
    for (int i = 0; i < n; i++) {
        if (strcmp(z[i].stat, "CZE") == 0 && strcmp(z[i].tym, "ANO") == 0) {
            int d, m, r;
            sscanf(z[i].datumNarozeni, "%d.%d.%d", &d, &m, &r);
            fprintf(f, "<tr><td>%d.</td><td>%s</td><td>%s</td><td>%04d-%02d-%02d</td><td>%d m</td></tr>\n",
                    poradi++, z[i].jmeno, z[i].prijmeni, r, m, d, z[i].delkaSkoku);
        }
    }

    fprintf(f, "</table>\n</body>\n</html>\n");

    fclose(f);
    printf("Byl vytvoren HTML soubor skoky.html.\n");
}

int main() {
    ZAVODNIK *zavodnici;
    int pocet = nactiSoubor(&zavodnici);
    if (pocet == 0) return 1;

    vypisStatistiky(zavodnici, pocet);
    ulozCesi(zavodnici, pocet);

    free(zavodnici);
    return 0;
}
