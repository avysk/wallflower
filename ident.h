#pragma once

#include <QByteArray>
#include <QCryptographicHash>
#include <QString>
#include <QUuid>

// This class allows to create a stable uuid based on the index of the
// wallpapers source and its name.
class Ident {
public:
  static QString generateUuid(const unsigned int index, const QString &name) {
    auto data = QByteArray::number(index) + name.toUtf8();
    auto hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1);
    auto uuidBytes = hash.left(16);
    return QUuid::fromRfc4122(uuidBytes).toString();
  }
};
