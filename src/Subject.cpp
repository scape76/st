#include "../include/Subject.h"
#include "../include/Task.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

Subject::Subject(const std::string &name, const std::string &code,
                 const std::string &description)
    : name(name), code(code), description(description) {}

std::string Subject::getName() const { return name; }
std::string Subject::getCode() const { return code; }
std::string Subject::getDescription() const { return description; }
std::vector<std::shared_ptr<Task>> Subject::getTasks() const { return tasks; }

void Subject::addTask(std::shared_ptr<Task> task) {
  if (!task) {
    std::cout << "Cannot add a null task." << std::endl;
    return;
  }

  auto it = std::find_if(tasks.begin(), tasks.end(),
                         [&](const std::shared_ptr<Task> &existingTask) {
                           return existingTask &&
                                  existingTask->getTitle() == task->getTitle();
                         });

  if (it == tasks.end()) {
    tasks.push_back(task);
    task->setSubject(
        std::shared_ptr<Subject>(this, [](Subject *) { /* no-op deleter */ }));
  } else {
    std::cout << "Task with title '" << task->getTitle()
              << "' already exists in subject '" << this->name << "'."
              << std::endl;
  }
}

void Subject::removeTask(const std::string &title) {
  auto it = std::find_if(tasks.begin(), tasks.end(),
                         [&](const std::shared_ptr<Task> &task_ptr) {
                           return task_ptr && task_ptr->getTitle() == title;
                         });

  if (it != tasks.end()) {
    if ((*it)->getSubject().get() == this) {
      (*it)->setSubject(nullptr);
    }
    tasks.erase(it);
    std::cout << "Task with title '" << title << "' removed from subject '"
              << this->name << "'." << std::endl;
  } else {
    std::cout << "Task with title '" << title << "' not found in subject '"
              << this->name << "'." << std::endl;
  }
}

std::shared_ptr<Task> Subject::findTask(const std::string &title) const {
  auto it = std::find_if(tasks.begin(), tasks.end(),
                         [&](const std::shared_ptr<Task> &task_ptr) {
                           return task_ptr && task_ptr->getTitle() == title;
                         });

  return (it != tasks.end()) ? *it : nullptr;
}

void Subject::displayInfo() const {
  std::cout << "Subject: " << name << " (" << code << ")" << std::endl;

  if (!description.empty()) {
    std::cout << "Description: " << description << std::endl;
  }

  std::cout << "Tasks (" << tasks.size() << "):" << std::endl;
  if (tasks.empty()) {
    std::cout << "  No tasks assigned to this subject." << std::endl;
  } else {
    for (const auto &task : tasks) {
      if (task) {
        std::cout << "  - " << task->getTitle() << " (" << task->getType()
                  << ")" << std::endl;
      }
    }
  }
}

SubjectBuilder::SubjectBuilder() : name(""), code(""), description("") {}

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
  if (name.empty() || code.empty()) {
    throw std::invalid_argument(
        "Subject name and code are required to build a Subject.");
  }
  return std::make_shared<Subject>(name, code, description);
}
