#ifndef SONG_H
#define SONG_H
#include <QString>
#include <QMediaMetaData>
#include <QList>

class Song
{
public:
    Song(const QString& filePath);

    QString getFilePath() const; // Pfad des Songs
    QList<QString> getMetaData(const QMediaMetaData& metaData); // Metadaten erstmalig
    QList<QString> getCachedMetaData() const; // gespeicherte Metadaten
    QString getTitle() const; // Titel
    QString getAuthor() const; // Autor
    QString getDuration() const; // Länge
    QString getDate() const; // Erscheinungsdatum
    bool operator==(const Song& other) const {
        return this->filePath == other.filePath;
    }
    bool metaDataLoaded = false;

private:

    QString filePath;
    QList<QString> metaList;



};

#endif // SONG_H
