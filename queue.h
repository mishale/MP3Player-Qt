#ifndef QUEUE_H
#define QUEUE_H

#include "song.h"
#include "playlist.h"

class Queue {
private:
    struct Node {
        Song* song;
        Node* next;
        Node* prev;

        Node(Song* song) : song(song), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;
    Node* current;

public:
    Queue();
    ~Queue();

    void addSong(Song* song); // Song zur Warteschlange hinzufügen
    void forwards(); // nächster Song in Warteschlange
    void backwards(); // vorheriger Song
    Song* getCurrentSong() const; // jetziger Song
    void clear(); // Warteschlange leeren
    bool isAtStart() const; // prüfen, ob erster Eintrag
    bool isAtEnd() const; // prüfen, ob letzter Eintrag
    Song* getFirst() const; // ersten Eintrag bekommen
    Song* getLast() const; // letzten Eintrag bekommen
};

#endif // QUEUE_H
