#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "song.h"
#include <QListWidgetItem>
#include <QString>

class Playlist
{
public:
    Playlist(const QString& name);

    QString getName() const; // Name der Playlist
    QList<Song*> getSongs() const; // alle Songs einer Playlist
    void addSong(Song* song); // Song hinzufügen
    void deleteSong(Song* song); // Song löschen
    void changeName(const QString& newName); // Namen ändern
    bool containsSong(Song* song); // prüfen, ob Song in Playlist
    void clearSongs(); // alle Songs einer Playlist löschen
    bool operator==(const Playlist& other) const {
        return this->name == other.name;
    }
    bool containsSong(const QString& filePath) const;
    Song* findSong(const QString& filePath) const; // Song finden
    void printSongs() const;

private:

    QString name;
    QList<Song*> songs;
};

#endif // PLAYLIST_H
