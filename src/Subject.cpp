#include "../include/Subject.h"
#include "../include/Task.h"
#include <algorithm>
#include <iostream>

// Subject implementation
Subject::Subject(const std::string &name, const std::string &code,
                 const std::string &description)
    : name(name), code(code), description(description) {}

std::string Subject::getName() const { return name; }

std::string Subject::getCode() const { return code; }

std::string Subject::getDescription() const { return description; }

std::vector<std::shared_ptr<Task>> Subject::getTasks() const { return tasks; }

void Subject::addTask(std::shared_ptr<Task> task) {
  // Avoid duplicates and set the subject reference in the task
  auto it = std::find_if(tasks.begin(), tasks.end(),
                         [&](const std::shared_ptr<Task> &a) {
                           return a->getTitle() == task->getTitle();
                         });

  if (it == tasks.end()) {
    tasks.push_back(task);
    task->setSubject(std::shared_ptr<Subject>(
        this, [](Subject *) {})); // Non-owning reference
  } else {
    std::cout << "Task with title '" << task->getTitle()
              << "' already exists in this subject." << std::endl;
  }
}

void Subject::removeTask(const std::string &title) {
  auto it = std::find_if(
      tasks.begin(), tasks.end(),
      [&](const std::shared_ptr<Task> &a) { return a->getTitle() == title; });

  if (it != tasks.end()) {
    tasks.erase(it);
  } else {
    std::cout << "Task with title '" << title << "' not found in this subject."
              << std::endl;
  }
}

std::shared_ptr<Task> Subject::findTask(const std::string &title) const {
  auto it = std::find_if(
      tasks.begin(), tasks.end(),
      [&](const std::shared_ptr<Task> &a) { return a->getTitle() == title; });

  return (it != tasks.end()) ? *it : nullptr;
}

void Subject::displayInfo() const {
  std::cout << "Subject: " << name << " (" << code << ")" << std::endl;

  if (!description.empty()) {
    std::cout << "Description: " << description << std::endl;
  }

  std::cout << "Tasks: " << tasks.size() << std::endl;

  for (const auto &task : tasks) {
    std::cout << "  - " << task->getTitle() << " (" << task->getType() << ")"
              << std::endl;
  }
}

// Memento pattern methods
std::shared_ptr<SubjectMemento> Subject::createMemento() const {
  std::vector<std::string> taskTitles;
  for (const auto &task : tasks) {
    taskTitles.push_back(task->getTitle());
  }

  return std::make_shared<SubjectMemento>(name, code, description, taskTitles);
}

void Subject::restoreFromMemento(
    const std::shared_ptr<SubjectMemento> &memento) {
  name = memento->getName();
  code = memento->getCode();
  description = memento->getDescription();

  // Note: Task references can't be fully restored from the memento
  // as we only store titles. The caller must reconnect tasks separately.
}

// SubjectBuilder implementation
SubjectBuilder &SubjectBuilder::setName(const std::string &name) {
  this->name = name;
  return *this;
}

SubjectBuilder &SubjectBuilder::setCode(const std::string &code) {
  this->code = code;
  return *this;
}

SubjectBuilder &SubjectBuilder::setDescription(const std::string &description) {
  this->description = description;
  return *this;
}

std::shared_ptr<Subject> SubjectBuilder::build() const {
  // Validate required fields
  if (name.empty() || code.empty()) {
    throw std::invalid_argument("Subject name and code are required");
  }

  return std::make_shared<Subject>(name, code, description);
}
