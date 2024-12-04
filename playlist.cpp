#include "playlist.h"
#include <QList>
#include <QString>
#include <QDebug>

Playlist::Playlist(const QString& name)
    : name(name){}

QList<Song*> Playlist::getSongs() const
{
    return songs;
}

QString Playlist::getName() const
{
    return name;
}

void Playlist::changeName(const QString& newName)
{
    name = newName;
}

void Playlist::addSong(Song* song)
{
    songs.append(song);
}

void Playlist::deleteSong(Song* song)
{
    songs.removeOne(song);
}

bool Playlist::containsSong(Song* song)
{
    return songs.contains(song);
}
