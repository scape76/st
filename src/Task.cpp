#include "../include/Task.h"
#include "../include/Notification.h"
#include "../include/Subject.h"
#include "../include/TaskState.h"
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
    : title_(title), description_(description), deadline_(deadline), marks_(0),
      subject_(nullptr) {
  setState(std::make_shared<PendingState>());
}

void Task::setState(std::shared_ptr<TaskState> newState) { state_ = newState; }

std::shared_ptr<TaskState> Task::getState() const { return state_; }

std::string Task::getTitle() const { return title_; }
std::string Task::getDescription() const { return description_; }
DateTime Task::getDeadline() const { return deadline_; }
std::shared_ptr<Subject> Task::getSubject() const { return subject_; }

std::string Task::getStateName() const {
  if (state_) {
    return state_->getName();
  }
  return "None";
}

std::vector<std::shared_ptr<Notification>> Task::getNotifications() const {
  return notifications_;
}

int Task::getMarks() const { return marks_; }

bool Task::isCompleted() const { return state_ ? state_->isFinished() : false; }

float Task::getProgress() const {
  return state_ ? state_->getConceptualProgress() : 0.0f;
}

void Task::setTitle(const std::string &title) { this->title_ = title; }

void Task::setDescription(const std::string &description) {
  this->description_ = description;
}
void Task::setDeadline(const DateTime &deadline) { this->deadline_ = deadline; }

void Task::setSubject(std::shared_ptr<Subject> subject) {
  this->subject_ = subject;
}

void Task::setMarks(int marks) {
  if (isCompleted()) {
    this->marks_ = marks;
  } else {
    this->marks_ = 0;
  }
}

void Task::startTask() {
  if (state_)
    state_->start(*this);
}
void Task::completeTask() {
  if (state_)
    state_->complete(*this);
}
void Task::reopenTask() {
  if (state_)
    state_->reopen(*this);
}

void Task::addNotification(std::shared_ptr<Notification> notification) {
  notifications_.push_back(notification);
}

void Task::removeNotification(size_t index) {
  if (index < notifications_.size()) {
    notifications_.erase(notifications_.begin() + index);
  } else {
    std::cout << "Invalid notification index for removal." << std::endl;
  }
}

void Task::displayInfo() const {
  std::cout << "Task: " << getTitle() << " (" << getType() << ")" << std::endl;

  if (!getDescription().empty()) {
    std::cout << "Description: " << getDescription() << std::endl;
  }

  std::cout << "Deadline: " << formatDateTime(getDeadline()) << std::endl;

  if (getSubject()) {
    std::cout << "Subject: " << getSubject()->getName() << " ("
              << getSubject()->getCode() << ")" << std::endl;
  } else {
    std::cout << "Not assigned to any subject" << std::endl;
  }

  // Use getStateName() for status
  std::cout << "Status: " << getStateName() << std::endl;

  // Display conceptual progress from the state
  // You can choose not to display 50% for "In Progress" if it's not meaningful
  // and just rely on the state name.
  if (getStateName() == "In Progress" ||
      isCompleted()) { // Show progress if In Progress or Completed
    std::cout << "Progress: " << std::fixed << std::setprecision(1)
              << getProgress() << "%" << std::endl;
  }

  if (isCompleted()) { // Use isCompleted() which checks the state
    std::cout << "Marks: " << getMarks() << std::endl;
  }

  std::cout << "Notifications: " << getNotifications().size() << std::endl;
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
