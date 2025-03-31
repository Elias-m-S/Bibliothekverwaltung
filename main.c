/**
 * main.c
 * Hauptprogramm für die Bibliotheksverwaltung mit Benutzerinteraktion
 * (Enthält alle Funktionen, die zuvor auf bibliothek.h und bibliothek.c aufgeteilt waren)
 * 
 * Bei der Kompilierung für Tests wird die main-Funktion durch die Definition des
 * Makros TEST_MODE auskommentiert, damit die test.c ihre eigene main-Funktion verwenden kann.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* Konstanten */
#define LEN_ISBN 14          // Maximale Länge einer ISBN
#define MAXRESULTS 10        // Maximale Anzahl von Suchergebnissen
#define MAX_TITEL_LAENGE 256 // Maximale Länge eines Buchtitels
#define MAX_AUSLEIHEN 100    // Maximale Anzahl von ausgeliehenen Büchern
#define AUSLEIHDAUER 28      // Ausleihdauer in Tagen
#define CSV_DATEI "attached_assets/books.csv" // Pfad zur CSV-Datei
#define AUSLEIHDATEI "ausleihen.txt"          // Datei zum Speichern der Ausleihvorgänge

/* Datenstrukturen */

// Struktur für ein Buch
typedef struct Buch {
    char isbn[LEN_ISBN];        // ISBN des Buches
    char titel[MAX_TITEL_LAENGE]; // Titel des Buches
    struct Buch* links;         // Zeiger auf linken Teilbaum (kleinere ISBN)
    struct Buch* rechts;        // Zeiger auf rechten Teilbaum (größere ISBN)
} Buch;

// Struktur für ein ausgeliehenes Buch
typedef struct Ausleihe {
    char isbn[LEN_ISBN];          // ISBN des ausgeliehenen Buches
    char titel[MAX_TITEL_LAENGE]; // Titel des ausgeliehenen Buches
    time_t ausleihDatum;         // Datum der Ausleihe
} Ausleihe;

/* Globale Variablen */
Buch* wurzel = NULL;                // Wurzel des Binärbaums
Ausleihe ausleihen[MAX_AUSLEIHEN];  // Array der ausgeliehenen Bücher
int anzahlAusleihen = 0;            // Anzahl der aktuellen Ausleihen

/* Funktionsprototypen */
void init();
void nachTitelSuchen(char* titel, char isbnListe[][LEN_ISBN], int maxResults);
void nachIsbnSuchen(char* isbn, char isbnListe[][LEN_ISBN], int maxResults);
int buchAusleihen(char* isbn);
int kontoAnzeigen();
int buchZurueckgeben(char* isbn);

/* Hilfsfunktionen Prototypen */
Buch* neuesBuchErstellen(const char* isbn, const char* titel);
Buch* buchEinfuegen(Buch* wurzelKnoten, const char* isbn, const char* titel);
Buch* buchNachIsbnSuchen(Buch* wurzelKnoten, const char* isbn);
void zuKleinbuchstaben(char* str);
int enthältTeilstring(const char* quelle, const char* teilstring);
int büchernachTitelSuchenRekursiv(Buch* wurzelKnoten, const char* titel, char isbnListe[][LEN_ISBN], int maxResults, int aktuelleAnzahl);
void csvDateiEinlesen();
void ausleihenLaden();
void ausleihenSpeichern();
void eingabePufferLeeren();
int menüAnzeigen();

#ifndef TEST_MODE

/* Hilfsfunktionen für Binärbaum-Operationen */

/**
 * Erstellt ein neues Buch mit den gegebenen Daten
 * @param isbn ISBN des Buches
 * @param titel Titel des Buches
 * @return Zeiger auf das neue Buch oder NULL bei Fehler
 */
Buch* neuesBuchErstellen(const char* isbn, const char* titel) {
    // Speicher für das neue Buch reservieren
    Buch* neuesBuch = (Buch*)malloc(sizeof(Buch));
    
    // Prüfen, ob die Speicherreservierung erfolgreich war
    if (neuesBuch == NULL) {
        printf("Fehler bei der Speicherreservierung!\n");
        return NULL;
    }
    
    // Daten in das neue Buch kopieren
    strncpy(neuesBuch->isbn, isbn, LEN_ISBN - 1);
    neuesBuch->isbn[LEN_ISBN - 1] = '\0'; // Sicherstellen, dass die ISBN nullterminiert ist
    
    strncpy(neuesBuch->titel, titel, MAX_TITEL_LAENGE - 1);
    neuesBuch->titel[MAX_TITEL_LAENGE - 1] = '\0'; // Sicherstellen, dass der Titel nullterminiert ist
    
    // Teilbäume initialisieren
    neuesBuch->links = NULL;
    neuesBuch->rechts = NULL;
    
    return neuesBuch;
}

/**
 * Fügt ein Buch in den Binärbaum ein
 * @param wurzelKnoten Zeiger auf die Wurzel des Baums
 * @param isbn ISBN des einzufügenden Buches
 * @param titel Titel des einzufügenden Buches
 * @return Zeiger auf die (möglicherweise neue) Wurzel des Baums
 */
Buch* buchEinfuegen(Buch* wurzelKnoten, const char* isbn, const char* titel) {
    // Wenn der Baum leer ist, neues Buch als Wurzel einfügen
    if (wurzelKnoten == NULL) {
        return neuesBuchErstellen(isbn, titel);
    }
    
    // Vergleichen der ISBNs, um die richtige Position im Baum zu finden
    int vergleich = strcmp(isbn, wurzelKnoten->isbn);
    
    // Wenn die ISBN kleiner ist, im linken Teilbaum einfügen
    if (vergleich < 0) {
        wurzelKnoten->links = buchEinfuegen(wurzelKnoten->links, isbn, titel);
    }
    // Wenn die ISBN größer ist, im rechten Teilbaum einfügen
    else if (vergleich > 0) {
        wurzelKnoten->rechts = buchEinfuegen(wurzelKnoten->rechts, isbn, titel);
    }
    // Wenn die ISBN bereits existiert, nichts tun (Duplikate vermeiden)
    // Alternativ könnte man hier den Titel aktualisieren, falls sich dieser geändert hat
    
    return wurzelKnoten;
}

/**
 * Sucht ein Buch im Binärbaum anhand der ISBN
 * @param wurzelKnoten Wurzel des (Teil-)Baums, in dem gesucht wird
 * @param isbn Zu suchende ISBN
 * @return Zeiger auf das gefundene Buch oder NULL, wenn nicht gefunden
 */
Buch* buchNachIsbnSuchen(Buch* wurzelKnoten, const char* isbn) {
    // Wenn der Baum leer ist oder wir am Ende eines Pfades sind, Buch nicht gefunden
    if (wurzelKnoten == NULL) {
        return NULL;
    }
    
    // ISBNs vergleichen
    int vergleich = strcmp(isbn, wurzelKnoten->isbn);
    
    // Wenn die gesuchte ISBN kleiner ist, im linken Teilbaum suchen
    if (vergleich < 0) {
        return buchNachIsbnSuchen(wurzelKnoten->links, isbn);
    }
    // Wenn die gesuchte ISBN größer ist, im rechten Teilbaum suchen
    else if (vergleich > 0) {
        return buchNachIsbnSuchen(wurzelKnoten->rechts, isbn);
    }
    // Wenn die ISBNs übereinstimmen, Buch gefunden
    else {
        return wurzelKnoten;
    }
}

/**
 * Konvertiert einen String zu Kleinbuchstaben
 * @param str Der zu konvertierende String
 */
void zuKleinbuchstaben(char* str) {
    // Jeden Buchstaben zu Kleinbuchstaben konvertieren
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

/**
 * Prüft, ob ein Teilstring in einem String enthalten ist (case-insensitiv)
 * @param quelle Der zu durchsuchende String
 * @param teilstring Der zu suchende Teilstring
 * @return 1 wenn der Teilstring gefunden wurde, sonst 0
 */
int enthältTeilstring(const char* quelle, const char* teilstring) {
    // Wenn der Teilstring leer ist, geben wir true zurück - leerer Suchbegriff passt zu allem
    if (teilstring == NULL || teilstring[0] == '\0') {
        return 1;
    }
    
    // Wenn die Quelle leer ist, geben wir false zurück
    if (quelle == NULL || quelle[0] == '\0') {
        return 0;
    }
    
    // Kopien der Strings erstellen, um sie zu Kleinbuchstaben zu konvertieren
    char quelleKlein[MAX_TITEL_LAENGE];
    char teilstringKlein[MAX_TITEL_LAENGE];
    
    // Strings kopieren
    strncpy(quelleKlein, quelle, MAX_TITEL_LAENGE - 1);
    quelleKlein[MAX_TITEL_LAENGE - 1] = '\0';
    
    strncpy(teilstringKlein, teilstring, MAX_TITEL_LAENGE - 1);
    teilstringKlein[MAX_TITEL_LAENGE - 1] = '\0';
    
    // Zu Kleinbuchstaben konvertieren
    zuKleinbuchstaben(quelleKlein);
    zuKleinbuchstaben(teilstringKlein);
    
    // Prüfen, ob der Teilstring in der Quelle enthalten ist
    return strstr(quelleKlein, teilstringKlein) != NULL;
}

/**
 * Rekursive Suche nach Büchern mit einem bestimmten Titel (case-insensitiv)
 * @param wurzelKnoten Wurzel des zu durchsuchenden (Teil-)Baums
 * @param titel Zu suchender Titel (Teil des Titels)
 * @param gefundeneISBNs Array, das die ISBNs der gefundenen Bücher aufnimmt
 * @param maxResults Maximale Anzahl zurückzugebender Ergebnisse
 * @param aktuelleAnzahl Aktuelle Anzahl gefundener Bücher
 * @return Aktualisierte Anzahl gefundener Bücher
 */
int büchernachTitelSuchenRekursiv(Buch* wurzelKnoten, const char* titel, char isbnListe[][LEN_ISBN], int maxResults, int aktuelleAnzahl) {
    // Wenn der Baum leer ist oder die maximale Anzahl erreicht wurde, beenden
    if (wurzelKnoten == NULL || aktuelleAnzahl >= maxResults) {
        return aktuelleAnzahl;
    }
    
    // Zuerst im linken Teilbaum suchen
    aktuelleAnzahl = büchernachTitelSuchenRekursiv(wurzelKnoten->links, titel, isbnListe, maxResults, aktuelleAnzahl);
    
    // Prüfen, ob der aktuelle Knoten dem Suchkriterium entspricht
    if (aktuelleAnzahl < maxResults && enthältTeilstring(wurzelKnoten->titel, titel)) {
        // Wenn ja, ISBN in die Liste aufnehmen
        strncpy(isbnListe[aktuelleAnzahl], wurzelKnoten->isbn, LEN_ISBN - 1);
        isbnListe[aktuelleAnzahl][LEN_ISBN - 1] = '\0';
        aktuelleAnzahl++;
    }
    
    // Dann im rechten Teilbaum suchen
    aktuelleAnzahl = büchernachTitelSuchenRekursiv(wurzelKnoten->rechts, titel, isbnListe, maxResults, aktuelleAnzahl);
    
    return aktuelleAnzahl;
}

/**
 * Liest die CSV-Datei ein und baut den Binärbaum auf
 */
void csvDateiEinlesen() {
    FILE* datei = fopen(CSV_DATEI, "r");
    
    // Prüfen, ob die Datei geöffnet werden konnte
    if (datei == NULL) {
        printf("Fehler beim Öffnen der CSV-Datei: %s\n", CSV_DATEI);
        return;
    }
    
    char zeile[1024];  // Puffer für eine Zeile aus der CSV-Datei
    
    // Die erste Zeile (Header) überspringen
    if (fgets(zeile, sizeof(zeile), datei) == NULL) {
        printf("CSV-Datei ist leer oder fehlerhaft\n");
        fclose(datei);
        return;
    }

    int anzahlBücher = 0; // Zähler für erfolgreiche Einträge
    
    // Zeile für Zeile einlesen
    while (fgets(zeile, sizeof(zeile), datei) != NULL) {
        char isbn[LEN_ISBN] = "";
        char titel[MAX_TITEL_LAENGE] = "";
        
        // CSV-Format verarbeiten: "isbn","titel",...
        // Die ISBN ist das erste Feld, der Titel das zweite
        
        // Verbesserter CSV-Parser zur korrekten Extraktion der Felder
        char* pos = zeile;
        int feldNummer = 0;
        int inAnführungszeichen = 0;
        
        char tempFeld[MAX_TITEL_LAENGE] = ""; // Temporärer Puffer für das aktuelle Feld
        int tempIndex = 0;
        
        // Zeichen für Zeichen verarbeiten
        for (int i = 0; zeile[i] != '\0' && zeile[i] != '\n'; i++) {
            char c = zeile[i];
            
            if (c == '"') {
                // Anfang oder Ende von Anführungszeichen
                inAnführungszeichen = !inAnführungszeichen;
                
                // Wenn Anführungszeichen geschlossen werden, feld könnte vollständig sein
                if (!inAnführungszeichen) {
                    tempFeld[tempIndex] = '\0';
                    
                    if (feldNummer == 0) {
                        // ISBN speichern
                        strncpy(isbn, tempFeld, LEN_ISBN - 1);
                        isbn[LEN_ISBN - 1] = '\0';
                    } else if (feldNummer == 1) {
                        // Titel speichern
                        strncpy(titel, tempFeld, MAX_TITEL_LAENGE - 1);
                        titel[MAX_TITEL_LAENGE - 1] = '\0';
                        
                        // Nach dem Titel können wir aufhören, wir benötigen keine weiteren Felder
                        break;
                    }
                    
                    // Nächstes Feld vorbereiten
                    feldNummer++;
                    tempIndex = 0;
                    tempFeld[0] = '\0';
                }
            } else if (inAnführungszeichen) {
                // Zeichen innerhalb von Anführungszeichen zum temporären Feld hinzufügen
                if (tempIndex < MAX_TITEL_LAENGE - 1) {
                    tempFeld[tempIndex++] = c;
                }
            }
        }
        
        // Buch in den Binärbaum einfügen, wenn ISBN und Titel nicht leer sind
        if (strlen(isbn) > 0 && strlen(titel) > 0) {
            wurzel = buchEinfuegen(wurzel, isbn, titel);
            anzahlBücher++;
        }
    }
    
    printf("Bibliotheksdaten geladen: %d Bücher eingelesen.\n", anzahlBücher);
    fclose(datei);
}

/**
 * Lädt die Ausleihdaten aus der Datei
 */
void ausleihenLaden() {
    FILE* datei = fopen(AUSLEIHDATEI, "r");
    
    // Wenn die Datei nicht existiert, gibt es noch keine Ausleihen
    if (datei == NULL) {
        return;
    }
    
    // Ausleihen einlesen
    anzahlAusleihen = 0;
    
    while (anzahlAusleihen < MAX_AUSLEIHEN && 
            fscanf(datei, "%13s\n", ausleihen[anzahlAusleihen].isbn) == 1) {
        
        // Titel aus dem Binärbaum abrufen
        Buch* buch = buchNachIsbnSuchen(wurzel, ausleihen[anzahlAusleihen].isbn);
        if (buch != NULL) {
            strncpy(ausleihen[anzahlAusleihen].titel, buch->titel, MAX_TITEL_LAENGE - 1);
            ausleihen[anzahlAusleihen].titel[MAX_TITEL_LAENGE - 1] = '\0';
        } else {
            strcpy(ausleihen[anzahlAusleihen].titel, "Unbekanntes Buch");
        }
        
        // Ausleihdatum einlesen
        fscanf(datei, "%ld\n", &ausleihen[anzahlAusleihen].ausleihDatum);
        
        anzahlAusleihen++;
    }
    
    fclose(datei);
}

/**
 * Speichert die Ausleihdaten in der Datei
 */
void ausleihenSpeichern() {
    FILE* datei = fopen(AUSLEIHDATEI, "w");
    
    // Prüfen, ob die Datei geöffnet werden konnte
    if (datei == NULL) {
        printf("Fehler beim Öffnen der Ausleihdatei zum Schreiben!\n");
        return;
    }
    
    // Ausleihen speichern
    for (int i = 0; i < anzahlAusleihen; i++) {
        fprintf(datei, "%s\n%ld\n", ausleihen[i].isbn, ausleihen[i].ausleihDatum);
    }
    
    fclose(datei);
}

/* Implementierung der öffentlichen Funktionen */

/**
 * Initialisiert die Bibliothek: Lädt die Bücher und Ausleihdaten
 */
void init() {
    // Bücher aus CSV-Datei laden
    csvDateiEinlesen();
    
    // Ausleihdaten laden
    ausleihenLaden();
}

/**
 * Sucht nach Büchern mit einem bestimmten Titel
 * @param titel Zu suchender Titel (Teil des Titels)
 * @param isbnListe Array, das die ISBNs der gefundenen Bücher aufnehmen soll
 * @param maxResults Maximale Anzahl zurückzugebender Ergebnisse
 */
void nachTitelSuchen(char* titel, char isbnListe[][LEN_ISBN], int maxResults) {
    // Alle Einträge in der ISBN-Liste auf leere Strings setzen
    for (int i = 0; i < maxResults; i++) {
        isbnListe[i][0] = '\0';
    }
    
    // Rekursive Suche im Binärbaum durchführen
    int gefunden = büchernachTitelSuchenRekursiv(wurzel, titel, isbnListe, maxResults, 0);
    
    // Ausgabe, wie viele Bücher gefunden wurden
    printf("Es wurden %d Bücher mit dem Titel '%s' gefunden.\n", gefunden, titel);
    for (int i = 0; i < gefunden; i++) {
        // Buch im Binärbaum finden, um den kompletten Titel auszugeben
        Buch* buch = buchNachIsbnSuchen(wurzel, isbnListe[i]);
        if (buch != NULL) {
            printf("%d. ISBN: %s, Titel: %s\n", i + 1, isbnListe[i], buch->titel);
        }
    }
}

/**
 * Sucht nach einem Buch mit einer bestimmten ISBN
 * @param isbn Zu suchende ISBN
 * @param isbnListe Array, das die ISBN des gefundenen Buches aufnehmen soll
 * @param maxResults Maximale Anzahl zurückzugebender Ergebnisse (hier immer 1)
 */
void nachIsbnSuchen(char* isbn, char isbnListe[][LEN_ISBN], int maxResults) {
    // Alle Einträge in der ISBN-Liste auf leere Strings setzen
    for (int i = 0; i < maxResults; i++) {
        isbnListe[i][0] = '\0';
    }
    
    // Buch im Binärbaum suchen
    Buch* buch = buchNachIsbnSuchen(wurzel, isbn);
    
    // Wenn ein Buch gefunden wurde, die ISBN in die Liste eintragen
    if (buch != NULL) {
        strncpy(isbnListe[0], buch->isbn, LEN_ISBN - 1);
        isbnListe[0][LEN_ISBN - 1] = '\0';
        printf("Buch gefunden: ISBN: %s, Titel: %s\n", buch->isbn, buch->titel);
    } else {
        printf("Kein Buch mit der ISBN '%s' gefunden.\n", isbn);
    }
}

/**
 * Leiht ein Buch aus
 * @param isbn ISBN des auszuleihenden Buches
 * @return 1 bei Erfolg, 0 bei Misserfolg
 */
int buchAusleihen(char* isbn) {
    // Prüfen, ob das Buch existiert
    Buch* buch = buchNachIsbnSuchen(wurzel, isbn);
    if (buch == NULL) {
        printf("Fehler: Das Buch mit der ISBN '%s' existiert nicht!\n", isbn);
        return 0;
    }
    
    // Prüfen, ob noch Platz für weitere Ausleihen ist
    if (anzahlAusleihen >= MAX_AUSLEIHEN) {
        printf("Fehler: Maximale Anzahl an Ausleihen erreicht!\n");
        return 0;
    }
    
    // Prüfen, ob das Buch bereits ausgeliehen ist (obwohl laut Aufgabe unbegrenzt viele Exemplare vorhanden sind)
    for (int i = 0; i < anzahlAusleihen; i++) {
        if (strcmp(ausleihen[i].isbn, isbn) == 0) {
            printf("Hinweis: Das Buch ist bereits ausgeliehen, aber da unbegrenzt viele Exemplare vorhanden sind, wird es erneut ausgeliehen.\n");
            break;
        }
    }
    
    // Buch ausleihen
    strncpy(ausleihen[anzahlAusleihen].isbn, isbn, LEN_ISBN - 1);
    ausleihen[anzahlAusleihen].isbn[LEN_ISBN - 1] = '\0';
    
    strncpy(ausleihen[anzahlAusleihen].titel, buch->titel, MAX_TITEL_LAENGE - 1);
    ausleihen[anzahlAusleihen].titel[MAX_TITEL_LAENGE - 1] = '\0';
    
    // Aktuelles Datum als Ausleihdatum festlegen
    ausleihen[anzahlAusleihen].ausleihDatum = time(NULL);
    
    anzahlAusleihen++;
    
    // Ausleihdaten speichern
    ausleihenSpeichern();
    
    printf("Buch '%s' erfolgreich ausgeliehen.\n", buch->titel);
    return 1;
}

/**
 * Zeigt das Ausleihkonto an
 * @return Anzahl der ausgeliehenen Bücher
 */
int kontoAnzeigen() {
    // Aktuelle Zeit für Vergleich mit Ausleihdatum
    time_t jetzt = time(NULL);
    
    printf("\n=== Ihr Ausleihkonto ===\n");
    printf("Anzahl ausgeliehener Bücher: %d\n\n", anzahlAusleihen);
    
    if (anzahlAusleihen == 0) {
        printf("Sie haben derzeit keine Bücher ausgeliehen.\n");
    } else {
        printf("%-14s %-50s %-20s\n", "ISBN", "Titel", "Restliche Ausleihdauer");
        printf("---------------------------------------------------------------------------------\n");
        
        for (int i = 0; i < anzahlAusleihen; i++) {
            // Berechnen der verstrichenen Zeit seit der Ausleihe in Tagen
            double verstricheneTage = difftime(jetzt, ausleihen[i].ausleihDatum) / (60 * 60 * 24);
            
            // Berechnen der verbleibenden Ausleihdauer
            int restlicheTage = AUSLEIHDAUER - (int)verstricheneTage;
            
            // Anzeigen der Buch-Informationen
            printf("%-14s %-50s ", ausleihen[i].isbn, ausleihen[i].titel);
            
            // Warnung, wenn die Ausleihdauer überschritten wurde
            if (restlicheTage < 0) {
                printf("ÜBERFÄLLIG seit %d Tagen!\n", -restlicheTage);
            } else {
                printf("Noch %d Tage\n", restlicheTage);
            }
        }
    }
    
    printf("\n======================\n");
    
    return anzahlAusleihen;
}

/**
 * Gibt ein ausgeliehenes Buch zurück
 * @param isbn ISBN des zurückzugebenden Buches
 * @return 1 bei Erfolg, 0 bei Misserfolg
 */
int buchZurueckgeben(char* isbn) {
    // Index des zurückzugebenden Buches suchen
    int index = -1;
    for (int i = 0; i < anzahlAusleihen; i++) {
        if (strcmp(ausleihen[i].isbn, isbn) == 0) {
            index = i;
            break;
        }
    }
    
    // Wenn das Buch nicht gefunden wurde
    if (index == -1) {
        printf("Fehler: Das Buch mit der ISBN '%s' ist nicht ausgeliehen!\n", isbn);
        return 0;
    }
    
    // Buchname für die Ausgabemeldung speichern
    char titelKopie[MAX_TITEL_LAENGE];
    strncpy(titelKopie, ausleihen[index].titel, MAX_TITEL_LAENGE - 1);
    titelKopie[MAX_TITEL_LAENGE - 1] = '\0';
    
    // Die Ausleihe an der gefundenen Position durch die letzte Ausleihe ersetzen
    if (index < anzahlAusleihen - 1) {
        ausleihen[index] = ausleihen[anzahlAusleihen - 1];
    }
    
    // Anzahl der Ausleihen verringern
    anzahlAusleihen--;
    
    // Ausleihdaten speichern
    ausleihenSpeichern();
    
    printf("Buch '%s' erfolgreich zurückgegeben.\n", titelKopie);
    return 1;
}

/**
 * Löscht den Eingabepuffer
 */
void eingabePufferLeeren() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Zeigt das Hauptmenü an und gibt die ausgewählte Option zurück
 * @return ausgewählte Menüoption
 */
int menüAnzeigen() {
    int auswahl = 0;
    
    printf("\n========== Bibliotheksverwaltung ==========\n");
    printf("1. Nach Büchern suchen (Titel)\n");
    printf("2. Nach Büchern suchen (ISBN)\n");
    printf("3. Buch ausleihen\n");
    printf("4. Buch zurückgeben\n");
    printf("5. Ausleihkonto anzeigen\n");
    printf("6. Programm beenden\n");
    printf("==========================================\n");
    printf("Bitte wählen Sie eine Option (1-6): ");
    
    scanf("%d", &auswahl);
    eingabePufferLeeren();
    
    return auswahl;
}

#endif /* Endet den Bereich der im Testmodus auskommentiert wird */

/**
 * Hauptfunktion des Programms
 */
int main() {
    // Initialisierung
    init();
    
    int auswahl = 0;
    char eingabe[256];
    char results[MAXRESULTS][LEN_ISBN];
    
    // Hauptschleife
    do {
        auswahl = menüAnzeigen();
        
        switch (auswahl) {
            case 1: // Nach Titel suchen
                printf("\nBitte geben Sie den Titel oder einen Teil davon ein: ");
                fgets(eingabe, sizeof(eingabe), stdin);
                // Newline am Ende entfernen
                eingabe[strcspn(eingabe, "\n")] = 0;
                
                nachTitelSuchen(eingabe, results, MAXRESULTS);
                
                // Warten auf Benutzer, bevor es weitergeht
                printf("\nDrücken Sie Enter, um fortzufahren...");
                getchar();
                break;
                
            case 2: // Nach ISBN suchen
                printf("\nBitte geben Sie die ISBN ein: ");
                fgets(eingabe, sizeof(eingabe), stdin);
                // Newline am Ende entfernen
                eingabe[strcspn(eingabe, "\n")] = 0;
                
                nachIsbnSuchen(eingabe, results, MAXRESULTS);
                
                // Warten auf Benutzer, bevor es weitergeht
                printf("\nDrücken Sie Enter, um fortzufahren...");
                getchar();
                break;
                
            case 3: // Buch ausleihen
                printf("\nBitte geben Sie die ISBN des Buches ein, das Sie ausleihen möchten: ");
                fgets(eingabe, sizeof(eingabe), stdin);
                // Newline am Ende entfernen
                eingabe[strcspn(eingabe, "\n")] = 0;
                
                buchAusleihen(eingabe);
                
                // Warten auf Benutzer, bevor es weitergeht
                printf("\nDrücken Sie Enter, um fortzufahren...");
                getchar();
                break;
                
            case 4: // Buch zurückgeben
                printf("\nBitte geben Sie die ISBN des Buches ein, das Sie zurückgeben möchten: ");
                fgets(eingabe, sizeof(eingabe), stdin);
                // Newline am Ende entfernen
                eingabe[strcspn(eingabe, "\n")] = 0;
                
                buchZurueckgeben(eingabe);
                
                // Warten auf Benutzer, bevor es weitergeht
                printf("\nDrücken Sie Enter, um fortzufahren...");
                getchar();
                break;
                
            case 5: // Ausleihkonto anzeigen
                kontoAnzeigen();
                
                // Warten auf Benutzer, bevor es weitergeht
                printf("\nDrücken Sie Enter, um fortzufahren...");
                getchar();
                break;
                
            case 6: // Programm beenden
                printf("\nVielen Dank für die Nutzung der Bibliotheksverwaltung. Auf Wiedersehen!\n");
                break;
                
            default:
                printf("\nUngültige Eingabe. Bitte wählen Sie eine Option zwischen 1 und 6.\n");
                printf("\nDrücken Sie Enter, um fortzufahren...");
                getchar();
        }
        
    } while (auswahl != 6);
    
    return 0;
}