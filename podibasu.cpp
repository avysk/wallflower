#include <QDebug>
#include <QString>
#include <QtDBus>

#include "podibasu.h"

void Podibasu::setWallpaper(const QString &fullImagePath) {
  qDebug() << "setting wallpaper to" << fullImagePath;
  QDBusInterface pcmanfmInterface(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,
                                  QDBusConnection::sessionBus());
  if (!pcmanfmInterface.isValid()) {
    emit wallpaperSetError(QString("Failed to connect to PCManFM: %1")
                               .arg(pcmanfmInterface.lastError().message()));
    return;
  }
  qDebug() << "got DBus interface";

  QDBusReply<void> dbus_reply =
      pcmanfmInterface.call(DBUS_COMMAND, fullImagePath, WALLPAPER_MODE);
  qDebug() << "got DBus reply";
  if (dbus_reply.isValid()) {
    emit wallpaperSet("");
  } else {
    emit wallpaperSetError(QString("Failed to set wallpaper: %1")
                               .arg(dbus_reply.error().message()));
  }
}
