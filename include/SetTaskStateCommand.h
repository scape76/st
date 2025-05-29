// include/TaskCommands.h
#ifndef TASK_COMMANDS_H
#define TASK_COMMANDS_H

#include "Command.h"
#include "Task.h"
#include "TaskState.h"
#include <memory>

class SetTaskStateCommand : public Command {
private:
  Task &task_;
  std::shared_ptr<TaskState> newState_;
  std::shared_ptr<TaskState> previousState_;

public:
  SetTaskStateCommand(Task &task, std::shared_ptr<TaskState> targetState);

  void execute() override;
  void undo() override;
};

#endif // TASK_COMMANDS_H
