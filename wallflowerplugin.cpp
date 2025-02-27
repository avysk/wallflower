#include "wallflowerplugin.h"

Wallflower::Wallflower(const ILXQtPanelPluginStartupInfo &startupInfo)
    : QObject(), ILXQtPanelPlugin(startupInfo) {
  button.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  button.setAutoRaise(true);
  button.show();
}

Wallflower::~Wallflower() = default;

void Wallflower::realign() {
  button.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
