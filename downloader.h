#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

class Downloader : public QObject {
  Q_OBJECT

public:
  Downloader(QObject *parent = nullptr);
  ~Downloader() = default;

signals:
  void downloadFinished(const QString &imageFile);
  void downloadError(const QString &message);

public slots:
  void download(const QString &imageUri);

private slots:
  void onDownloadFinished(QNetworkReply *downloadReply);

private:
  QNetworkAccessManager mDownloadManager;
  QString mSavePath;
};
