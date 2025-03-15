#pragma once

#include <concepts>

#include <QString>
#include <QVector>

#include "searcher.h"

template <typename SearcherImplementation>
concept IsSearcherImplementation =
    requires(SearcherImplementation impl, const Searcher *searcher,
             const QString &term) {
      { impl.attach(searcher) } -> std::same_as<void>;
      { impl.searchWallpapers(term) } -> std::same_as<void>;
    };
