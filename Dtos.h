#ifndef DTOS_H
#define DTOS_H

#include <optional>
#include <string>
#include <vector>

struct NotificationSettingDto {
  std::string messageContent;
  std::string relativeSendTime; // e.g., "2 days before deadline", "on deadline"
};

struct AssignmentDto {
  std::string name;
  std::string deadline; // Format: "YYYY-MM-DD HH:MM"
  std::vector<NotificationSettingDto> notificationSettings;
};

#endif // DTOS_H
