#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "song.h"
#include <QListWidgetItem>
#include <QString>

class Playlist
{
public:
    Playlist(const QString& name);

    QString getName() const;
    QList<Song> getSongs() const;
    void addSong(const Song &song);
    void deleteSong(const Song& song);
    void changeName(const QString& newName);
    bool containsSong(const Song& song);
    bool operator==(const Playlist& other) const {
        return this->name == other.name;
    }

private:

    QString name;
    QList<Song> songs;
};

#endif // PLAYLIST_H
