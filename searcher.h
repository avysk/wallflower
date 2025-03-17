#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSet>
#include <QString>
#include <QVector>

#include "picture.h"
#include "searcherinterface.h"

class Searcher : public QObject {
  Q_OBJECT

public:
  explicit Searcher(QObject *parent = nullptr) : QObject(parent) {};
  void setImpl(std::unique_ptr<SearcherInterface> impl);

signals:
  // unfortunately, QVector<const T> is not possible, and QVector<const T*> is
  // too much manual memory-management work
  void searchFinished(const QVector<Picture> &result);
  void searchError(const QString &message);

public:
  virtual void onSearchFinished(const QVector<Picture> &result);
  virtual void onSearchError(const QString &message);

public slots:
  void searchWallpapers(const QString &term, const unsigned int resultsCutoff,
                        const QSet<QString> &ignoredWallpapers);

private:
  std::unique_ptr<SearcherInterface> mImpl;
};
