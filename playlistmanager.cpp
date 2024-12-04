#include "playlistmanager.h"
#include <QList>

PlaylistManager::PlaylistManager(){}

QList<Playlist*> PlaylistManager::getPlaylists()
{
    return playlists;
}

void PlaylistManager::createPlaylist(const Playlist &playlist)
{
    //playlists.addItem(playlist.getName());
    //verticalLayout->addWidget(ui->nameInput);
    //verticalLayout->addWidget(ui->createButton);
}
/*Playlist PlaylistManager::setPlaylist() const
{
}
*/
void PlaylistManager::addPlaylist(Playlist* playlist)
{
    playlists.append(playlist);
}

void PlaylistManager::deletePlaylist(Playlist* playlist)
{
    playlists.removeOne(playlist);
}
