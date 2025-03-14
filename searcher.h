#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QVector>

#include "picture.h"
#include "searcherinterface.h"

class Searcher : public QObject {
  Q_OBJECT

public:
  explicit Searcher(QObject *parent = nullptr) : QObject(parent) {};
  ~Searcher() = default;
  void setImpl(std::unique_ptr<SearcherInterface> impl);

signals:
  void searchFinished(QVector<Picture> &result);
  void searchError(const QString &message);

public:
  virtual void onSearchFinished(QVector<Picture> &result);
  virtual void onSearchError(const QString &message);

public slots:
  void searchWallpapers(const QString &term);

private:
  std::unique_ptr<SearcherInterface> mImpl;
};
