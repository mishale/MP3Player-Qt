#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H
#include "playlist.h"
#include <QListWidgetItem>
#include <QString>

class PlaylistManager
{
public:
    PlaylistManager();

    QList<Playlist*> getPlaylists();
    void createPlaylist(const Playlist& playlist);
    void addPlaylist(Playlist* playlist);
    void deletePlaylist(Playlist* playlist);
    Playlist* getPlaylistByName(QString name);

private:

    QList<Playlist*> playlists;
};

#endif // PLAYLISTMANAGER_H
