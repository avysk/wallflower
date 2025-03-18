#pragma once

#include <QDialog>
#include <QIcon>
#include <QObject>
#include <QSet>
#include <QString>
#include <QToolButton>
#include <QVector>
#include <ilxqtpanel.h>
#include <ilxqtpanelplugin.h>
#include <qcontainerfwd.h>

#include "downloader.h"
#include "picture.h"
#include "podibasu.h"
#include "searcher.h"

class Wallflower : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT

public:
  Wallflower(const ILXQtPanelPluginStartupInfo &startupInfo);
  ~Wallflower();

  virtual QWidget *widget() override { return mButton.get(); }
  virtual QString themeId() const override {
    return QStringLiteral("wallflower");
  }
  virtual ILXQtPanelPlugin::Flags flags() const override {
    return HaveConfigDialog | PreferRightAlignment;
  }
  QDialog *configureDialog() override;
  bool isSeparate() const override { return false; }
  bool isExpandable() const override { return false; }
  void realign() override;

public slots:
  void errorSlot(const QString &message);
  void setMessage(const QString &message);

private slots:
  void busySlot(const QString &state);
  void downloadDone(const QString &imageFile);
  void normalSlot(const QString &state = "");
  void searchDone(const QVector<Picture> &wallpapers);
  void searchWallpapers();

private:
  void settingsChanged() override;
  QMenu *menuWithAbout();
  void updateMenu(QMenu *newMenu);
  std::unique_ptr<QToolButton> mButton;
  std::unique_ptr<Downloader> mDownloader;
  std::unique_ptr<Podibasu> mPodibasu;
  std::unique_ptr<Searcher> mSearcher;
  std::unique_ptr<const QIcon> mBusyIcon, mDbusIcon, mErrorIcon, mNormalIcon;
  std::unique_ptr<QTimer> mTimer;
  bool mAutoReload;
  unsigned int mUpdateInterval;
  QSet<QString> mIgnoredWallpapers;
  unsigned int mResultsCutoff;
  QString mSearchTerm;
  Picture mCurrentWallpaper;
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
