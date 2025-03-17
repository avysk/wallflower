#include <QDialog>
#include <QLabel>
#include <QMenu>
#include <QObject>
#include <QRandomGenerator>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QToolButton>
#include <QWidgetAction>
#include <QtCore/Qt>
#include <ilxqtpanelplugin.h>

#include "basesearcher.h"
#include "downloader.h"
#include "searcher.h"
#include "sites/wallheavensearcher.h"
#include "version.h"
#include "wallflower.h"
#include "wallflowersettings.h"

Wallflower::Wallflower(const ILXQtPanelPluginStartupInfo &startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo) {
  qDebug() << "Wallflower" << WALLFLOWER_VERSION;
  mBusyIcon = std::make_unique<const QIcon>(QIcon::fromTheme("view-refresh"));
  mErrorIcon = std::make_unique<const QIcon>(QIcon::fromTheme("dialog-error"));
  mNormalIcon = std::make_unique<const QIcon>(
      QIcon::fromTheme("desktop-preferences-wallpaper"));

  mButton = std::make_unique<QToolButton>();
  mButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  mButton->setPopupMode(QToolButton::InstantPopup);
  auto menu = new QMenu(mButton.get());
  auto *about = new QLabel(QString("wallflower %1").arg(WALLFLOWER_VERSION));
  about->setAlignment(Qt::AlignCenter);
  about->setStyleSheet("font-weight: bold;");
  auto *aboutAction = new QWidgetAction(menu);
  aboutAction->setDefaultWidget(about);
  menu->addAction(aboutAction);
  menu->addSeparator();
  menu->addAction("search", this, [this]() {
    auto ignored =
        settings()->value("ignoredWallpapers", QStringList()).toStringList();
    busySlot("searching");
    mSearcher->searchWallpapers(
        settings()->value("searchTerm", "nature").toString(),
        settings()->value("resultsCutoff", "100").toInt(),
        QSet<QString>(ignored.begin(), ignored.end()));
  });
  menu->addAction("never show again", this, [this]() {
    // TODO: do not allow to do this before the wallpaper is set
    // TODO: do not allow to do this before the wallpaper is set by plugin
    auto ignored =
        settings()->value("ignoredWallpapers", QStringList()).toStringList();
    ignored.append(mCurrentWallpaper.id);
    settings()->setValue("ignoredWallpapers", ignored);
    settings()->sync();
    busySlot("searching");
    mSearcher->searchWallpapers(
        settings()->value("searchTerm", "nature").toString(),
        settings()->value("resultsCutoff", "100").toInt(),
        QSet<QString>(ignored.begin(), ignored.end()));
  });

  mButton->setMenu(menu);
  normalSlot();

  mDownloader = std::make_unique<Downloader>();
  connect(mDownloader.get(), &Downloader::downloadFinished, this,
          &Wallflower::downloadDone);
  connect(mDownloader.get(), &Downloader::downloadError, this,
          &Wallflower::errorSlot);

  mPodibasu = std::make_unique<Podibasu>();
  connect(mPodibasu.get(), &Podibasu::wallpaperSet, this,
          &Wallflower::normalSlot);
  connect(mPodibasu.get(), &Podibasu::wallpaperSetError, this,
          &Wallflower::errorSlot);

  mSearcher = std::make_unique<Searcher>();
  mSearcher->setImpl(
      std::make_unique<BaseSearcher<WallheavenSearcher>>(*mSearcher));
  connect(mSearcher.get(), &Searcher::searchFinished, this,
          &Wallflower::searchDone);
  connect(mSearcher.get(), &Searcher::searchError, this,
          &Wallflower::errorSlot);
}

Wallflower::~Wallflower() = default;

void Wallflower::realign() {
  mButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void Wallflower::setMessage(const QString &message) {
  mButton->setToolTip(message);
  mButton->update();
}

void Wallflower::busySlot(const QString &state) {
  mButton->setIcon(*mBusyIcon);
  setMessage(state);
}

void Wallflower::errorSlot(const QString &message) {
  mButton->setIcon(*mErrorIcon);
  setMessage(message);
}

void Wallflower::normalSlot(const QString &state) {
  mButton->setIcon(*mNormalIcon);
  mButton->update();
  setMessage(state);
}

void Wallflower::downloadDone(const QString &imageFile) {
  this->busySlot(
      QString("Wallpaper downloaded to %1, setting.").arg(imageFile));
  mPodibasu->setWallpaper(imageFile);
}

void Wallflower::searchDone(const QVector<Picture> &result) {
  if (result.isEmpty()) {
    errorSlot("No wallpapers found.");
    return;
  }
  int randomIndex = QRandomGenerator::global()->bounded(result.size());
  mCurrentWallpaper = result[randomIndex];
  qDebug() << randomIndex << mCurrentWallpaper.id << mCurrentWallpaper.path;
  mDownloader->download(mCurrentWallpaper.path);
}

QDialog *Wallflower::configureDialog() {
  return new WallflowerSettings(settings());
}
