#include "NotificationService.h"
#include "Exceptions.h"
#include <algorithm> // For std::remove_if
#include <iostream>

NotificationService::NotificationService(
    std::unique_ptr<NotificationFactory> factory)
    : notificationFactory(std::move(factory)) {}

// Simplified time calculation
std::string
NotificationService::calculateSendTime(const std::string &deadline,
                                       const std::string &relativeTime) const {
  // This is a placeholder. A real implementation would parse dates and subtract
  // durations. For example, "2 days before deadline", "on deadline"
  if (relativeTime == "on deadline") {
    return deadline;
  } else if (relativeTime == "1 day before deadline") {
    // Simplistic example, assuming YYYY-MM-DD format and just modifying the
    // string
    if (deadline.length() >= 10) {
      // This is NOT robust date math, just a placeholder
      return deadline.substr(0, 8) +
             std::to_string(std::stoi(deadline.substr(8, 2)) - 1) +
             deadline.substr(10) + " (approx. 1 day before)";
    }
  }
  return deadline + " (" + relativeTime + ")"; // Default if no specific logic
}

void NotificationService::scheduleNotification(
    const Assignment &assignment, const NotificationSettingDto &settings,
    DataStore &dataStore) {
  if (!notificationFactory) {
    throw std::runtime_error(
        "Notification factory not set in NotificationService.");
  }
  if (assignment.getDeadline().empty()) {
    throw ValidationError(
        "Cannot schedule notification for assignment without a deadline.");
  }

  std::string actualSendTime =
      calculateSendTime(assignment.getDeadline(), settings.relativeSendTime);
  std::unique_ptr<Notification> notification =
      notificationFactory->createNotification(assignment.getAssignmentId(),
                                              settings.messageContent,
                                              actualSendTime);
  std::cout << "🗓️ Scheduled Notification ID: "
            << notification->getNotificationId()
            << " for Assignment ID: " << assignment.getAssignmentId() << " at "
            << actualSendTime << std::endl;
  dataStore.scheduledNotifications[notification->getNotificationId()] =
      std::move(notification);
}

bool NotificationService::sendNotification(int notificationId,
                                           DataStore &dataStore) {
  auto it = dataStore.scheduledNotifications.find(notificationId);
  if (it == dataStore.scheduledNotifications.end() || !it->second) {
    throw NotificationNotFoundError("Notification with ID " +
                                    std::to_string(notificationId) +
                                    " not found.");
  }

  Notification *notification = it->second.get();
  // Simulate time check: in a real system, you'd check if it's time to send.
  // For this example, we just send it if it's not already sent.
  if (!notification->isSent()) {
    try {
      notification->send(); // Polymorphic call
      notification->markAsSent();
      return true;
    } catch (const std::exception &e) {
      throw NotifyError("Failed to send notification " +
                        std::to_string(notificationId) + ": " + e.what());
    }
  } else {
    std::cout << "Notification " << notificationId << " was already sent."
              << std::endl;
    return false; // Already sent
  }
}

void NotificationService::cancelNotificationsForAssignment(
    int assignmentId, DataStore &dataStore) {
  std::cout << "🗑️ Cancelling notifications for Assignment ID: " << assignmentId
            << std::endl;
  // Iterate and remove notifications associated with the assignmentId
  for (auto it = dataStore.scheduledNotifications.begin();
       it != dataStore.scheduledNotifications.end();) {
    if (it->second && it->second->getAssignmentId() == assignmentId) {
      std::cout << "   - Notification ID " << it->first << " cancelled."
                << std::endl;
      it = dataStore.scheduledNotifications.erase(
          it); // Erase and get next valid iterator
    } else {
      ++it;
    }
  }
}
