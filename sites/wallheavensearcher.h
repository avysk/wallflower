#ifndef SEARCHER_H
#define SEARCHER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QVector>
#include <qobject.h>
#include <qtmetamacros.h>

#include "../picture.h"
#include "basesearcher.h"

class WallheavenSearcher : public BaseSearcher {
  Q_OBJECT

public:
  explicit WallheavenSearcher(QObject *parent = nullptr);
  ~WallheavenSearcher();

public slots:
  void search(const QString &term) override;

private slots:
  void onPageReceived(QNetworkReply *searchReply);

private:
  void searchWallpapers();
  const QString WALLHEAVEN_API_URL = "https://wallhaven.cc/api/v1/search";
  QNetworkAccessManager mSearchManager;
  unsigned int mCurrentPage;
  QString mQuery;
  QVector<const Picture> mWallpapers;
};

#endif // SEARCHER_H
