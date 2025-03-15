#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

#include "../searcher.h"
#include "wallheavensearcher.h"

WallheavenSearcher::WallheavenSearcher(QObject *parent) : QObject(parent) {
  mQuery = std::make_unique<const QString>("");
  mSearchManager = std::make_unique<QNetworkAccessManager>();
  mTimer = std::make_unique<QTimer>();
  mTimer->setSingleShot(true);
  connect(mSearchManager.get(), &QNetworkAccessManager::finished, this,
          &WallheavenSearcher::onPageReceived);
  connect(mTimer.get(), &QTimer::timeout, this, &WallheavenSearcher::search);
}

void WallheavenSearcher::attach(const Searcher *searcher) const {
  connect(this, &WallheavenSearcher::searchFinished, searcher,
          &Searcher::onSearchFinished);
  connect(this, &WallheavenSearcher::searchError, searcher,
          &Searcher::onSearchError);
}

void WallheavenSearcher::searchWallpapers(const QString &term) {
  mCurrentPage = 1;
  mWallpapers.clear();
  mQuery = std::make_unique<const QString>(term);
  search();
}

void WallheavenSearcher::search() {
  QUrl searchUrl(WALLHEAVEN_API_URL);
  QUrlQuery queryUrl;
  queryUrl.addQueryItem("q", *mQuery);
  // queryUrl.addQueryItem("sorting", "top");
  queryUrl.addQueryItem("page", QString::number(mCurrentPage));
  qDebug() << "Getting page" << mCurrentPage;
  searchUrl.setQuery(queryUrl);
  QNetworkRequest searchRequest(searchUrl);
  // TODO: error on timeout
  mSearchManager->get(searchRequest);
}

void WallheavenSearcher::onPageReceived(QNetworkReply *searchReply) {
  if (searchReply->error() != QNetworkReply::NoError) {

    unsigned int code =
        searchReply->attribute(QNetworkRequest::HttpStatusCodeAttribute)
            .toInt();
    if (code == 429) {
      // rate limited
      qDebug() << "Hit rate limit";
      if (!searchReply->hasRawHeader("Retry-After")) {
        qCritical() << "Rate limited but no Retry-After header";
        ;
        emit searchError("wallheaven.cc API reported too many requests, but "
                         "the reply had no Retry-After header");
      } else {
        QByteArray retryAfterValue = searchReply->rawHeader("Retry-After");
        qDebug() << "Retry-After value:" << retryAfterValue;

        bool ok;
        int retrySeconds = retryAfterValue.toInt(&ok);
        if (ok) {
          int retryMiliseconds =
              retrySeconds * 1000 + 10; // extra 10 ms as safety margin
          qDebug() << "Retry after" << retryMiliseconds << "miliseconds.";
          mTimer->setInterval(retryMiliseconds);
          mTimer->start();
        } else {
          qCritical() << "Retry-After is a date string:" << retryAfterValue;
          emit searchError("wallheaven.cc API reported too many requests, but "
                           "the value of Retry-After header cannot be parsed.");
        }
      }
    } else { // nat 429
      emit searchError(
          QString("Cannot get page %1 af search result for term %2: %3 %4")
              .arg(mCurrentPage)
              .arg(*mQuery)
              .arg(code)
              .arg(searchReply->errorString()));
    }
    searchReply->deleteLater();
    return;
  }

  auto jsonDoc = QJsonDocument::fromJson(searchReply->readAll());
  auto jsonObj = jsonDoc.object();
  auto jsonArray = jsonObj["data"].toArray();

  for (const auto &entry : jsonArray) {
    const Picture p = {.id = entry.toObject()["id"].toString(),
                       .path = entry.toObject()["path"].toString()};
    mWallpapers.append(p);
  }

  searchReply->deleteLater();

  unsigned int lastPage = jsonObj["meta"].toObject()["last_page"].toInt();
  qDebug() << "pages:" << lastPage;
  if (mWallpapers.size() >= RESULTS_CUTOFF) {
    qDebug() << "Found" << mWallpapers.size() << "results, stopping search";
  } else if (mCurrentPage < lastPage) {
    mCurrentPage++;
    mTimer->setInterval(0);
    mTimer->start();
    return;
  }
  emit searchFinished(mWallpapers);
}
