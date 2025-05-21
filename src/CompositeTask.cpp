#include "../include/CompositeTask.h"
#include "../include/Subject.h"
#include <algorithm>
#include <iostream>

// Constructor
CompositeTask::CompositeTask(const std::string &title, const DateTime &deadline,
                             const std::string &description)
    : Task(title, deadline, description) {}

// Add a child task
void CompositeTask::addChildTask(std::shared_ptr<Task> task) {
  // Don't add null tasks
  if (!task) {
    return;
  }

  // Check for duplicates
  auto it = std::find_if(childTasks.begin(), childTasks.end(),
                         [&](const std::shared_ptr<Task> &a) {
                           return a->getTitle() == task->getTitle();
                         });

  if (it == childTasks.end()) {
    childTasks.push_back(task);

    // Set the same subject for the child task
    if (getSubject()) {
      task->setSubject(getSubject());
    }
  } else {
    std::cout << "Child task with title '" << task->getTitle()
              << "' already exists in this composite task." << std::endl;
  }
}

// Remove a child task by index
void CompositeTask::removeChildTask(size_t index) {
  if (index < childTasks.size()) {
    childTasks.erase(childTasks.begin() + index);
  } else {
    std::cout << "Invalid child task index." << std::endl;
  }
}

// Remove a child task by title
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

// Get all child tasks
std::vector<std::shared_ptr<Task>> CompositeTask::getChildTasks() const {
  return childTasks;
}

// Find a child task by title
std::shared_ptr<Task>
CompositeTask::findChildTask(const std::string &title) const {
  auto it = std::find_if(
      childTasks.begin(), childTasks.end(),
      [&](const std::shared_ptr<Task> &a) { return a->getTitle() == title; });

  return (it != childTasks.end()) ? *it : nullptr;
}

// Override display to include child tasks
void CompositeTask::displayInfo() const {
  // First display this task's info using the base class method
  Task::displayInfo();

  // Then display child tasks
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

// Calculate overall completion status based on child tasks
bool CompositeTask::isCompleted() const {
  // If no child tasks, use the parent's completion status
  if (childTasks.empty()) {
    return Task::isCompleted();
  }

  // Check if all child tasks are completed
  return std::all_of(
      childTasks.begin(), childTasks.end(),
      [](const std::shared_ptr<Task> &a) { return a->isCompleted(); });
}

// Calculate average marks based on completed child tasks
int CompositeTask::getMarks() const {
  // If no child tasks, use the parent's marks
  if (childTasks.empty()) {
    return Task::getMarks();
  }

  // Count completed tasks and sum their marks
  int completedCount = 0;
  int totalMarks = 0;

  for (const auto &task : childTasks) {
    if (task->isCompleted()) {
      totalMarks += task->getMarks();
      completedCount++;
    }
  }

  // Calculate average marks
  return completedCount > 0 ? totalMarks / completedCount : 0;
}

// Propagate status changes to children
void CompositeTask::setCompleted(bool completed) {
  // Set the parent's status
  Task::setCompleted(completed);

  // Propagate to all children
  for (auto &task : childTasks) {
    task->setCompleted(completed);
  }
}

void CompositeTask::setMarks(int marks) {
  // Set the parent's marks
  Task::setMarks(marks);

  // Only propagate marks if marking as completed with uniform marks
  if (isCompleted()) {
    for (auto &task : childTasks) {
      task->setMarks(marks);
    }
  }
}

// Propagate subject changes to children
void CompositeTask::setSubject(std::shared_ptr<Subject> subject) {
  // Set the parent's subject
  Task::setSubject(subject);

  // Propagate to all children
  for (auto &task : childTasks) {
    task->setSubject(subject);
  }
}

// Get total number of child tasks (recursively)
int CompositeTask::getTotalChildCount() const {
  int count = childTasks.size();

  // Count child tasks of composite children
  for (const auto &task : childTasks) {
    if (auto compositeTask = std::dynamic_pointer_cast<CompositeTask>(task)) {
      count += compositeTask->getTotalChildCount();
    }
  }

  return count;
}

// Factory for creating composite tasks
std::shared_ptr<Task>
CompositeTaskFactory::createTask(const std::string &title,
                                 const DateTime &deadline,
                                 const std::string &description) {
  return std::make_shared<CompositeTask>(title, deadline, description);
}
