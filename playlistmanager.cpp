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

void PlaylistManager::deletePlaylist(Playlist* playlist)
{
    playlists.removeOne(playlist);
}
