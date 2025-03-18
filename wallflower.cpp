#include <QDialog>
#include <QLabel>
#include <QMenu>
#include <QObject>
#include <QPushButton>
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
  mDbusIcon =
      std::make_unique<const QIcon>(QIcon::fromTheme("image-x-generic"));
  mErrorIcon = std::make_unique<const QIcon>(QIcon::fromTheme("dialog-error"));
  mNormalIcon = std::make_unique<const QIcon>(
      QIcon::fromTheme("preferences-desktop-wallpaper"));

  mTimer = std::make_unique<QTimer>();
  connect(mTimer.get(), &QTimer::timeout, this, &Wallflower::searchWallpapers);

  settingsChanged();

  mButton = std::make_unique<QToolButton>();
  mButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  mButton->setPopupMode(QToolButton::InstantPopup);
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

QMenu *Wallflower::menuWithAbout() {
  auto menu = new QMenu(mButton.get());
  auto *about = new QLabel(QString("wallflower %1").arg(WALLFLOWER_VERSION));
  about->setAlignment(Qt::AlignCenter);
  about->setStyleSheet("font-weight: bold;");
  auto *aboutAction = new QWidgetAction(menu);
  aboutAction->setDefaultWidget(about);
  menu->addAction(aboutAction);
  menu->addSeparator();
  return menu;
}

void Wallflower::updateMenu(QMenu *newMenu) {
  QPointer<QMenu> oldMenu = mButton->menu();
  mButton->setMenu(newMenu);
  mButton->update();
  if (oldMenu) {
    oldMenu->disconnect();
    oldMenu->deleteLater();
  }
}

void Wallflower::busySlot(const QString &state) {
  mButton->setIcon(*mBusyIcon);

  mTimer->stop();
  auto menu = menuWithAbout();
  auto *searchMessage = new QLabel(state);
  searchMessage->setAlignment(Qt::AlignCenter);
  auto *searchAction = new QWidgetAction(menu);
  searchAction->setDefaultWidget(searchMessage);
  menu->addAction(searchAction);
  updateMenu(menu);
}

void Wallflower::errorSlot(const QString &message) {
  mButton->setIcon(*mErrorIcon);

  auto menu = menuWithAbout();
  menu->addAction("show error message", this, [this, message]() {
    QDialog dialog;
    dialog.setWindowIcon(*mErrorIcon);
    dialog.setWindowTitle("Wallflower Error");

    auto *label = new QLabel(message, &dialog);
    label->setWordWrap(true);
    label->setFixedWidth(300);
    auto *closeButton = new QPushButton("Close", &dialog);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::close);
    auto *layout = new QVBoxLayout(&dialog);
    layout->addWidget(label);
    layout->addWidget(closeButton);
    dialog.setLayout(layout);
    dialog.exec();
    dialog.deleteLater();
  });
  menu->addAction("clear error", this, [this]() { normalSlot(); });
  updateMenu(menu);

  setMessage(message);
}

void Wallflower::normalSlot(const QString &state) {
  mButton->setIcon(*mNormalIcon);

  auto menu = menuWithAbout();
  menu->addAction("search", this, [this]() { searchWallpapers(); });
  menu->addAction("never show again", this, [this]() {
    // TODO: do not allow to do this before the wallpaper is set by plugin
    auto ignored =
        settings()->value("ignoredWallpapers", QStringList()).toStringList();
    ignored.append(mCurrentWallpaper.id);
    mIgnoredWallpapers.insert(mCurrentWallpaper.id);
    settings()->setValue("ignoredWallpapers", ignored);
    searchWallpapers();
  });

  updateMenu(menu);
  setMessage(state);
  mTimer->start();
  connect(menu, &QMenu::aboutToShow, this, [this]() { mTimer->stop(); });
  connect(menu, &QMenu::aboutToHide, this, [this]() { mTimer->start(); });
}

void Wallflower::downloadDone(const QString &imageFile) {
  mButton->setIcon(*mDbusIcon);
  auto menu = menuWithAbout();
  auto *settingMessage = new QLabel("Setting wallpaper...");
  settingMessage->setAlignment(Qt::AlignCenter);
  auto *searchAction = new QWidgetAction(menu);
  searchAction->setDefaultWidget(settingMessage);
  menu->addAction(searchAction);
  updateMenu(menu);
  setMessage(QString("Wallpaper downloaded to %1, setting...").arg(imageFile));
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

void Wallflower::searchWallpapers() {
  busySlot("searching");
  mSearcher->searchWallpapers(mSearchTerm, mResultsCutoff, mIgnoredWallpapers);
}

void Wallflower::settingsChanged() {
  qDebug() << "settingsChanged";
  auto ignored =
      settings()->value("ignoredWallpapers", QStringList()).toStringList();
  mIgnoredWallpapers = QSet<QString>(ignored.begin(), ignored.end());

  mAutoReload = settings()->value("autoReload", false).toBool();
  mResultsCutoff = settings()->value("resultsCutoff", 100).toUInt();
  mSearchTerm = settings()->value("searchTerm", "nature").toString();
  mUpdateInterval = settings()->value("updateInterval", -1).toUInt();

  if (mAutoReload) {
    qDebug() << "autoReload" << mUpdateInterval;
    mTimer->setInterval(mUpdateInterval);
    mTimer->start();
  } else {
    mTimer->stop();
  }
}
