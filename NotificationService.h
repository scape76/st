#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include "Assignment.h" // For Assignment to get deadline
#include "DataStore.h"
#include "Dtos.h" // For NotificationSettingDto
#include "NotificationFactory.h"
#include <memory>

class NotificationService {
public:
  // Dependency Injection for the factory
  explicit NotificationService(std::unique_ptr<NotificationFactory> factory);

  void scheduleNotification(const Assignment &assignment,
                            const NotificationSettingDto &settings,
                            DataStore &dataStore);
  bool sendNotification(int notificationId, DataStore &dataStore);
  void cancelNotificationsForAssignment(int assignmentId, DataStore &dataStore);

private:
  std::unique_ptr<NotificationFactory> notificationFactory;
  // Helper to calculate actual send time based on deadline and relative setting
  std::string calculateSendTime(const std::string &deadline,
                                const std::string &relativeTime) const;
};

#endif // NOTIFICATIONSERVICE_H
