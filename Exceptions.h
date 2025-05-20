#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class SubjectNotFoundError : public std::runtime_error {
public:
  explicit SubjectNotFoundError(const std::string &msg)
      : std::runtime_error(msg) {}
};

class ValidationError : public std::runtime_error {
public:
  explicit ValidationError(const std::string &msg) : std::runtime_error(msg) {}
};

class AssignmentNotFoundError : public std::runtime_error {
public:
  explicit AssignmentNotFoundError(const std::string &msg)
      : std::runtime_error(msg) {}
};

class AssignmentAlreadyCompletedError : public std::runtime_error {
public:
  explicit AssignmentAlreadyCompletedError(const std::string &msg)
      : std::runtime_error(msg) {}
};

class NotifyError : public std::runtime_error {
public:
  explicit NotifyError(const std::string &msg) : std::runtime_error(msg) {}
};

class NotificationNotFoundError : public std::runtime_error {
public:
  explicit NotificationNotFoundError(const std::string &msg)
      : std::runtime_error(msg) {}
};

#endif // EXCEPTIONS_H
