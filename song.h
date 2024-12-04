#ifndef SONG_H
#define SONG_H
#include <QString>
#include <QMediaMetaData>
#include <QList>

class Song
{
public:
    Song(const QString& filePath);

    QString getFilePath() const;
    QList<QString> getMetaData(const QMediaMetaData& metaData);
    QList<QString> getCachedMetaData() const;
    bool operator==(const Song& other) const {
        return this->filePath == other.filePath;
    }

private:

    QString filePath;
    QList<QString> metaList;
    bool metaDataLoaded = false;


};

#endif // SONG_H
