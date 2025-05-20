#ifndef NOTIFICATIONFACTORY_H
#define NOTIFICATIONFACTORY_H

#include "Notification.h"
#include <memory>
#include <string>

// Abstract Factory (Creator)
class NotificationFactory {
public:
  virtual ~NotificationFactory() = default;
  // The Factory Method
  virtual std::unique_ptr<Notification>
  createNotification(int assignmentId, const std::string &message,
                     const std::string &sendTime) const = 0;
};

// Concrete Factory (ConcreteCreator)
class SimpleNotificationFactory : public NotificationFactory {
public:
  std::unique_ptr<Notification>
  createNotification(int assignmentId, const std::string &message,
                     const std::string &sendTime) const override;
};

#endif // NOTIFICATIONFACTORY_H
