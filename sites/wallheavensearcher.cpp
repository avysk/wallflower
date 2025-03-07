#include "qobject.h"
#include "qtmetamacros.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QUrlQuery>

#include "wallheavensearcher.h"

WallheavenSearcher::WallheavenSearcher(QObject *parent) : BaseSearcher(parent) {
  connect(&mSearchManager, &QNetworkAccessManager::finished, this,
          &WallheavenSearcher::onPageReceived);
}

void WallheavenSearcher::search(const QString &term) {
  mCurrentPage = 1;
  mWallpapers.clear();
  mQuery = term;
  searchWallpapers();
}

void WallheavenSearcher::searchWallpapers() {

  QUrl searchUrl(WALLHEAVEN_API_URL);
  QUrlQuery queryUrl;
  queryUrl.addQueryItem("q", mQuery);
  queryUrl.addQueryItem("page", QString::number(mCurrentPage));
  searchUrl.setQuery(queryUrl);
  QNetworkRequest searchRequest(searchUrl);
  // TODO: error on timeout
  mSearchManager.get(searchRequest);
}

// this is called when the next page received
void WallheavenSearcher::onPageReceived(QNetworkReply *searchReply) {
  if (searchReply->error() != QNetworkReply::NoError) {

    emit searchError(
        QString("Cannot get page %1 af search result for term %2: %3")
            .arg(mCurrentPage)
            .arg(mQuery)
            .arg(searchReply->errorString()));
    searchReply->deleteLater();
    return;
  }

  auto jsonDoc = QJsonDocument::fromJson(searchReply->readAll());
  auto jsonObj = jsonDoc.object();
  auto jsonArray = jsonObj["data"].toArray();

  for (const auto &entry : jsonArray) {
    Picture p;
    p.id = entry.toObject()["id"].toString();
    p.path = entry.toObject()["path"].toString();
    mWallpapers.append(p);
  }

  searchReply->deleteLater();

  unsigned int lastPage = jsonObj["meta"].toObject()["last_page"].toInt();
  if (mCurrentPage < lastPage) {
    mCurrentPage++;
    searchWallpapers();
  } else {
    emit searchFinished(mWallpapers);
  }
}
