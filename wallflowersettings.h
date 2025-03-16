#pragma once

#include <QDialog>
#include <QObject>
#include <pluginsettings.h>

#include "ui_wallflowersettings.h"

class WallflowerSettings : public QDialog {
  Q_OBJECT

public:
  explicit WallflowerSettings(PluginSettings &settings,
                              QWidget *parent = nullptr);
  explicit WallflowerSettings(PluginSettings *settings,
                              QWidget *parent = nullptr)
      : WallflowerSettings(*settings, parent) {}
  ~WallflowerSettings() = default;

  PluginSettings &settings() const;

private:
  Ui::Dialog *ui;
  PluginSettings &msettings;
};
