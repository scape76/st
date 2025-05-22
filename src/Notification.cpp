#include "../include/Notification.h"
#include "../include/Subject.h"
#include "../include/Task.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string formatDateTimeNotif(const DateTime &dt) {
  auto time = std::chrono::system_clock::to_time_t(dt);
  std::tm tm_local = {};

#ifdef _WIN32
  localtime_s(&tm_local, &time);
#else
  tm_local = *std::localtime(&time);
#endif

  std::stringstream ss;
  ss << std::put_time(&tm_local, "%Y-%m-%d %H:%M");
  return ss.str();
}

std::unique_ptr<NotificationManager, std::function<void(NotificationManager *)>>
    NotificationManager::instance{nullptr, [](NotificationManager *p) {}};
std::mutex NotificationManager::mutex;

Notification::Notification(const std::string &message,
                           const DateTime &triggerTime,
                           std::shared_ptr<Task> task)
    : message(message), triggerTime(triggerTime), task(task) {}

std::string Notification::getMessage() const { return message; }

DateTime Notification::getTriggerTime() const { return triggerTime; }

std::shared_ptr<Task> Notification::getTask() const {
  return task.lock();
}

void Notification::setMessage(const std::string &message) {
  this->message = message;
}

void Notification::setTriggerTime(const DateTime &triggerTime) {
  this->triggerTime = triggerTime;
}

bool Notification::shouldTrigger(const DateTime &currentTime) const {
  return currentTime >= triggerTime;
}

void Notification::display() const {
  std::cout << "NOTIFICATION: " << message << std::endl;

  auto taskPtr = task.lock();
  if (taskPtr) {
    std::cout << "For task: " << taskPtr->getTitle() << std::endl;
    std::cout << "Due: " << formatDateTimeNotif(taskPtr->getDeadline())
              << std::endl;
  }

  std::cout << "Trigger time: " << formatDateTimeNotif(triggerTime)
            << std::endl;
}

DeadlineNotification::DeadlineNotification(const std::string &message,
                                           std::shared_ptr<Task> task,
                                           int daysBeforeDeadline)
    : Notification(
          message,
          std::chrono::system_clock::time_point(),
          task),
      daysBeforeDeadline(daysBeforeDeadline) {

  auto deadline = task->getDeadline();
  auto triggerTime = deadline - std::chrono::hours(24 * daysBeforeDeadline);
  setTriggerTime(triggerTime);
}

int DeadlineNotification::getDaysBeforeDeadline() const {
  return daysBeforeDeadline;
}

void DeadlineNotification::setDaysBeforeDeadline(int days) {
  daysBeforeDeadline = days;

  auto taskPtr = getTask();
  if (taskPtr) {
    auto deadline = taskPtr->getDeadline();
    auto triggerTime = deadline - std::chrono::hours(24 * daysBeforeDeadline);
    setTriggerTime(triggerTime);
  }
}

bool DeadlineNotification::shouldTrigger(const DateTime &currentTime) const {
  auto taskPtr = getTask();
  if (!taskPtr) {
    return false;
  }

  auto deadline = taskPtr->getDeadline();
  auto timeToDeadline =
      std::chrono::duration_cast<std::chrono::hours>(deadline - currentTime)
          .count();

  auto upperBound = daysBeforeDeadline * 24;
  auto lowerBound = upperBound - 24;

  return (timeToDeadline <= upperBound && timeToDeadline > lowerBound);
}

void DeadlineNotification::display() const {
  auto taskPtr = getTask();
  if (!taskPtr) {
    std::cout << "NOTIFICATION: Task no longer exists." << std::endl;
    return;
  }

  std::cout << "DEADLINE REMINDER: " << getMessage() << std::endl;
  std::cout << "Task: " << taskPtr->getTitle() << std::endl;
  std::cout << "Due in " << daysBeforeDeadline << " days on "
            << formatDateTimeNotif(taskPtr->getDeadline()) << std::endl;

  if (auto subject = taskPtr->getSubject()) {
    std::cout << "Subject: " << subject->getName() << " (" << subject->getCode()
              << ")" << std::endl;
  }
}

NotificationManager &NotificationManager::getInstance() {
  std::lock_guard<std::mutex> lock(mutex);
  if (instance == nullptr) {
    instance.reset(new NotificationManager());
  }
  return *instance.get();
}

void NotificationManager::addNotification(
    std::shared_ptr<Notification> notification) {
  notifications.push_back(notification);

  if (auto task = notification->getTask()) {
    task->addNotification(notification);
  }
}

void NotificationManager::removeNotification(size_t index) {
  if (index < notifications.size()) {
    if (auto task = notifications[index]->getTask()) {
      auto taskNotifs = task->getNotifications();
      auto it =
          std::find(taskNotifs.begin(), taskNotifs.end(), notifications[index]);
      if (it != taskNotifs.end()) {
        task->removeNotification(std::distance(taskNotifs.begin(), it));
      }
    }

    notifications.erase(notifications.begin() + index);
  } else {
    std::cout << "Invalid notification index." << std::endl;
  }
}

void NotificationManager::checkNotifications() {
  auto now = std::chrono::system_clock::now();
  bool foundNotifications = false;

  std::cout << "Current time: " << formatDateTimeNotif(now) << std::endl;

  for (const auto &notification : notifications) {
    if (notification->shouldTrigger(now)) {
      notification->display();
      foundNotifications = true;
    }
  }

  if (!foundNotifications) {
    if (notifications.empty()) {
      std::cout << "You have no notifications set up." << std::endl;
    } else {
      std::cout << "No notifications are due at this time." << std::endl;

      std::cout << "You have " << notifications.size()
                << " notification(s) scheduled." << std::endl;

      if (auto deadlineNotif = std::dynamic_pointer_cast<DeadlineNotification>(
              notifications[0])) {
        if (auto task = deadlineNotif->getTask()) {
          std::cout << "Next deadline: " << task->getTitle() << " on "
                    << formatDateTimeNotif(task->getDeadline()) << std::endl;
        }
      }
    }
  }
}

std::vector<std::shared_ptr<Notification>>
NotificationManager::getNotifications() const {
  return notifications;
}

std::vector<std::shared_ptr<Notification>>
NotificationManager::getNotificationsForTask(std::shared_ptr<Task> task) const {

  std::vector<std::shared_ptr<Notification>> result;

  for (const auto &notification : notifications) {
    if (auto notifTask = notification->getTask()) {
      if (notifTask == task) {
        result.push_back(notification);
      }
    }
  }

  return result;
}