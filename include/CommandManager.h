#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include "Command.h"
#include <memory>
#include <vector>

class CommandManager {
private:
  std::vector<std::shared_ptr<Command>> history_;

  CommandManager() = default;
  ~CommandManager() = default;

public:
  static CommandManager &instance();

  CommandManager(const CommandManager &) = delete;
  CommandManager &operator=(const CommandManager &) = delete;
  CommandManager(CommandManager &&) = delete;
  CommandManager &operator=(CommandManager &&) = delete;

  void executeCommand(std::shared_ptr<Command> command);
  void undoLastCommand();
  bool canUndo() const;

  void clearHistory();
};

#endif // COMMAND_MANAGER_H
