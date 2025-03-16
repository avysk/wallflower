#include <QRegularExpressionValidator>
#include <pluginsettings.h>

#include "ui_wallflowersettings.h"
#include "wallflowersettings.h"

WallflowerSettings::WallflowerSettings(PluginSettings &settings,
                                       QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), msettings(settings) {
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName(QStringLiteral("Wallflower Settings"));
  ui->setupUi(this);
  ui->resultsCutoff->setValidator(new QRegularExpressionValidator(
      QRegularExpression(QStringLiteral("[1-9]\\d{0,3}")), this));
}

PluginSettings &WallflowerSettings::settings() const { return msettings; }
