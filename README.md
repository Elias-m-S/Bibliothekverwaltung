
# Bibliotheksverwaltungssystem

Ein C-Programm zur Verwaltung einer Bibliothek mit Binärbaum-Datenstruktur, Suchfunktionen und Ausleihverwaltung.

## Teil 1: Funktionalitäten

Das Programm bietet folgende Funktionen:

1. **Suche nach Büchern**:
   - Nach Titel (case-insensitiv, Teilstring-Suche)
   - Nach ISBN (case-insensitiv)

2. **Ausleihe und Rückgabe**:
   - Bücher ausleihen (unbegrenzte Exemplare)
   - Bücher zurückgeben
   - Ausleihdauer von 28 Tagen

3. **Ausleihkonto**:
   - Anzeige aller ausgeliehenen Bücher mit Titel, ISBN und restlicher Ausleihdauer
   - Warnhinweise bei überschrittener Ausleihdauer
   - Persistente Speicherung in einer Datei

## Teil 2: Dokumentation

### 1. Datenstruktur für den Buchbestand

Die gewählte Datenstruktur ist ein **Binärer Suchbaum** (BST) mit folgender Definition:

```c
typedef struct Buch {
    char isbn[LEN_ISBN];
    char titel[MAX_TITEL_LAENGE];
    struct Buch* links;
    struct Buch* rechts;
} Buch;
```

**Vorteile:**
- Effiziente Suche nach ISBN (O(log n) im durchschnittlichen Fall)
- Automatische Sortierung der Bücher nach ISBN
- Dynamische Größe, wächst mit der Anzahl der Bücher
- Einfache Implementierung von Einfüge- und Suchoperationen

**Nachteile:**
- Kann bei ungünstiger Einfügereihenfolge degenerieren (worst case O(n))
- Titelsuche muss den gesamten Baum durchlaufen
- Kein direkter Zugriff auf einzelne Elemente wie bei Arrays

**Begründung der Wahl:**
- ISBN-Suche sehr effizient (Hauptanwendungsfall)
- Flexible Größenanpassung möglich
- Gute Balance zwischen Implementierungskomplexität und Effizienz

### 2. Algorithmus zum Einlesen der CSV-Datei

Der Algorithmus liest die CSV-Datei zeilenweise ein und verarbeitet die Daten im Format:
`"isbn","titel"`

### 3. Suchalgorithmen

**ISBN-Suche:**
- Rekursiver Abstieg im BST
- Vergleich der ISBNs zur Navigationsbestimmung
- Direkter Zugriff bei Übereinstimmung

**Titelsuche:**
- Vollständige Baumtraversierung
- Case-insensitiver Teilstring-Vergleich
- Sammlung aller Treffer bis zur maximalen Ergebnisanzahl

### 4. Komplexität der Suchalgorithmen

**ISBN-Suche:**
- Best Case: O(1) - Wurzel enthält gesuchte ISBN
- Average Case: O(log n) - Ausbalancierter BST
- Worst Case: O(n) - Degenerierter BST

**Titelsuche:**
- Best/Average/Worst Case: O(n) - Muss alle Knoten durchsuchen

## Teil 3: Softwaretest

### 1. Testkonzepte

Das Programm verwendet:
- Unit Tests für einzelne Funktionen
- Integrationstests für Zusammenspiel mehrerer Komponenten
- Systematische Testfälle für Grenzwerte
- Separate Testdatei (test_standalone.c)

### 2. Testfälle

Implementierte Testfälle:
1. Ausleihkonto anzeigen (leeres Konto)
2. Titelsuche nach "science" oder "fiction"
3. Ausleihe von gefundenen Büchern
4. Anzeige des aktualisierten Ausleihkontos
5. Rückgabe eines Buches
6. Prüfung des finalen Kontostands

### 3. Test von Anforderung 7 (Persistenz)

Getestet durch:
- Speichern und Laden von Ausleihvorgängen
- Programmneustart mit bestehenden Ausleihdaten
- Prüfung der Datenintegrität nach Neustart
- Validierung der Ausleihdaten und Datumsberechnung

## Kompilieren und Ausführen

### Hauptprogramm
```bash
gcc -o library_app main.c -lm && ./library_app
```

### Testprogramm
```bash
gcc -o library_test test_standalone.c -lm && ./library_test
```
