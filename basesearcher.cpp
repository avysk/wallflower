#include <QObject>
#include <QString>
#include <QVector>

#include "basesearcher.h"
#include "searcher.h"
#include "sites/wallheavensearcher.h"

template <typename SearcherImplementation>
  requires IsSearcherImplementation<SearcherImplementation>
BaseSearcher<SearcherImplementation>::BaseSearcher(Searcher &searcher) {
  mImpl = std::make_unique<SearcherImplementation>();
  mImpl->attach(&searcher);
}

template <typename SearcherImplementation>
  requires IsSearcherImplementation<SearcherImplementation>
void BaseSearcher<SearcherImplementation>::searchWallpapers(
    const QString &term) {
  mImpl->searchWallpapers(term);
}

template class BaseSearcher<WallheavenSearcher>;
