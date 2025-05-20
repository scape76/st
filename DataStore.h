#ifndef DATASTORE_H
#define DATASTORE_H

#include "Assignment.h"
#include "Notification.h"
#include "Subject.h"
#include <map>
#include <memory>
#include <vector>

class DataStore {
public:
  std::map<int, Subject> subjects;
  std::map<int, std::unique_ptr<Assignment>> assignments;
  // Storing notifications by their own ID for easier direct access if needed
  std::map<int, std::unique_ptr<Notification>> scheduledNotifications;

  // Helper to get notifications for a specific assignment
  std::vector<Notification *> getNotificationsForAssignment(int assignmentId);
};

#endif // DATASTORE_H
