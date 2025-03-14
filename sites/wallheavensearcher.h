#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>

#include "../picture.h"
#include "../searcher.h"

class WallheavenSearcher : public QObject {
  Q_OBJECT

public:
  explicit WallheavenSearcher(QObject *parent = nullptr);
  ~WallheavenSearcher() = default;
  void attach(Searcher *searcher);
  void searchWallpapers(const QString &term);

signals:
  void searchFinished(QVector<Picture> &result);
  void searchError(const QString &message);

private slots:
  void onPageReceived(QNetworkReply *searchReply);
  void search();

private:
  const unsigned int RESULTS_CUTOFF = 3000;
  const QString WALLHEAVEN_API_URL = "https://wallhaven.cc/api/v1/search";
  QNetworkAccessManager mSearchManager;
  unsigned int mCurrentPage;
  QString mQuery;
  QTimer mTimer;
  QVector<Picture> mWallpapers;
};
