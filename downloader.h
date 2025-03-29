#pragma once

#include <QDir>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <memory>

class IPathmaker {

public:
  virtual void makePath(const QString &path) const = 0;
  virtual ~IPathmaker() = default;
};

class QPathmaker : public IPathmaker {
public:
  virtual void makePath(const QString &path) const override {
    QDir().mkpath(path);
  }
};

class Downloader : public QObject {
  Q_OBJECT

public:
  explicit Downloader(QObject *parent = nullptr);
  explicit Downloader(std::unique_ptr<IPathmaker> pathmaker,
                      QObject *parent = nullptr);
  ~Downloader() = default;

signals:
  void downloadFinished(const QString &imageFile);
  void downloadError(const QString &message);

public slots:
  void download(const QString &imageUri);

private slots:
  void onDownloadFinished(QNetworkReply *downloadReply);

private:
  std::unique_ptr<QNetworkAccessManager> mDownloadManager;
  std::unique_ptr<const QString> mSavePath;
};
