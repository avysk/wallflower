#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSet>
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
  static QString name() { return "wallheaven"; }
  void attach(const Searcher *searcher) const;
  void searchWallpapers(const QString &term, const unsigned int resultsCutoff,
                        const QSet<QString> &ignoredWallpapers);

signals:
  void searchFinished(const QVector<Picture> &result);
  void searchError(const QString &message);

private slots:
  void onPageReceived(QNetworkReply *searchReply);
  void search();

private:
  const QString WALLHEAVEN_API_URL = "https://wallhaven.cc/api/v1/search";
  unsigned int mCurrentPage, mResultsCutoff;
  std::unique_ptr<QNetworkAccessManager> mSearchManager;
  std::unique_ptr<const QString> mQuery;
  std::unique_ptr<QTimer> mTimer;
  QVector<Picture> mWallpapers;
  QSet<QString> mIgnoredWallpapers;
};
