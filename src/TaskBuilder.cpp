#include "../include/TaskBuilder.h"
#include <stdexcept>

TaskBuilder::TaskBuilder() { reset(); }

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

std::shared_ptr<Task> TaskBuilder::build() const {
  if (title.empty()) {
    throw std::invalid_argument("Task title is required");
  }

  if (factory == nullptr) {
    throw std::invalid_argument(
        "Task type (lab/project/exam) must be specified");
  }

  return factory->createTask(title, deadline, description);
}

void TaskBuilder::reset() {
  title.clear();
  description.clear();
  deadline = DateTime();
  factory = nullptr;
}
