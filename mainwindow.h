#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVBoxLayout>
#include "playlistmanager.h"
#include "queue.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void initPlayer();
    void selectDirectory(); // Verzeichnis auswählen
    void startSong(QListWidgetItem *item); // Song abspielen & Warteschlange
    void pause(); // pausieren/abspielen
    void createPlaylistUI(); // GUI zum Playlist erstellen
    void getPlaylistOnClick(QListWidgetItem *playlist); // Playlist anzeigen
    void printSongList();
    void exportSongListToJson();
    void showContextMenuPlaylist(const QPoint &pos); // Kontext-Menü bei Rechtsklick
    void showContextMenuSongs(const QPoint &pos);
    void deletePlaylist(); // Playlist löschen
    void addSongToPlaylist(); // Song zu einer Playlist hinzufügen
    void deleteSong(); // Song aus Playlist löschen
    void changeVolume(int value); // Ändern der Lautstärke
    void fromLibToPlaylist(Playlist* playlist);

private:
    Ui::MainWindow *ui;
    void searchMP3Files(const QString &directoryPath); // MP3-Suche
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVBoxLayout *verticalLayout;
    bool widgetsCreated;
    Playlist *bibliothek;
    Queue *queue;
    PlaylistManager *allPlaylists;
    void addClass(QWidget* widget, const QString& classToAdd);
    void removeClass(QWidget* widget, const QString& classToRemove);
    void displayPlaylist(Playlist* playlist);
    void displayMetaData(Song* song);
    Playlist* getPlaylistByGUI(QListWidgetItem *selectedItem);

};
#endif // MAINWINDOW_H

