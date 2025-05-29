#include "../include/TaskState.h"
#include "../include/Task.h"
#include <iostream>

void TaskState::start(Task &task) {}
void TaskState::complete(Task &task) {}
void TaskState::reopen(Task &task) {}
bool TaskState::isFinished() const { return false; }

void PendingState::start(Task &task) {
  task.setState(std::make_shared<InProgressState>());
}
void PendingState::complete(Task &task) {
  task.setState(std::make_shared<CompletedState>());
}

void InProgressState::complete(Task &task) {
  task.setState(std::make_shared<CompletedState>());
}
void InProgressState::reopen(Task &task) {
  task.setState(std::make_shared<PendingState>());
  task.setMarks(0);
}

void CompletedState::reopen(Task &task) {
  task.setMarks(0);
  task.setState(std::make_shared<InProgressState>());
}

bool CompletedState::isFinished() const { return true; }
