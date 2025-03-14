#ifndef SEARCHERINTERFACE_H
#define SEARCHERINTERFACE_H

#include <QString>

class S;

class SearcherInterface {
public:
  virtual ~SearcherInterface() = default;
  virtual void searchWallpapers(const QString &term) = 0;
};

#endif // SEARCHERINTERFACE_H
