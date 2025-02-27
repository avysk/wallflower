#ifndef WALLFLOWER_H
#define WALLFLOWER_H

#include <QToolButton>
#include <QtCore/qtmetamacros.h>
#include <ilxqtpanel.h>
#include <ilxqtpanelplugin.h>

class Wallflower : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT

public:
  Wallflower(const ILXQtPanelPluginStartupInfo &startupInfo);
  ~Wallflower();

  virtual QWidget *widget() override { return button; }
  virtual QString themeId() const override {
    return QStringLiteral("wallflower");
  }
  virtual ILXQtPanelPlugin::Flags flags() const override {
    return PreferRightAlignment;
  }
  bool isSeparate() const override { return false; }
  bool isExpandable() const override { return false; }
  void realign() override;

private:
  QToolButton *button;
};

class WallflowerLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
  ILXQtPanelPlugin *
  instance(const ILXQtPanelPluginStartupInfo &startupInfo) const {
    return new Wallflower(startupInfo);
  }
};

#endif
