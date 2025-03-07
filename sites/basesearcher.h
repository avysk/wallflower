#ifndef BASESEARCHER_H
#define BASESEARCHER_H

#include <QString>
#include <qobject.h>
#include <qtmetamacros.h>

#include "../picture.h"

class BaseSearcher : public QObject {
  Q_OBJECT

public:
  explicit BaseSearcher(QObject *parent = nullptr) : QObject(parent){};
  ~BaseSearcher();

signals:
  void searchFinished(const QVector<const Picture> &result);
  void searchError(const QString &message);

public slots:
  virtual void search(const QString &term) = 0;
};
#endif // BASESEARCHER_H
