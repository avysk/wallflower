#pragma once

#include <QObject>
#include <QString>

class Podibasu : public QObject {
  Q_OBJECT

public:
  Podibasu(QObject *parent = nullptr) : QObject(parent) {};
  ~Podibasu() = default;

signals:
  void wallpaperSet(const QString &message);
  void wallpaperSetError(const QString &message);

public slots:
  void setWallpaper(const QString &fullImagePath);

private:
  constexpr static auto DBUS_SERVICE = "org.pcmanfm.PCManFM";
  constexpr static auto DBUS_PATH = "/Application";
  constexpr static auto DBUS_INTERFACE = "org.pcmanfm.Application";
  constexpr static auto DBUS_COMMAND = "setWallpaper";
  constexpr static auto WALLPAPER_MODE = "zoom";
};
