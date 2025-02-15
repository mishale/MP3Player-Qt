#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVBoxLayout>
#include <QLabel>
#include "playlistmanager.h"
#include "queue.h"
#include "sleeptimerwindow.h"

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
    void addClass(QWidget* widget, const QString& classToAdd);
    void removeClass(QWidget* widget, const QString& classToRemove);

private slots:
    void initPlayer(); // Player initialisieren
    void initMiniPlayer();
    void setUpSleepTimer(); // Sleep Timer initialisieren
    void selectDirectory(); // Verzeichnis auswählen
    void displayMetaData(Song* song); // Metadaten anzeigen
    void startSong(QListWidgetItem *item); // Song abspielen & Warteschlange
    void getPlaylistOnClick(QListWidgetItem *playlist); // Playlist anzeigen
    void pause(); // pausieren/abspielen
    void createPlaylistUI(); // GUI zum Playlist erstellen
    void showContextMenuPlaylist(const QPoint &pos); // Kontext-Menü bei Rechtsklick für Playlist
    void showContextMenuSongs(const QPoint &pos); // Kontext-Menü bei Rechtsklick für Songs
    void fromLibToPlaylist(Playlist* playlist); // Songs von Bib in andere Playlists hinzufügen
    void handleLoop(); // Loop Handling
    void shuffle(); // Shuffle Handling
    void playNextSong(); // nächster Song
    void playPrevSong(); // vorheriger Song
    void handleSongFinish(qint64 position);
    void deletePlaylist(); // Playlist löschen
    void deleteSong(); // Song aus Playlist löschen
    void updateSliderPosition(qint64 position); // Aktualisiert den Slider, wenn die Songposition sich ändert
    void setSongPosition(int position); // Setzt die Songposition, wenn der Slider bewegt wird
    void updateSliderRange(qint64 duration); // Aktualisiert den Sliderbereich, wenn ein neuer Song geladen wird
    void printSongList();
    void exportSongListToJson();
    void importPlaylistsFromJson();
    void changeVolume(int value); // Ändern der Lautstärke
    void printColored(const QString& text, const QString& textColor, const QString& backgroundColor, bool bold); // farbige Konsolenausgaben
    void toggleMiniPlayer();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVBoxLayout *verticalLayout;
    QLabel *songMeta;
    bool widgetsCreated; // Hilfsvariable zur Playlisterstellung
    Playlist *bibliothek;
    Playlist *currentPlaylist; // Playlist, die derzeit gespielt wird
    Queue *queue;
    SleepTimerWindow *sleeptimerwindow;
    PlaylistManager *allPlaylists;
    QWidget *playerContainer;
    QVBoxLayout *playerLayout;
    bool isSongLooped = false;
    bool isPlaylistLooped = false;
    bool isShuffled = false;
    void loadCombinedStylesheet(const QStringList &stylesheetFiles);
    bool IsSongInPlaylist(QString filePath, Playlist* playlist); // Prüft, ob Song bereits in Playlist gespeichert ist
    void displayPlaylist(Playlist* playlist); // Befüllen des QListWidgets mit Songs
    void addSongToUIList(Song* s);
    bool isSongInUIList(Song* s);
    void catchMetaData(Song* song); // zum einmaligen Abgreifen der Metadaten
    void buildQueue(Song* song, Playlist* playlist); // bildet Warteschlange
    Playlist* getPlaylistByGUI(QListWidgetItem *selectedItem); // Playlist-Objekt erreichbar durch GUI
    Song* getSongByGUI(QListWidgetItem *selectedItem); // Song-Objekt erreichbar durch GUI
};
#endif // MAINWINDOW_H

