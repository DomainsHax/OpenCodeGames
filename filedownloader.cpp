#include "filedownloader.h"

#include <QLoggingCategory>

FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) :QObject(parent)
{
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
    QNetworkRequest request(imageUrl);
    m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader() { }

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    m_DownloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded();
}

QByteArray FileDownloader::downloadedData() const { return m_DownloadedData; }
