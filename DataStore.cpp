#include "DataStore.h"
#include <algorithm>

std::vector<Notification *>
DataStore::getNotificationsForAssignment(int assignmentId) {
  std::vector<Notification *> result;
  for (auto const &[id, notification_ptr] : scheduledNotifications) {
    if (notification_ptr->getAssignmentId() == assignmentId) {
      result.push_back(notification_ptr.get());
    }
  }
  return result;
}
