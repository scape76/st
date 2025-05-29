#include "../include/CommandManager.h"
#include <iostream>

CommandManager &CommandManager::instance() {
  static CommandManager singletonInstance;
  return singletonInstance;
}

void CommandManager::executeCommand(std::shared_ptr<Command> command) {
  if (command) {
    command->execute();
    history_.push_back(command);
  }
}

void CommandManager::undoLastCommand() {
  if (!history_.empty()) {
    std::shared_ptr<Command> lastCommand = history_.back();
    history_.pop_back();
    lastCommand->undo();
  } else {
    std::cout << "CommandManager: No commands in history to undo." << std::endl;
  }
}

bool CommandManager::canUndo() const { return !history_.empty(); }

void CommandManager::clearHistory() { history_.clear(); }
