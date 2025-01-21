#include "playlist.h"
#include <QList>
#include <QString>
#include <QDebug>

Playlist::Playlist(const QString& name)
    : name(name) {}

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

bool Playlist::containsSong(const QString& filePath) const
{
    for (const Song* song : songs) {
        if (song->getFilePath() == filePath) {
            return true;
        }
    }
    return false;
}

Song* Playlist::findSong(const QString& filePath) const
{
    for (Song* song : songs) {
        if (song->getFilePath() == filePath) {
            return song;
        }
    }
    return nullptr;
}

void Playlist::clearSongs()
{
    qDeleteAll(songs);
    songs.clear();
}


void Playlist::printSongs() const
{
    qDebug() << "Songs in playlist:" << name;
    for (const Song* song : songs) {
        qDebug() << "    Title: " << song->getTitle();
        qDebug() << "    Author: " << song->getAuthor();
        qDebug() << "    Duration: " << song->getDuration();
        qDebug() << "    Date: " << song->getDate();
        qDebug() << "    File Path: " << song->getFilePath();
        qDebug() << "----------------------";
    }
}
