#include "playlistmanager.h"
#include <QList>

PlaylistManager::PlaylistManager(){}

QList<Playlist*> PlaylistManager::getPlaylists()
{
    return playlists;
}

void PlaylistManager::addPlaylist(Playlist* playlist)
{
    playlists.append(playlist);
}

Playlist* PlaylistManager::getPlaylistByName(QString name)
{
    for(Playlist* playlist : playlists)
    {
        if (name == playlist->getName())
        {
            return playlist;
        }
    }
    return nullptr;
}
void PlaylistManager::deletePlaylist(Playlist* playlist)
{
    playlists.removeOne(playlist);
    delete playlist;
}
