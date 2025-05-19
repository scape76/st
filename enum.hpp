#pragma once

enum class AssignmentStatus { PENDING, COMPLETED };

enum class NotificationStatus {
  PENDING,
  PROCESSING,
  PROCESSED,
  ERRORED,
  CANCELLED
};
