#pragma once

#include <QSet>
#include <QString>

class SearcherInterface {
public:
  virtual ~SearcherInterface() = default;
  virtual void searchWallpapers(const QString &term,
                                const unsigned int resultsCutoff,
                                const QSet<QString> &ignoredWallpapers) = 0;
};
