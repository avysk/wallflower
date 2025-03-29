#include <QDebug>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QStandardPaths>
#include <QUrl>
#include <memory>

#include "downloader.h"

Downloader::Downloader(QObject *parent) : QObject(parent) {
  mSavePath = std::make_unique<const QString>(
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
      "/wallflower");
  auto pathmaker = std::make_unique<QPathmaker>();
  pathmaker->makePath(*mSavePath);

  mDownloadManager = std::make_unique<QNetworkAccessManager>();
  connect(mDownloadManager.get(), &QNetworkAccessManager::finished, this,
          &Downloader::onDownloadFinished);
}

void Downloader::download(const QString &imageUri) {
  qDebug() << "Starting download of" << imageUri;
  const QUrl url(imageUri);
  const QNetworkRequest imageRequest(imageUri);
  mDownloadManager->get(imageRequest);
}

void Downloader::onDownloadFinished(QNetworkReply *downloadReply) {
  if (downloadReply->error() == QNetworkReply::NoError) {
    const QString imageName = downloadReply->url().fileName();
    const QString downloadTo = *mSavePath + "/" + imageName;
    QFile imageFile(downloadTo);
    if (imageFile.open(QIODevice::WriteOnly)) {
      imageFile.write(downloadReply->readAll());
      // now we need to delete all other files in mSavePath directory
      QDir dir(*mSavePath);
      QStringList files = dir.entryList(QDir::Files);
      qDebug() << "Found files:" << files;
      for (auto &file : files) {
        if (file != imageName) {
          qDebug() << "Removing" << file;
          // TODO remove can fail
          dir.remove(file);
        }
      }
      emit downloadFinished(downloadTo);
    } else {
      emit downloadError(QString("Failed to save file %1").arg(downloadTo));
    }
  } else {
    emit downloadError(QString("Failed to download the wallpaper at %1: %2")
                           .arg(downloadReply->url().path())
                           .arg(downloadReply->errorString()));
  }
  downloadReply->deleteLater();
}
