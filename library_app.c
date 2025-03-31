/**
 * library_app.c
 * Hauptprogramm für die Bibliotheksverwaltung mit Benutzerinteraktion
 */

#include "bibliothek.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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