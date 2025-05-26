#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class Task;

using DateTime = std::chrono::system_clock::time_point;

class Notification {
private:
  std::string message;
  DateTime triggerTime;
  std::weak_ptr<Task> task;

public:
  Notification(const std::string &message, const DateTime &triggerTime,
               std::shared_ptr<Task> task);

  virtual ~Notification() = default;

  std::string getMessage() const;
  DateTime getTriggerTime() const;
  std::shared_ptr<Task> getTask() const;

  void setMessage(const std::string &message);
  void setTriggerTime(const DateTime &triggerTime);

  virtual bool shouldTrigger(const DateTime &currentTime) const;

  virtual void display() const;
};

class DeadlineNotification : public Notification {
private:
  int daysBeforeDeadline;

public:
  DeadlineNotification(const std::string &message, std::shared_ptr<Task> task,
                       int daysBeforeDeadline);

  int getDaysBeforeDeadline() const;
  void setDaysBeforeDeadline(int days);

  bool shouldTrigger(const DateTime &currentTime) const override;
  void display() const override;
};

class NotificationManager {
private:
  NotificationManager() = default;
  ~NotificationManager() = default;

  NotificationManager(const NotificationManager &) = delete;
  NotificationManager &operator=(const NotificationManager &) = delete;

  static std::unique_ptr<NotificationManager,
                         std::function<void(NotificationManager *)>>
      instance;
  static std::mutex mutex;

  std::vector<std::shared_ptr<Notification>> notifications;

public:
  static NotificationManager &getInstance();

  void addNotification(std::shared_ptr<Notification> notification);

  void removeNotification(size_t index);

  void checkNotifications();

  std::vector<std::shared_ptr<Notification>> getNotifications() const;

  std::vector<std::shared_ptr<Notification>>
  getNotificationsForTask(std::shared_ptr<Task> task) const;
};

#endif