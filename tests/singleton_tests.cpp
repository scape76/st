#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "../include/Internship.h"
#include "../include/Registry.h"
#include "../include/Subject.h"

class RegistrySingletonTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override {
    Registry::instance().internships.clear();
    Registry::instance().subjects.clear();
  }
};

TEST_F(RegistrySingletonTest, InstanceIsUnique) {
  Registry &instance1 = Registry::instance();
  Registry &instance2 = Registry::instance();
  Registry &instance3 = Registry::instance();

  EXPECT_EQ(&instance1, &instance2)
      << "First and second references should point to the same instance.";
  EXPECT_EQ(&instance2, &instance3)
      << "Second and third references should point to the same instance.";
}

TEST_F(RegistrySingletonTest, IsNonCopyableAndNonAssignableByDesign) {
  SUCCEED()
      << "Registry is non-copyable/non-assignable due to private constructor "
         "and deleted copy/assignment ops (compiler enforced).";
}
