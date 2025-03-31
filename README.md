# Bibliotheksverwaltungssystem

Ein C-Programm zur Verwaltung einer Bibliothek mit Binärbaum-Datenstruktur, Suchfunktionen und Ausleihverwaltung gemäß den vorgegebenen Anforderungen.

## Funktionalitäten

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

## Verwendete Datenstrukturen

- **Binärer Suchbaum** für die effiziente Speicherung und Suche der Bücher
- **Array** für die Verwaltung der ausgeliehenen Bücher

## Dateiübersicht

- `bibliothek.h`: Header-Datei mit Deklarationen der Funktionen und Datenstrukturen
- `bibliothek.c`: Implementierung der Funktionen für die Bibliotheksverwaltung
- `library_app.c`: Hauptprogramm mit Benutzerinteraktion
- `test.c`: Testprogramm, das die grundlegenden Funktionen testet
- `books.csv`: CSV-Datei mit den Bücherdaten (ISBN, Titel)
- `ausleihen.txt`: Datei zum Speichern der Ausleihvorgänge

## Kompilieren und Ausführen

### Hauptprogramm

```
gcc -o library_app bibliothek.c library_app.c -lm && ./library_app
```

### Testprogramm

```
gcc -o library_test bibliothek.c test.c -lm && ./library_test
```

## Menüführung

Das Hauptprogramm bietet ein Menü mit folgenden Optionen:

1. Nach Büchern suchen (Titel)
2. Nach Büchern suchen (ISBN)
3. Buch ausleihen
4. Buch zurückgeben
5. Ausleihkonto anzeigen
6. Programm beenden

## Benutzte Programmiertechniken

- Dynamische Speicherverwaltung mit Zeigern
- Rekursive Funktionen für Baumoperationen
- Strukturen zur Datenspeicherung
- Dateioperationen zum Speichern und Laden von Daten
- Case-insensitive Stringverarbeitung
- Binärbaumtraversierung (Inorder, für die Suche)