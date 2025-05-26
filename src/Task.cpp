#include "../include/Task.h"
#include "../include/Notification.h"
#include "../include/Subject.h"
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

  std::time_t now = std::time(nullptr);
  std::tm tm_now = {};

#ifdef _WIN32
  gmtime_s(&tm_now, &now);
#else
  tm_now = *std::gmtime(&now);
#endif

  std::time_t gmt = std::mktime(&tm_now);
  std::time_t local = std::time(nullptr);
  double diff = std::difftime(local, gmt);

  std::stringstream ss;
  ss << std::put_time(&tm_local, "%Y-%m-%d %H:%M");
  return ss.str();
}

Task::Task(const std::string &title, const DateTime &deadline,
           const std::string &description)
    : title(title), description(description), deadline(deadline),
      completed(false), marks(0), progress(0.0f) {
  state = NotStartedState::getInstance();
  state->setContext(this);
}

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

void Task::setCompleted(bool completed) {
  this->completed = completed;

  if (completed) {
    state->complete(marks);
  } else if (state->getName() == "Completed") {
    setState(NotStartedState::getInstance());
  }
}

void Task::setMarks(int marks) { this->marks = marks; }

void Task::setProgress(float progress) { this->progress = progress; }

void Task::setState(std::shared_ptr<TaskState> newState) {
  this->state = newState;
  this->state->setContext(this);
}

std::shared_ptr<TaskState> Task::getState() const { return state; }

void Task::startTask() { state->start(); }

void Task::updateProgress(float percentage) { state->makeProgress(percentage); }

void Task::checkDeadline() { state->checkDeadline(); }

std::string Task::getStateName() const { return state->getName(); }

std::string Task::getStateDescription() const {
  return state->getDescription();
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

  std::cout << "Status: " << getStateName() << std::endl;

  if (state->getName() == "In Progress") {
    std::cout << "Progress: " << progress << "%" << std::endl;
  } else if (state->getName() == "Completed") {
    std::cout << "Marks: " << marks << std::endl;
  }

  std::cout << "Notifications: " << notifications.size() << std::endl;
}

std::shared_ptr<TaskMemento> Task::createMemento() const {
  std::string subjectCode = "";
  if (auto subjectPtr = getSubject()) {
    subjectCode = subjectPtr->getCode();
  }

  return std::make_shared<TaskMemento>(title, description, deadline, completed,
                                       marks, progress, state->getName(),
                                       subjectCode);
}

void Task::restoreFromMemento(const std::shared_ptr<TaskMemento> &memento) {
  title = memento->getTitle();
  description = memento->getDescription();
  deadline = memento->getDeadline();
  completed = memento->isCompleted();
  marks = memento->getMarks();
  progress = memento->getProgress();

  std::string stateName = memento->getState();
  if (stateName == "Not Started") {
    setState(NotStartedState::getInstance());
  } else if (stateName == "In Progress") {
    setState(std::make_shared<InProgressState>(progress));
  } else if (stateName == "Completed") {
    setState(std::make_shared<CompletedState>(marks));
  } else if (stateName == "Overdue") {
    setState(std::make_shared<OverdueState>(deadline));
  }
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
