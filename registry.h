#pragma once

#include <functional>
#include <unordered_map>

#include <QString>

#include "basesearcher.h"
#include "searcher.h"
#include "searcherconcepts.h"
#include "searcherinterface.h"

class Registry {
public:
  template <IsSearcherImplementation SearcherImplementation>
  static bool registerSearcher() {
    auto name = SearcherImplementation::name();
    if (registry().contains(name)) {
      return false;
    }
    registry()[name] = [](Searcher &searcher) {
      return std::make_unique<BaseSearcher<SearcherImplementation>>(searcher);
    };
    qDebug() << "Registered searcher" << name;
    return true;
  }

  static std::unique_ptr<SearcherInterface> createSearcher(const QString &name,
                                                           Searcher &searcher) {
    qDebug() << "Creating searcher" << name;
    if (registry().contains(name)) {
      return registry()[name](searcher);
    } else {
      qCritical() << "Unknown searcher" << name;
      return nullptr;
    }
  }

private:
  static std::unordered_map<
      QString, std::function<std::unique_ptr<SearcherInterface>(Searcher &)>> &
  registry() {
    static std::unordered_map<
        QString, std::function<std::unique_ptr<SearcherInterface>(Searcher &)>>
        registry;
    return registry;
  }
};
