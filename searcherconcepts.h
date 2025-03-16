#pragma once

#include <concepts>

#include <QString>
#include <QVector>

#include "searcher.h"

template <typename SearcherImplementation>
concept IsSearcherImplementation =
    requires(SearcherImplementation impl, const Searcher *searcher,
             const QString &term, const unsigned int resultsCutoff) {
      { impl.attach(searcher) } -> std::same_as<void>;
      { impl.searchWallpapers(term, resultsCutoff) } -> std::same_as<void>;
    };
