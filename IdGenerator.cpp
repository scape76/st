#include "IdGenerator.h"

IdGenerator::IdGenerator() : nextAssignmentId(1), nextNotificationId(1) {}

IdGenerator &IdGenerator::getInstance() {
  static IdGenerator
      instance; // Guaranteed to be destroyed. Instantiated on first use.
  return instance;
}

int IdGenerator::generateAssignmentId() { return nextAssignmentId++; }

int IdGenerator::generateNotificationId() { return nextNotificationId++; }
