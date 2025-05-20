#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

class Notification {
public:
  Notification(int id, int assignmentId, std::string message,
               std::string sendTime);

  int getNotificationId() const;
  int getAssignmentId() const;
  const std::string &getMessage() const;
  const std::string &getSendTime() const; // For simplicity, time is a string
  bool isSent() const;

  void markAsSent();
  virtual void send() const = 0; // Pure virtual, making Notification abstract
  virtual ~Notification() = default;

protected:
  int notificationId;
  int assignmentId;
  std::string message;
  std::string sendTime;
  bool sent;
};

class SimpleNotification : public Notification {
public:
  SimpleNotification(int id, int assignmentId, std::string message,
                     std::string sendTime);
  void send() const override;
};

#endif // NOTIFICATION_H
