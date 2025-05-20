#include "SystemFacade.h"
#include "AssignmentBuilder.h"
#include "Exceptions.h"
#include <iostream>

SystemFacade::SystemFacade(std::unique_ptr<NotificationFactory> notifFactory)
    : notificationService(std::move(notifFactory)) {}

void SystemFacade::addSubject(int id, const std::string &name) {
  if (dataStore.subjects.count(id)) {
    throw ValidationError("Subject with ID " + std::to_string(id) +
                          " already exists.");
  }
  dataStore.subjects[id] = {id, name};
  std::cout << "📚 Subject '" << name << "' (ID: " << id << ") added."
            << std::endl;
}

int SystemFacade::createAssignment(int subjectId, const AssignmentDto &dto) {
  if (!dataStore.subjects.count(subjectId)) {
    throw SubjectNotFoundError("Subject with ID " + std::to_string(subjectId) +
                               " not found.");
  }
  if (dto.name.empty() || dto.deadline.empty()) {
    throw ValidationError("Assignment name and deadline are required.");
  }

  AssignmentBuilder builder(subjectId);
  builder.withName(dto.name).withDeadline(dto.deadline);
  std::unique_ptr<Assignment> assignment = builder.build();
  int assignmentId = assignment->getAssignmentId();

  std::cout << "📝 Assignment '" << assignment->getName()
            << "' (ID: " << assignmentId
            << ") created for Subject ID: " << subjectId << "." << std::endl;

  // Schedule notifications if specified
  for (const auto &notifSetting : dto.notificationSettings) {
    notificationService.scheduleNotification(*assignment, notifSetting,
                                             dataStore);
  }

  dataStore.assignments[assignmentId] = std::move(assignment);
  return assignmentId;
}

void SystemFacade::markAssignmentAsCompleted(int assignmentId,
                                             std::optional<int> grade) {
  auto it = dataStore.assignments.find(assignmentId);
  if (it == dataStore.assignments.end()) {
    throw AssignmentNotFoundError("Assignment with ID " +
                                  std::to_string(assignmentId) + " not found.");
  }
  Assignment *assignment = it->second.get();
  assignment->markCompleted(grade); // Throws if already completed
  std::cout << "✅ Assignment '" << assignment->getName()
            << "' (ID: " << assignmentId << ") marked as completed.";
  if (grade.has_value()) {
    std::cout << " Grade: " << grade.value();
  }
  std::cout << std::endl;

  notificationService.cancelNotificationsForAssignment(assignmentId, dataStore);
}

const Assignment &SystemFacade::getAssignment(int assignmentId) const {
  auto it = dataStore.assignments.find(assignmentId);
  if (it == dataStore.assignments.end()) {
    throw AssignmentNotFoundError("Assignment with ID " +
                                  std::to_string(assignmentId) + " not found.");
  }
  return *it->second;
}

void SystemFacade::scheduleNotificationForAssignment(
    int assignmentId, const NotificationSettingDto &notificationSettingDto) {
  auto it = dataStore.assignments.find(assignmentId);
  if (it == dataStore.assignments.end()) {
    throw AssignmentNotFoundError("Assignment with ID " +
                                  std::to_string(assignmentId) +
                                  " not found for scheduling notification.");
  }
  notificationService.scheduleNotification(*(it->second),
                                           notificationSettingDto, dataStore);
}

void SystemFacade::sendNotification(int notificationId) {
  try {
    if (notificationService.sendNotification(notificationId, dataStore)) {
      std::cout << "   Notification " << notificationId
                << " processed successfully." << std::endl;
    }
  } catch (const NotificationNotFoundError &e) {
    std::cerr << "Error sending notification: " << e.what() << std::endl;
  } catch (const NotifyError &e) {
    std::cerr << "Error during notification sending process: " << e.what()
              << std::endl;
  }
}

void SystemFacade::sendAllDueNotifications() {
  std::cout << "\n📬 Attempting to send all (due) notifications..."
            << std::endl;
  if (dataStore.scheduledNotifications.empty()) {
    std::cout << "No notifications scheduled." << std::endl;
    return;
  }
  // In a real system, this would check current time against
  // notification.sendTime For this example, we'll just iterate and try to send
  // unsent ones.
  for (auto const &[id, notification_ptr] : dataStore.scheduledNotifications) {
    if (notification_ptr && !notification_ptr->isSent()) {
      // Check if assignment still exists and is not completed
      auto assignmentIt =
          dataStore.assignments.find(notification_ptr->getAssignmentId());
      if (assignmentIt != dataStore.assignments.end() &&
          assignmentIt->second->getStatus() == AssignmentStatus::PENDING) {
        sendNotification(id);
      } else if (assignmentIt != dataStore.assignments.end() &&
                 assignmentIt->second->getStatus() ==
                     AssignmentStatus::COMPLETED) {
        std::cout << "   Skipping notification ID " << id
                  << " for completed assignment ID "
                  << notification_ptr->getAssignmentId() << std::endl;
      } else if (assignmentIt == dataStore.assignments.end()) {
        std::cout << "   Skipping notification ID " << id
                  << " as assignment ID " << notification_ptr->getAssignmentId()
                  << " no longer exists." << std::endl;
      }
    }
  }
}

void SystemFacade::displayAllAssignments() const {
  std::cout << "\n--- All Assignments ---" << std::endl;
  if (dataStore.assignments.empty()) {
    std::cout << "No assignments in the system." << std::endl;
    return;
  }
  for (const auto &pair : dataStore.assignments) {
    const auto &assignment = pair.second;
    std::cout << "ID: " << assignment->getAssignmentId() << ", Name: \""
              << assignment->getName()
              << "\", SubjectID: " << assignment->getSubjectId()
              << ", Deadline: " << assignment->getDeadline() << ", Status: "
              << (assignment->getStatus() == AssignmentStatus::PENDING
                      ? "Pending"
                      : "Completed");
    if (assignment->getGrade().has_value()) {
      std::cout << ", Grade: " << assignment->getGrade().value();
    }
    std::cout << std::endl;
  }
  std::cout << "-----------------------" << std::endl;
}

void SystemFacade::displayAllNotifications() const {
  std::cout << "\n--- Scheduled Notifications ---" << std::endl;
  if (dataStore.scheduledNotifications.empty()) {
    std::cout << "No notifications scheduled." << std::endl;
    return;
  }
  for (const auto &pair : dataStore.scheduledNotifications) {
    const auto &notification = pair.second;
    std::cout << "ID: " << notification->getNotificationId()
              << ", AssignmentID: " << notification->getAssignmentId()
              << ", Message: \"" << notification->getMessage()
              << "\", Send Time: " << notification->getSendTime()
              << ", Sent: " << (notification->isSent() ? "Yes" : "No")
              << std::endl;
  }
  std::cout << "---------------------------" << std::endl;
}
