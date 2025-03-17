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
  }
  if (!settings.contains(QStringLiteral("searchTerm"))) {
    settings.setValue(QStringLiteral("searchTerm"), QStringLiteral("nature"));
  }
  if (!settings.contains(QStringLiteral("autoReload"))) {
    settings.setValue(QStringLiteral("autoReload"), QStringLiteral("false"));
  }
  if (!settings.contains(QStringLiteral("updateInterval"))) {
    settings.setValue(QStringLiteral("updateInterval"), 60000);
  }

  ui->resultsCutoff->setText(
      settings.value(QStringLiteral("resultsCutoff")).toString());
  ui->searchTerm->setText(
      settings.value(QStringLiteral("searchTerm")).toString());
  ui->autoReload->setChecked(
      settings.value(QStringLiteral("autoReload")).toBool());
  if (!ui->autoReload->isChecked()) {
    ui->updateInterval->setEnabled(false);
  }

  // Delay in milliseconds
  ui->updateInterval->setItemData(0, 60000);    // 1 minute
  ui->updateInterval->setItemData(1, 300000);   // 5 minutes
  ui->updateInterval->setItemData(2, 600000);   // 10 minutes
  ui->updateInterval->setItemData(3, 900000);   // 15 minutes
  ui->updateInterval->setItemData(4, 1800000);  // 30 minutes
  ui->updateInterval->setItemData(5, 3600000);  // 1 hour
  ui->updateInterval->setItemData(6, 7200000);  // 2 hours
  ui->updateInterval->setItemData(7, 21600000); // 6 hours
  ui->updateInterval->setItemData(8, 43200000); // 12 hours
  ui->updateInterval->setItemData(9, 86400000); // 24 hours

  if (ui->autoReload->isChecked()) {
    auto index = ui->updateInterval->findData(
        settings.value(QStringLiteral("updateInterval")).toUInt());
    if (index != -1) {
      ui->updateInterval->setCurrentIndex(index);
    }
  }
}

PluginSettings &WallflowerSettings::settings() const { return mSettings; }

void WallflowerSettings::dialogButtonBoxAction(QAbstractButton *button) {
  if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
    mSettings.setValue(QStringLiteral("resultsCutoff"),
                       ui->resultsCutoff->text());
    mSettings.setValue(QStringLiteral("searchTerm"), ui->searchTerm->text());
    mSettings.setValue(QStringLiteral("autoReload"),
                       ui->autoReload->isChecked());
    mSettings.setValue(QStringLiteral("updateInterval"),
                       ui->updateInterval->currentData());
  }
  close();
}
