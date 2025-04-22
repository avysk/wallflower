#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QString>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../downloader.h"

class MockPathmaker : public IPathmaker {

public:
  MOCK_METHOD(void, makePath, (const QString &path), (const, override));
};

TEST(DownloaderTest, CorrectPathCreated) {

  int argc = 0;
  char *argv[] = {nullptr};

  QCoreApplication app(argc, argv);

  std::unique_ptr<MockPathmaker> mockPathmaker =
      std::make_unique<MockPathmaker>();

  EXPECT_CALL(*mockPathmaker, makePath(::testing::Truly([](const QString &s) {
    return s.endsWith("/wallflower");
  }))).Times(1);

  Downloader downloader(std::move(mockPathmaker));
}
