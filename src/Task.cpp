#include "../include/Task.h"
#include "../include/Notification.h"
#include "../include/Subject.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string formatDateTime(const DateTime &dt) {
  auto time = std::chrono::system_clock::to_time_t(dt);
  std::tm tm_local = {};

#ifdef _WIN32
  localtime_s(&tm_local, &time);
#else
  tm_local = *std::localtime(&time);
#endif

  std::stringstream ss;
  ss << std::put_time(&tm_local, "%Y-%m-%d %H:%M");
  return ss.str();
}

Task::Task(const std::string &title, const DateTime &deadline,
           const std::string &description)
    : title(title), description(description), deadline(deadline),
      completed(false), marks(0), progress(0.0f), subject(nullptr) {}

std::string Task::getTitle() const { return title; }

std::string Task::getDescription() const { return description; }

DateTime Task::getDeadline() const { return deadline; }

std::shared_ptr<Subject> Task::getSubject() const { return subject; }

std::vector<std::shared_ptr<Notification>> Task::getNotifications() const {
  return notifications;
}

bool Task::isCompleted() const { return completed; }

int Task::getMarks() const { return marks; }

float Task::getProgress() const { return progress; }

void Task::setTitle(const std::string &title) { this->title = title; }

void Task::setDescription(const std::string &description) {
  this->description = description;
}

void Task::setDeadline(const DateTime &deadline) { this->deadline = deadline; }

void Task::setSubject(std::shared_ptr<Subject> subject) {
  this->subject = subject;
}

void Task::setCompleted(bool completed_status) {
  this->completed = completed_status;

  if (this->completed) {
    this->progress = 100.0f;
  } else {
    this->progress = 0.0f;
    this->marks = 0;
  }
}

void Task::setMarks(int marks) { this->marks = marks; }

void Task::setProgress(float progress) {
  if (progress < 0.0f)
    this->progress = 0.0f;
  else if (progress > 100.0f)
    this->progress = 100.0f;
  else
    this->progress = progress;

  if (this->progress == 100.0f && !this->completed) {
  } else if (this->progress < 100.0f && this->completed) {
    this->completed = false;
  }
}

void Task::addNotification(std::shared_ptr<Notification> notification) {
  notifications.push_back(notification);
}

void Task::removeNotification(size_t index) {
  if (index < notifications.size()) {
    notifications.erase(notifications.begin() + index);
  } else {
    std::cout << "Invalid notification index." << std::endl;
  }
}

void Task::displayInfo() const {
  std::cout << "Task: " << title << " (" << getType() << ")" << std::endl;

  if (!description.empty()) {
    std::cout << "Description: " << description << std::endl;
  }

  std::cout << "Deadline: " << formatDateTime(deadline) << std::endl;

  if (subject) {
    std::cout << "Subject: " << subject->getName() << " (" << subject->getCode()
              << ")" << std::endl;
  } else {
    std::cout << "Not assigned to any subject" << std::endl;
  }

  std::string current_status;
  if (completed) {
    current_status = "Completed";
  } else {
    bool overdue = deadline < std::chrono::system_clock::now();
    if (overdue) {
      current_status = "Overdue";
    } else if (progress > 0.0f && progress < 100.0f) {
      current_status = "In Progress";
    } else if (progress == 0.0f) {
      current_status = "Not Started";
    } else {
      current_status = "Pending Completion";
    }
  }
  std::cout << "Status: " << current_status << std::endl;

  if (completed) {
    std::cout << "Marks: " << marks << std::endl;
  } else if (progress > 0.0f) {
    std::cout << "Progress: " << std::fixed << std::setprecision(1) << progress
              << "%" << std::endl;
  }

  std::cout << "Notifications: " << notifications.size() << std::endl;
}

LabTask::LabTask(const std::string &title, const DateTime &deadline,
                 const std::string &description)
    : Task(title, deadline, description) {}

ProjectTask::ProjectTask(const std::string &title, const DateTime &deadline,
                         const std::string &description)
    : Task(title, deadline, description) {}

ExamTask::ExamTask(const std::string &title, const DateTime &deadline,
                   const std::string &description)
    : Task(title, deadline, description) {}

std::shared_ptr<Task> LabFactory::createTask(const std::string &title,
                                             const DateTime &deadline,
                                             const std::string &description) {
  return std::make_shared<LabTask>(title, deadline, description);
}

std::shared_ptr<Task>
ProjectFactory::createTask(const std::string &title, const DateTime &deadline,
                           const std::string &description) {
  return std::make_shared<ProjectTask>(title, deadline, description);
}

std::shared_ptr<Task> ExamFactory::createTask(const std::string &title,
                                              const DateTime &deadline,
                                              const std::string &description) {
  return std::make_shared<ExamTask>(title, deadline, description);
}
