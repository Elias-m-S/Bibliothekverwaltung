/**
 * test.c
 * Test-Programm für die Bibliotheksverwaltung
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funktionsprototypen aus main.c
void init();
void nachTitelSuchen(char* titel, char isbnListe[][14], int maxResults);
void nachIsbnSuchen(char* isbn, char isbnListe[][14], int maxResults);
int buchAusleihen(char* isbn);
int kontoAnzeigen();
int buchZurueckgeben(char* isbn);

#define LEN_ISBN 14
#define MAXRESULTS 10

int main()
{
    init();
    
    char results[MAXRESULTS][LEN_ISBN];
    
    printf("\n\n--- Bibliothek Verwaltungssystem Test ---\n\n");
    
    // Ausleihkonto anzeigen
    printf("1. Ausleihkonto anzeigen:\n");
    kontoAnzeigen();
    
    // Nach Titel suchen
    printf("\n2. Nach Titel 'science' suchen:\n");
    nachTitelSuchen("science", results, MAXRESULTS);
    
    // Wenn keine Bücher mit "science" gefunden, nach "fiction" suchen
    if (results[0][0] == '\0') {
        printf("\nAlternativ nach 'fiction' suchen:\n");
        nachTitelSuchen("fiction", results, MAXRESULTS);
    }
    
    // Wenn immer noch keine Bücher gefunden, nach leeren String suchen (alle Bücher)
    if (results[0][0] == '\0') {
        printf("\nZeige alle verfügbaren Bücher:\n");
        nachTitelSuchen("", results, MAXRESULTS);
    }
    
    // Wenn Bücher gefunden wurden, zwei davon ausleihen
    if (results[0][0] != '\0') {
        printf("\n3. Bücher ausleihen:\n");
        buchAusleihen(results[0]);
        
        if (results[1][0] != '\0') {
            buchAusleihen(results[1]);
        }
        
        // Ausleihkonto anzeigen
        printf("\n4. Ausleihkonto nach dem Ausleihen:\n");
        kontoAnzeigen();
        
        // Ein Buch zurückgeben
        printf("\n5. Ein Buch zurückgeben:\n");
        buchZurueckgeben(results[0]);
        
        // Ausleihkonto nach Rückgabe anzeigen
        printf("\n6. Ausleihkonto nach der Rückgabe:\n");
        kontoAnzeigen();
    }
    
    printf("\n--- Test abgeschlossen ---\n\n");
    
    return 0;
}
