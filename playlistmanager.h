#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H
#include "playlist.h"
#include <QListWidgetItem>
#include <QString>

class PlaylistManager
{
public:
    PlaylistManager();

    QList<Playlist*> getPlaylists(); // alle Playlists
    void addPlaylist(Playlist* playlist); // Playlist hinzufügen
    void deletePlaylist(Playlist* playlist); // Playlist löschen
    Playlist* getPlaylistByName(QString name);
    Playlist* findPlaylistByName(const QString& name) const;

private:

    QList<Playlist*> playlists;
};

#endif // PLAYLISTMANAGER_H
