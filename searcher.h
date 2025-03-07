#ifndef SEARCHER_H
#define SEARCHER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QVector>
#include <qobject.h>
#include <qtmetamacros.h>

#include "picture.h"
#include "sites/basesearcher.h"

class Searcher : public QObject {
  Q_OBJECT

public:
  explicit Searcher(QObject *parent = nullptr);
  ~Searcher();
  void setImpl(std::unique_ptr<BaseSearcher> impl);

signals:
  void searchFinished(const QVector<const Picture> &result);
  void searchError(const QString &message);

public slots:
  void searchWallpapers(const QString &term);
  void onSearchFinished(const QVector<const Picture> &result);
  void onSearchError(const QString &message);

private:
  std::unique_ptr<BaseSearcher> mSearcherImpl;
};

#endif
