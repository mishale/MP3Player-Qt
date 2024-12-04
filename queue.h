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

    void addSong(Song* song);
    void addPlaylist(const Playlist& playlist);
    void forwards();
    void backwards();
    Song* getCurrentSong() const;
};

#endif // QUEUE_H
