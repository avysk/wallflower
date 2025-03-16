#pragma once

#include <QString>

class S;

class SearcherInterface {
public:
  virtual ~SearcherInterface() = default;
  virtual void searchWallpapers(const QString &term,
                                const unsigned int resultsCutoff) = 0;
};
