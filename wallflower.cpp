#include <QMenu>
#include <QObject>
#include <QRandomGenerator>
#include <QWidgetAction>

#include "basesearcher.h"
#include "searcher.h"
#include "sites/wallheavensearcher.h"
#include "wallflower.h"

Wallflower::Wallflower(const ILXQtPanelPluginStartupInfo &startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo) {
  mBusyIcon = QIcon::fromTheme("view-refresh");
  mErrorIcon = QIcon::fromTheme("dialog-error");
  mNormalIcon = QIcon::fromTheme("desktop-preferences-wallpaper");
  mButton = std::make_unique<QToolButton>();
  mButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
  mButton->setPopupMode(QToolButton::InstantPopup);
  auto menu = new QMenu(mButton.get());
  menu->addAction("busy", this, [this]() { busySlot("busy"); });
  menu->addAction("error", this, [this]() { errorSlot("error"); });
  menu->addAction("normal", this, [this]() { normalSlot(); });
  menu->addAction("search", this, [this]() {
    busySlot("searching");
    mSearcher->searchWallpapers("nature");
  });
  mButton->setMenu(menu);
  normalSlot();

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
  mButton->setIcon(mBusyIcon);
  setMessage(state);
}

void Wallflower::errorSlot(const QString &message) {
  mButton->setIcon(mErrorIcon);
  setMessage(message);
}

void Wallflower::normalSlot(const QString &state) {
  mButton->setIcon(mNormalIcon);
  mButton->update();
  setMessage(state);
}

void Wallflower::searchDone(QVector<Picture> &result) {
  this->normalSlot(QString("Got %1 results").arg(result.length()));
  int randomIndex = QRandomGenerator::global()->bounded(result.size());
  auto p = result[randomIndex];
  qDebug() << randomIndex << p.id << p.path;
}
