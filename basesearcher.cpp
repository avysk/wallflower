#include <QObject>
#include <QSet>
#include <QString>
#include <QVector>

#include "basesearcher.h"
#include "searcher.h"
#include "sites/wallheavensearcher.h"

template <typename SearcherImplementation>
  requires IsSearcherImplementation<SearcherImplementation>
BaseSearcher<SearcherImplementation>::BaseSearcher(const Searcher &searcher) {
  mImpl = std::make_unique<SearcherImplementation>();
  // Connections to searcher can be made here, but how to require that
  // SearcherImplementation has appropriate signals?
  mImpl->attach(&searcher);
}

template <typename SearcherImplementation>
  requires IsSearcherImplementation<SearcherImplementation>
void BaseSearcher<SearcherImplementation>::searchWallpapers(
    const QString &term, const unsigned int resultsCutoff,
    const QSet<QString> &ignoredWallpapers) {
  mImpl->searchWallpapers(term, resultsCutoff, ignoredWallpapers);
}

template class BaseSearcher<WallheavenSearcher>;
