#ifndef ASSIGNMENT_BUILDER_H
#define ASSIGNMENT_BUILDER_H

#include "Task.h"
#include <chrono>
#include <memory>
#include <string>

// Builder Pattern for creating assignments
class TaskBuilder {
private:
  std::string title;
  std::string description;
  DateTime deadline;
  std::shared_ptr<TaskFactory> factory;

public:
  // Constructor
  TaskBuilder();

  // Fluent interface methods
  TaskBuilder &setTitle(const std::string &title);
  TaskBuilder &setDescription(const std::string &description);
  TaskBuilder &setDeadline(const DateTime &deadline);

  // Set which type of assignment to build
  TaskBuilder &asLab();
  TaskBuilder &asProject();
  TaskBuilder &asExam();

  // Build the assignment
  std::shared_ptr<Task> build() const;

  // Reset the builder to create another assignment
  void reset();
};

#endif // ASSIGNMENT_BUILDER_H
