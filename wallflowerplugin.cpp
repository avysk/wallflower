#include <QTimer>

#include "wallflowerplugin.h"

Wallflower::Wallflower(const ILXQtPanelPluginStartupInfo &startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo) {
  button = new QToolButton();
  button->setAutoRaise(true);
  QTimer::singleShot(0, [this] {
    this->button->setText("Wallflower");
    this->button->show();
    this->button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    this->button->update();
  });
}

Wallflower::~Wallflower() = default;

void Wallflower::realign() {
  button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}
