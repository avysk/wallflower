#include "wallflowersettings.h"
#include "ui_wallflowersettings.h"

WallflowerSettings::WallflowerSettings(PluginSettings &settings,
                                       QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), msettings(settings) {
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName(QStringLiteral("Wallflower Settings"));
  ui->setupUi(this);
}

PluginSettings &WallflowerSettings::settings() const { return msettings; }
