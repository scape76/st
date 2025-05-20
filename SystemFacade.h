#ifndef SYSTEMFACADE_H
#define SYSTEMFACADE_H

#include "DataStore.h"
#include "Dtos.h"
#include "NotificationService.h"
#include <optional>
#include <string>
#include <vector>

class SystemFacade {
public:
  SystemFacade(std::unique_ptr<NotificationFactory> notifFactory);

  // Subject management (simplified)
  void addSubject(int id, const std::string &name);

  // Assignment operations
  int createAssignment(int subjectId, const AssignmentDto &dto);
  void markAssignmentAsCompleted(int assignmentId, std::optional<int> grade);
  const Assignment &getAssignment(int assignmentId) const;

  // Notification operations
  void scheduleNotificationForAssignment(
      int assignmentId, const NotificationSettingDto &notificationSettingDto);
  void sendNotification(
      int notificationId);        // Send a specific notification by its ID
  void sendAllDueNotifications(); // Placeholder for a more complex due check

  void displayAllAssignments() const;
  void displayAllNotifications() const;

private:
  DataStore dataStore;
  NotificationService notificationService;
};

#endif // SYSTEMFACADE_H
