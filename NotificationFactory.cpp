#include "NotificationFactory.h"
#include "IdGenerator.h"

std::unique_ptr<Notification> SimpleNotificationFactory::createNotification(
    int assignmentId, const std::string &message,
    const std::string &sendTime) const {
  int notificationId = IdGenerator::getInstance().generateNotificationId();
  return std::make_unique<SimpleNotification>(notificationId, assignmentId,
                                              message, sendTime);
}
