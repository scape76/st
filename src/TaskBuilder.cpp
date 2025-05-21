#include "../include/TaskBuilder.h"
#include <stdexcept>

// Constructor
TaskBuilder::TaskBuilder() { reset(); }

// Fluent interface methods
TaskBuilder &TaskBuilder::setTitle(const std::string &title) {
  this->title = title;
  return *this;
}

TaskBuilder &TaskBuilder::setDescription(const std::string &description) {
  this->description = description;
  return *this;
}

TaskBuilder &TaskBuilder::setDeadline(const DateTime &deadline) {
  this->deadline = deadline;
  return *this;
}

// Set which type of task to build
TaskBuilder &TaskBuilder::asLab() {
  factory = std::make_shared<LabFactory>();
  return *this;
}

TaskBuilder &TaskBuilder::asProject() {
  factory = std::make_shared<ProjectFactory>();
  return *this;
}

TaskBuilder &TaskBuilder::asExam() {
  factory = std::make_shared<ExamFactory>();
  return *this;
}

// Build the task
std::shared_ptr<Task> TaskBuilder::build() const {
  // Validate required fields
  if (title.empty()) {
    throw std::invalid_argument("Task title is required");
  }

  if (factory == nullptr) {
    throw std::invalid_argument(
        "Task type (lab/project/exam) must be specified");
  }

  // Create the task using the factory
  return factory->createTask(title, deadline, description);
}

// Reset the builder to create another task
void TaskBuilder::reset() {
  title.clear();
  description.clear();
  deadline = DateTime();
  factory = nullptr;
}
