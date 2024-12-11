#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVBoxLayout>
#include <QLabel>
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
    void importPlaylistsFromJson();
    void showContextMenuPlaylist(const QPoint &pos); // Kontext-Menü bei Rechtsklick
    void showContextMenuSongs(const QPoint &pos);
    void deletePlaylist(); // Playlist löschen
    void deleteSong(); // Song aus Playlist löschen
    void changeVolume(int value); // Ändern der Lautstärke
    void fromLibToPlaylist(Playlist* playlist);
    void displayMetaData(Song* song);
    void handleLoop();
    void handleSongFinish(qint64 position);
    void updateSliderPosition(qint64 position); // Aktualisiert den Slider, wenn die Songposition sich ändert
    void setSongPosition(int position); // Setzt die Songposition, wenn der Slider bewegt wird
    void updateSliderRange(qint64 duration); // Aktualisiert den Sliderbereich, wenn ein neuer Song geladen wird
    void playNextSong();
    void playPrevSong();
    void shuffle();

private:
    Ui::MainWindow *ui;
    void loadCombinedStylesheet(const QStringList &stylesheetFiles);
    void searchMP3Files(const QString &directoryPath); // MP3-Suche
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVBoxLayout *verticalLayout;
    QLabel *songMeta;
    bool widgetsCreated;
    Playlist *bibliothek;
    Playlist *currentPlaylist;
    Queue *queue;
    bool isSongLooped = false;
    bool isPlaylistLooped = false;
    bool isShuffled = false;
    PlaylistManager *allPlaylists;
    void addClass(QWidget* widget, const QString& classToAdd);
    void removeClass(QWidget* widget, const QString& classToRemove);
    void displayPlaylist(Playlist* playlist);
    void catchMetaData(Song* song);
    Playlist* getPlaylistByGUI(QListWidgetItem *selectedItem);
    Song* getSongByGUI(QListWidgetItem *selectedItem);
    void buildQueue(Song* song, Playlist* playlist);
    bool checkIfSongIsInPlaylist(QString filePath, Playlist* playlist);
};
#endif // MAINWINDOW_H

