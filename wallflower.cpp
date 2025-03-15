#include <QMenu>
#include <QObject>
#include <QRandomGenerator>
#include <QWidgetAction>

#include "basesearcher.h"
#include "downloader.h"
#include "searcher.h"
#include "sites/wallheavensearcher.h"
#include "wallflower.h"

Wallflower::Wallflower(const ILXQtPanelPluginStartupInfo &startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo) {
  mBusyIcon = std::make_unique<const QIcon>(QIcon::fromTheme("view-refresh"));
  mErrorIcon = std::make_unique<const QIcon>(QIcon::fromTheme("dialog-error"));
  mNormalIcon = std::make_unique<const QIcon>(
      QIcon::fromTheme("desktop-preferences-wallpaper"));
  mButton = std::make_unique<QToolButton>();
  mButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  mButton->setPopupMode(QToolButton::InstantPopup);
  auto menu = new QMenu(mButton.get());
  menu->addAction("search", this, [this]() {
    busySlot("searching");
    mSearcher->searchWallpapers("nature");
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
  int randomIndex = QRandomGenerator::global()->bounded(result.size());
  auto p = result[randomIndex];
  qDebug() << randomIndex << p.id << p.path;
  mDownloader->download(p.path);
}
