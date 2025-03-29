#include <memory>

#include <QDialog>
#include <QLabel>
#include <QMenu>
#include <QObject>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QToolButton>
#include <QWidgetAction>
#include <QtCore/Qt>
#include <ilxqtpanelplugin.h>

#include "downloader.h"
#include "registry.h"
#include "searcher.h"
#include "sites/wallheavensearcher.h"
#include "version.h"
#include "wallflower.h"
#include "wallflowersettings.h"

Wallflower::Wallflower(const ILXQtPanelPluginStartupInfo &startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo) {
  Registry::registerSearcher<WallheavenSearcher>();

  qDebug() << "Wallflower" << WALLFLOWER_VERSION;
  mBusyIcon = std::make_unique<const QIcon>(QIcon::fromTheme("view-refresh"));
  mDbusIcon =
      std::make_unique<const QIcon>(QIcon::fromTheme("image-x-generic"));
  mErrorIcon = std::make_unique<const QIcon>(QIcon::fromTheme("dialog-error"));
  mNormalIcon = std::make_unique<const QIcon>(
      QIcon::fromTheme("preferences-desktop-wallpaper"));

  mErrorTimer = std::make_unique<QTimer>();
  mErrorTimer->setSingleShot(true);
  connect(mErrorTimer.get(), &QTimer::timeout, this,
          &Wallflower::searchWallpapers);
  mUpdateTimer = std::make_unique<QTimer>();
  connect(mUpdateTimer.get(), &QTimer::timeout, this,
          &Wallflower::searchWallpapers);

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

  mSearchers = std::vector<std::unique_ptr<Searcher>>();
  mSearcherIndex = 0;
  auto searcher = std::make_unique<Searcher>();
  auto impl = Registry::createSearcher("wallheaven", *searcher);
  searcher->setImpl(std::move(impl));
  connect(searcher.get(), &Searcher::searchFinished, this,
          &Wallflower::searchDone);
  connect(searcher.get(), &Searcher::searchError, this, &Wallflower::errorSlot);
  mSearchers.push_back(std::move(searcher));
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

  mUpdateTimer->stop();
  auto menu = menuWithAbout();
  auto *searchMessage = new QLabel(state);
  searchMessage->setAlignment(Qt::AlignCenter);
  auto *searchAction = new QWidgetAction(menu);
  searchAction->setDefaultWidget(searchMessage);
  menu->addAction(searchAction);
  updateMenu(menu);
}

void Wallflower::errorSlot(const QString &message) {
  // TODO: postpone if the busy menu is shown
  mButton->setIcon(*mErrorIcon);

  if (mAutoAcknowledge) {
    mErrorTimer->start(mAutoAcknowledgeInterval * 1000);
  }

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
  menu->addAction("clear error", this, [this]() {
    mErrorTimer->stop();
    normalSlot();
  });
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
  mUpdateTimer->start();
  connect(menu, &QMenu::aboutToShow, this, [this]() { mUpdateTimer->stop(); });
  connect(menu, &QMenu::aboutToHide, this, [this]() { mUpdateTimer->start(); });
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
  if (mSearchers.empty()) {
    errorSlot("No searchers available.");
    return;
  }
  mSearchers[mSearcherIndex++]->searchWallpapers(mSearchTerm, mResultsCutoff,
                                                 mIgnoredWallpapers);
  mSearcherIndex %= mSearchers.size();
}

void Wallflower::settingsChanged() {
  auto ignored =
      settings()->value("ignoredWallpapers", QStringList()).toStringList();
  mIgnoredWallpapers = QSet<QString>(ignored.begin(), ignored.end());

  mAutoAcknowledge = settings()->value("autoAcknowledge", false).toBool();
  mAutoAcknowledgeInterval =
      settings()->value("autoAcknowledgeInterval", 1).toUInt();
  mAutoReload = settings()->value("autoReload", false).toBool();
  mResultsCutoff = settings()->value("resultsCutoff", 100).toUInt();
  mSearchTerm = settings()->value("searchTerm", "nature").toString();
  mUpdateInterval = settings()->value("updateInterval", -1).toUInt();

  if (mAutoReload) {
    qDebug() << "autoReload" << mUpdateInterval;
    mUpdateTimer->setInterval(mUpdateInterval);
    mUpdateTimer->start();
  } else {
    mUpdateTimer->stop();
  }

  if (mAutoAcknowledge) {
    qDebug() << "autoAcknowledge" << mAutoAcknowledgeInterval;
  }
}
