#include "../include/TaskDecorator.h"
#include "../include/Subject.h"
#include "../include/Task.h"
#include <algorithm>
#include <iostream>

// Base TaskDecorator implementation
TaskDecorator::TaskDecorator(std::shared_ptr<Task> assignment)
    : Task("", std::chrono::system_clock::now(), ""), wrappedTask(assignment) {}

std::string TaskDecorator::getTitle() const { return wrappedTask->getTitle(); }

std::string TaskDecorator::getDescription() const {
  return wrappedTask->getDescription();
}

DateTime TaskDecorator::getDeadline() const {
  return wrappedTask->getDeadline();
}

std::shared_ptr<Subject> TaskDecorator::getSubject() const {
  return wrappedTask->getSubject();
}

std::vector<std::shared_ptr<Notification>>
TaskDecorator::getNotifications() const {
  return wrappedTask->getNotifications();
}

bool TaskDecorator::isCompleted() const { return wrappedTask->isCompleted(); }

int TaskDecorator::getMarks() const { return wrappedTask->getMarks(); }

void TaskDecorator::setTitle(const std::string &title) {
  wrappedTask->setTitle(title);
}

void TaskDecorator::setDescription(const std::string &description) {
  wrappedTask->setDescription(description);
}

void TaskDecorator::setDeadline(const DateTime &deadline) {
  wrappedTask->setDeadline(deadline);
}

void TaskDecorator::setSubject(std::shared_ptr<Subject> subject) {
  wrappedTask->setSubject(subject);
}

void TaskDecorator::setCompleted(bool completed) {
  wrappedTask->setCompleted(completed);
}

void TaskDecorator::setMarks(int marks) { wrappedTask->setMarks(marks); }

void TaskDecorator::addNotification(
    std::shared_ptr<Notification> notification) {
  wrappedTask->addNotification(notification);
}

void TaskDecorator::removeNotification(size_t index) {
  wrappedTask->removeNotification(index);
}

void TaskDecorator::displayInfo() const { wrappedTask->displayInfo(); }

std::string TaskDecorator::getType() const { return wrappedTask->getType(); }

// PriorityTaskDecorator implementation
PriorityTaskDecorator::PriorityTaskDecorator(std::shared_ptr<Task> assignment,
                                             int priorityLevel)
    : TaskDecorator(assignment), priorityLevel(priorityLevel) {
  // Ensure priority level is within valid range
  if (priorityLevel < 1)
    this->priorityLevel = 1;
  if (priorityLevel > 5)
    this->priorityLevel = 5;
}

void PriorityTaskDecorator::setPriorityLevel(int level) {
  priorityLevel = level;
  if (priorityLevel < 1)
    priorityLevel = 1;
  if (priorityLevel > 5)
    priorityLevel = 5;
}

int PriorityTaskDecorator::getPriorityLevel() const { return priorityLevel; }

void PriorityTaskDecorator::displayInfo() const {
  wrappedTask->displayInfo();

  std::cout << "Priority: ";
  switch (priorityLevel) {
  case 1:
    std::cout << "Very Low";
    break;
  case 2:
    std::cout << "Low";
    break;
  case 3:
    std::cout << "Medium";
    break;
  case 4:
    std::cout << "High";
    break;
  case 5:
    std::cout << "Very High";
    break;
  default:
    std::cout << "Unknown";
  }
  std::cout << " (" << priorityLevel << ")" << std::endl;
}

// TaggedTaskDecorator implementation
TaggedTaskDecorator::TaggedTaskDecorator(
    std::shared_ptr<Task> assignment,
    const std::vector<std::string> &initialTags)
    : TaskDecorator(assignment), tags(initialTags) {}

void TaggedTaskDecorator::addTag(const std::string &tag) {
  // Check if tag already exists
  for (const auto &existingTag : tags) {
    if (existingTag == tag) {
      return; // Tag already exists
    }
  }

  tags.push_back(tag);
}

void TaggedTaskDecorator::removeTag(const std::string &tag) {
  auto it = std::find(tags.begin(), tags.end(), tag);

  if (it != tags.end()) {
    tags.erase(it);
  }
}

std::vector<std::string> TaggedTaskDecorator::getTags() const { return tags; }

void TaggedTaskDecorator::displayInfo() const {
  wrappedTask->displayInfo();

  if (!tags.empty()) {
    std::cout << "Tags: ";
    for (size_t i = 0; i < tags.size(); ++i) {
      std::cout << tags[i];
      if (i < tags.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << std::endl;
  }
}

// ColoredTaskDecorator implementation
ColoredTaskDecorator::ColoredTaskDecorator(std::shared_ptr<Task> assignment,
                                           Color color)
    : TaskDecorator(assignment), color(color) {}

void ColoredTaskDecorator::setColor(Color color) { this->color = color; }

ColoredTaskDecorator::Color ColoredTaskDecorator::getColor() const {
  return color;
}

std::string ColoredTaskDecorator::getColorName() const {
  switch (color) {
  case Color::RED:
    return "Red";
  case Color::YELLOW:
    return "Yellow";
  case Color::GREEN:
    return "Green";
  case Color::BLUE:
    return "Blue";
  case Color::PURPLE:
    return "Purple";
  default:
    return "Default";
  }
}

void ColoredTaskDecorator::displayInfo() const {
  wrappedTask->displayInfo();
  std::cout << "Color: " << getColorName() << std::endl;
}
