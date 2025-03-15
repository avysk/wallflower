#include <memory>

#include <QObject>
#include <QString>
#include <QVector>

#include "picture.h"
#include "searcher.h"
#include "searcherinterface.h"

void Searcher::onSearchFinished(const QVector<Picture> &result) {
  emit searchFinished(result);
};

void Searcher::onSearchError(const QString &message) {
  emit searchError(message);
};

void Searcher::searchWallpapers(const QString &term) {
  mImpl->searchWallpapers(term);
};

void Searcher::setImpl(std::unique_ptr<SearcherInterface> interface) {
  mImpl = std::move(interface);
};
