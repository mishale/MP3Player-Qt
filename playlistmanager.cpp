#include "playlistmanager.h"
#include <QList>

PlaylistManager::PlaylistManager() {}

QList<Playlist*> PlaylistManager::getPlaylists()
{
    return playlists;
}

void PlaylistManager::addPlaylist(Playlist* playlist)
{
    playlists.append(playlist);
}

// Neue Funktion: Sucht eine Playlist nach Name und gibt sie zurück
Playlist* PlaylistManager::findPlaylistByName(const QString& name) const
{
    for (Playlist* playlist : playlists) {
        if (playlist->getName() == name) {
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
