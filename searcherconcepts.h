#pragma once

#include <concepts>

#include <QSet>
#include <QString>
#include <QVector>

#include "searcher.h"

template <typename SearcherImplementation>
concept IsSearcherImplementation =
    requires(SearcherImplementation impl, const Searcher *searcher,
             const QString &term, const unsigned int resultsCutoff,
             const QSet<QString> &ignoredWallpapers) {
      { impl.attach(searcher) } -> std::same_as<void>;
      {
        impl.searchWallpapers(term, resultsCutoff, ignoredWallpapers)
      } -> std::same_as<void>;
    };
