#include <QString>

#include <gtest/gtest.h>

#include "../ident.h"

TEST(TestIdent, StableUuid) {
  QString first = Ident::generateUuid(42, "foo");
  QString second = Ident::generateUuid(42, "foo");
  ASSERT_STREQ(first.toUtf8(), second.toUtf8());
}

TEST(TestIdent, UuidDependsOnIndex) {
  QString first = Ident::generateUuid(42, "foo");
  QString second = Ident::generateUuid(43, "foo");
  ASSERT_STRNE(first.toUtf8(), second.toUtf8());
}

TEST(TestIdent, UuidDependsOnName) {
  QString first = Ident::generateUuid(42, "foo");
  QString second = Ident::generateUuid(42, "bar");
  ASSERT_STRNE(first.toUtf8(), second.toUtf8());
}
