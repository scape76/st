#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <atomic>

class IdGenerator {
public:
  static IdGenerator &getInstance();
  int generateAssignmentId();
  int generateNotificationId();

  // Delete copy constructor and assignment operator
  IdGenerator(const IdGenerator &) = delete;
  IdGenerator &operator=(const IdGenerator &) = delete;

private:
  IdGenerator(); // Private constructor
  std::atomic<int> nextAssignmentId;
  std::atomic<int> nextNotificationId;
};

#endif // IDGENERATOR_H
