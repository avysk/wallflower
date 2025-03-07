#include <QString>
#include <QVector>
#include <qobject.h>
#include <qtmetamacros.h>

#include "picture.h"
#include "searcher.h"
#include "sites/basesearcher.h"

Searcher::Searcher(QObject *parent) : QObject(parent) {
  connect(mSearcherImpl.get(), &BaseSearcher::searchFinished, this,
          &Searcher::onSearchFinished);
  connect(mSearcherImpl.get(), &BaseSearcher::searchError, this,
          &Searcher::onSearchError);
};

void Searcher::onSearchFinished(const QVector<const Picture> &result) {
  emit searchFinished(result);
};

void Searcher::onSearchError(const QString &message) {
  emit searchError(message);
};

void Searcher::searchWallpapers(const QString &term) {
  mSearcherImpl->search(term);
};

void Searcher::setImpl(std::unique_ptr<BaseSearcher> impl) {
  mSearcherImpl = std::move(impl);
};
