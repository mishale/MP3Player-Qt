#include "song.h"
#include <QMediaMetaData>

Song::Song(const QString& filePath)
    : filePath(filePath){}

QString Song::getFilePath() const
{
    return filePath;
}

QList<QString> Song::getMetaData(const QMediaMetaData& metaData) const
{
    QList<QString> metaList;
    if (!metaData.isEmpty()) {
        metaList.append(metaData.stringValue(QMediaMetaData::Title));
        metaList.append(metaData.stringValue(QMediaMetaData::Author));
        metaList.append(metaData.stringValue(QMediaMetaData::Duration));
        metaList.append(metaData.stringValue(QMediaMetaData::Date));

    }

    return metaList;
}

