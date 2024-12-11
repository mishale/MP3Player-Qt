#include "queue.h"
#include <iostream>
#include <QMediaPlayer>
Queue::Queue() : head(nullptr), tail(nullptr), current(nullptr) {}

Queue::~Queue()
{
    Node* temp = head;
    while (temp != nullptr) {
        Node* next = temp->next;
        delete temp;
        temp = next;
    }
}

void Queue::addSong(Song* song)
{
    Node* newNode = new Node(song);
    if (head == nullptr) {
        head = tail = current = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

void Queue::forwards()
{
    if (current != nullptr && current->next != nullptr) {
        current = current->next;
    } else {
        std::cout << "Ende der Playlist erreicht." << std::endl;
    }
}

void Queue::backwards()
{
    if (current != nullptr && current->prev != nullptr) {
        current = current->prev;
    } else {
        std::cout << "Anfang der Playlist erreicht." << std::endl;
    }
}

Song* Queue::getCurrentSong() const
{
    if (current != nullptr) {
        return current->song;
    } else {
        throw std::runtime_error("Die Queue ist leer.");
    }
}

Song* Queue::getFirst() const
{
    return head->song;
}

Song* Queue::getLast() const
{
    return tail->song;
}

bool Queue::isAtStart() const
{
    return current == head;
}

bool Queue::isAtEnd() const
{
    return current == tail;
}

void Queue::clear()
{
    Node* temp = head;
    while (temp) {
        Node* toDelete = temp;
        temp = temp->next;
        delete toDelete;
    }

    head = nullptr;
    tail = nullptr;
    current = nullptr;
}
