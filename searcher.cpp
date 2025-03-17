#include <memory>

#include <QObject>
#include <QSet>
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

void Searcher::searchWallpapers(const QString &term,
                                const unsigned int resultsCutoff,
                                const QSet<QString> &ignoredWallpapers) {
  mImpl->searchWallpapers(term, resultsCutoff, ignoredWallpapers);
};

void Searcher::setImpl(std::unique_ptr<SearcherInterface> interface) {
  mImpl = std::move(interface);
};
