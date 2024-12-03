#include "playlist.h"
#include <QList>
#include <QString>
#include <QDebug>

Playlist::Playlist(const QString& name)
    : name(name){}

QList<Song> Playlist::getSongs() const
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

void Playlist::addSong(const Song& song)
{
    songs.append(song);
}

void Playlist::deleteSong(const Song& song)
{
    songs.removeOne(song);
}

bool Playlist::containsSong(const Song& song)
{
    return songs.contains(song);
}
