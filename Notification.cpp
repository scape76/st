#include "Notification.h"
#include <iostream>

Notification::Notification(int id, int assignId, std::string msg,
                           std::string time)
    : notificationId(id), assignmentId(assignId), message(std::move(msg)),
      sendTime(std::move(time)), sent(false) {}

int Notification::getNotificationId() const { return notificationId; }
int Notification::getAssignmentId() const { return assignmentId; }
const std::string &Notification::getMessage() const { return message; }
const std::string &Notification::getSendTime() const { return sendTime; }
bool Notification::isSent() const { return sent; }

void Notification::markAsSent() { sent = true; }

// --- SimpleNotification ---
SimpleNotification::SimpleNotification(int id, int assignmentId,
                                       std::string message,
                                       std::string sendTime)
    : Notification(id, assignmentId, std::move(message), std::move(sendTime)) {}

void SimpleNotification::send() const {
  // In a real system, this would integrate with an email/SMS/push service
  std::cout << "📤 Sending Notification (ID: " << notificationId
            << ") for Assignment ID: " << assignmentId << std::endl;
  std::cout << "   Time: " << sendTime << std::endl;
  std::cout << "   Message: " << message << std::endl;
  if (sent) {
    std::cout
        << "   (Already marked as sent, this is a re-simulation of send logic)"
        << std::endl;
  }
}
