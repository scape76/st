#ifndef ASSIGNMENT_BUILDER_H
#define ASSIGNMENT_BUILDER_H

#include "Task.h"
#include <chrono>
#include <memory>
#include <string>

class TaskBuilder {
private:
  std::string title;
  std::string description;
  DateTime deadline;
  std::shared_ptr<TaskFactory> factory;

public:
  TaskBuilder();

  TaskBuilder &setTitle(const std::string &title);
  TaskBuilder &setDescription(const std::string &description);
  TaskBuilder &setDeadline(const DateTime &deadline);

  TaskBuilder &asLab();
  TaskBuilder &asProject();
  TaskBuilder &asExam();

  std::shared_ptr<Task> build() const;

  void reset();
};

#endif