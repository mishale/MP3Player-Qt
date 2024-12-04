#include "song.h"
#include <QMediaMetaData>

Song::Song(const QString& filePath)
    : filePath(filePath), metaList(), metaDataLoaded(false){}

QString Song::getFilePath() const
{
    return filePath;
}

QList<QString> Song::getMetaData(const QMediaMetaData& metaData)
{
    if(!metaDataLoaded)
    {
        if (!metaData.isEmpty()) {
            metaList.append(metaData.stringValue(QMediaMetaData::Title));
            if(metaList.at(0) == "")
            {
                metaList[0] = "Kein Titel vorhanden " + filePath;
            }
            metaList.append(metaData.stringValue(QMediaMetaData::Author));
            metaList.append(metaData.stringValue(QMediaMetaData::Duration));
            metaList.append(metaData.stringValue(QMediaMetaData::Date));

        }
        metaDataLoaded = true;
    }
    return metaList;
}

QList<QString> Song::getCachedMetaData() const
{
    return metaList;
}

