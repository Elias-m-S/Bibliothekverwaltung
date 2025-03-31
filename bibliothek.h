/**
 * bibliothek.h
 * Enthält die Deklarationen der Funktionen und Datenstrukturen für die Bibliotheksverwaltung
 */

#ifndef BIBLIOTHEK_H
#define BIBLIOTHEK_H

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

/* Funktionen */

// Initialisierung: Daten laden etc.
void init();

// Suchfunktionen
// Parameter:
//    title, isbn: ist jeweils eine Zeichenkette mit dem Suchbegriff
//    isbnListe: eine Liste aus Zeichenketten der ISBNs der gefundenen Bücher
//    maxResults: Maximal anzahl Suchergebnisse die zurückgegeben werden sollen
void nachTitelSuchen(char* titel, char isbnListe[][LEN_ISBN], int maxResults);
void nachIsbnSuchen(char* isbn, char isbnListe[][LEN_ISBN], int maxResults);

// Buch ausleihen
// Parameter: isbn: zeichenkette die auszuleihende isbn enthält
// Rückgabewert: 0 wenn nicht erfolgreich
int buchAusleihen(char* isbn);

// Ausleihkonto anzeigen
int kontoAnzeigen();

// Buch zurückgeben
// Parameter: isbn: zeichenkette die zurückzugebende isbn enthält
// Rückgabewert: 0 wenn nicht erfolgreich
int buchZurueckgeben(char* isbn);

#endif // BIBLIOTHEK_H
