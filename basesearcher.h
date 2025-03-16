#pragma once

#include <memory>

#include <QString>

#include "searcher.h"
#include "searcherconcepts.h"
#include "searcherinterface.h"

template <typename SearcherImplementation>
  requires IsSearcherImplementation<SearcherImplementation>
class BaseSearcher : public SearcherInterface {

public:
  explicit BaseSearcher(const Searcher &searcher);
  ~BaseSearcher() = default;
  virtual void searchWallpapers(const QString &term,
                                const unsigned int resultsCutoff) override;

private:
  std::unique_ptr<SearcherImplementation> mImpl;
};
