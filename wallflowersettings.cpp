#include <QIntValidator>
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
  if (!settings.contains(QStringLiteral("autoAcknowledge"))) {
    settings.setValue(QStringLiteral("autoAcknowledge"),
                      QStringLiteral("false"));
  }
  if (!settings.contains(QStringLiteral("autoAcknowledgeInterval"))) {
    settings.setValue(QStringLiteral("autoAcknowledgeInterval"), 1);
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
  ui->autoAcknowledge->setChecked(
      settings.value(QStringLiteral("autoAcknowledge")).toBool());
  ui->autoAcknowledgeInterval->setText(
      settings.value(QStringLiteral("autoAcknowledgeInterval")).toString());

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

  if (ui->autoAcknowledge->isChecked()) {
    ui->autoAcknowledgeInterval->setEnabled(true);
  } else {
    ui->autoAcknowledgeInterval->setEnabled(false);
  }

  ui->autoAcknowledgeInterval->setText(
      QString::number(settings.value("autoAcknowledgeInterval", 1).toInt()));
  ui->autoAcknowledgeInterval->setValidator(new QIntValidator(0, 10000, this));
}

PluginSettings &WallflowerSettings::settings() const { return mSettings; }

#define UPDATE(setting, setting_in_ui)                                         \
  if (!mSettings.contains(setting) ||                                          \
      (ui->setting_in_ui) != mSettings.value(setting)) {                       \
    mSettings.setValue((setting), (ui->setting_in_ui));                        \
  }

void WallflowerSettings::dialogButtonBoxAction(QAbstractButton *button) {
  if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
    UPDATE("resultsCutoff", resultsCutoff->text());
    UPDATE("searchTerm", searchTerm->text());
    UPDATE("autoReload", autoReload->isChecked());
    UPDATE("updateInterval", updateInterval->currentData());
    auto aInterval = ui->autoAcknowledgeInterval->text().toInt();
    if (aInterval == 0) {
      ui->autoAcknowledge->setChecked(false);
      ui->autoAcknowledgeInterval->setText(QString::number(1));
    }
    UPDATE("autoAcknowledge", autoAcknowledge->isChecked());
    UPDATE("autoAcknowledgeInterval", autoAcknowledgeInterval->text());
    close();
  }
}
