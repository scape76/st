#include "../include/CompositeTask.h"
#include "../include/Subject.h"
#include <algorithm>
#include <iostream>

CompositeTask::CompositeTask(const std::string &title, const DateTime &deadline,
                             const std::string &description)
    : Task(title, deadline, description) {}

void CompositeTask::addChildTask(std::shared_ptr<Task> task) {
  if (!task) {
    return;
  }

  auto it = std::find_if(childTasks.begin(), childTasks.end(),
                         [&](const std::shared_ptr<Task> &a) {
                           return a->getTitle() == task->getTitle();
                         });

  if (it == childTasks.end()) {
    childTasks.push_back(task);

    if (getSubject()) {
      task->setSubject(getSubject());
    }
  } else {
    std::cout << "Child task with title '" << task->getTitle()
              << "' already exists in this composite task." << std::endl;
  }
}

void CompositeTask::removeChildTask(size_t index) {
  if (index < childTasks.size()) {
    childTasks.erase(childTasks.begin() + index);
  } else {
    std::cout << "Invalid child task index." << std::endl;
  }
}

void CompositeTask::removeChildTask(const std::string &title) {
  auto it = std::find_if(
      childTasks.begin(), childTasks.end(),
      [&](const std::shared_ptr<Task> &a) { return a->getTitle() == title; });

  if (it != childTasks.end()) {
    childTasks.erase(it);
  } else {
    std::cout << "Child task with title '" << title
              << "' not found in this composite task." << std::endl;
  }
}

std::vector<std::shared_ptr<Task>> CompositeTask::getChildTasks() const {
  return childTasks;
}

std::shared_ptr<Task>
CompositeTask::findChildTask(const std::string &title) const {
  auto it = std::find_if(
      childTasks.begin(), childTasks.end(),
      [&](const std::shared_ptr<Task> &a) { return a->getTitle() == title; });

  return (it != childTasks.end()) ? *it : nullptr;
}

void CompositeTask::displayInfo() const {
  Task::displayInfo();

  std::cout << "Child Tasks: " << childTasks.size() << std::endl;

  for (size_t i = 0; i < childTasks.size(); ++i) {
    std::cout << "  " << (i + 1) << ". " << childTasks[i]->getTitle() << " ("
              << childTasks[i]->getType() << ")";

    if (childTasks[i]->isCompleted()) {
      std::cout << " - Completed: " << childTasks[i]->getMarks() << " marks";
    } else {
      std::cout << " - Not completed";
    }

    std::cout << std::endl;
  }
}

bool CompositeTask::isCompleted() const {
  if (childTasks.empty()) {
    return Task::isCompleted();
  }

  return std::all_of(
      childTasks.begin(), childTasks.end(),
      [](const std::shared_ptr<Task> &a) { return a->isCompleted(); });
}

int CompositeTask::getMarks() const {
  if (childTasks.empty()) {
    return Task::getMarks();
  }

  int completedCount = 0;
  int totalMarks = 0;

  for (const auto &task : childTasks) {
    if (task->isCompleted()) {
      totalMarks += task->getMarks();
      completedCount++;
    }
  }

  return completedCount > 0 ? totalMarks / completedCount : 0;
}

void CompositeTask::setCompleted(bool completed) {
  Task::setCompleted(completed);

  for (auto &task : childTasks) {
    task->setCompleted(completed);
  }
}

void CompositeTask::setMarks(int marks) {
  Task::setMarks(marks);

  if (isCompleted()) {
    for (auto &task : childTasks) {
      task->setMarks(marks);
    }
  }
}

void CompositeTask::setSubject(std::shared_ptr<Subject> subject) {
  Task::setSubject(subject);

  for (auto &task : childTasks) {
    task->setSubject(subject);
  }
}

int CompositeTask::getTotalChildCount() const {
  int count = childTasks.size();

  for (const auto &task : childTasks) {
    if (auto compositeTask = std::dynamic_pointer_cast<CompositeTask>(task)) {
      count += compositeTask->getTotalChildCount();
    }
  }

  return count;
}

std::shared_ptr<Task>
CompositeTaskFactory::createTask(const std::string &title,
                                 const DateTime &deadline,
                                 const std::string &description) {
  return std::make_shared<CompositeTask>(title, deadline, description);
}
