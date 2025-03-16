#include <QRegularExpressionValidator>
#include <pluginsettings.h>

#include "ui_wallflowersettings.h"
#include "wallflowersettings.h"

WallflowerSettings::WallflowerSettings(PluginSettings &settings,
                                       QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), mSettings(settings) {
  setAttribute(Qt::WA_DeleteOnClose);
  setObjectName(QStringLiteral("Wallflower Settings"));
  ui->setupUi(this);
  ui->resultsCutoff->setValidator(new QRegularExpressionValidator(
      QRegularExpression(QStringLiteral("[1-9]\\d{0,3}")), this));
  connect(ui->buttonBox, &QDialogButtonBox::clicked, this,
          &WallflowerSettings::dialogButtonBoxAction);
  if (!settings.contains(QStringLiteral("resultsCutoff"))) {
    settings.setValue(QStringLiteral("resultsCutoff"), QStringLiteral("100"));
    settings.sync();
  }
  if (!settings.contains(QStringLiteral("searchTerm"))) {
    settings.setValue(QStringLiteral("searchTerm"), QStringLiteral("nature"));
    settings.sync();
  }
  ui->resultsCutoff->setText(
      settings.value(QStringLiteral("resultsCutoff")).toString());
  ui->searchTerm->setText(
      settings.value(QStringLiteral("searchTerm")).toString());
}

PluginSettings &WallflowerSettings::settings() const { return mSettings; }

void WallflowerSettings::dialogButtonBoxAction(QAbstractButton *button) {
  if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
    settings().setValue(QStringLiteral("resultsCutoff"),
                        ui->resultsCutoff->text());
    mSettings.setValue(QStringLiteral("searchTerm"), ui->searchTerm->text());
  }
  close();
}
