#include "../include/TaskCommands.h"
#include "../include/Task.h"
#include "../include/TaskState.h"
#include <iostream>

SetTaskStateCommand::SetTaskStateCommand(Task &task,
                                         std::shared_ptr<TaskState> targetState)
    : task_(task), newState_(std::move(targetState)), previousState_(nullptr) {}

void SetTaskStateCommand::execute() {
  previousState_ = task_.getState(); // Store the current state of the task

  // Trigger the appropriate semantic action on the task based on the target
  // state type This assumes newState_ is an instance representing the desired
  // state, e.g., std::make_shared<InProgressState>()
  if (dynamic_cast<PendingState *>(newState_.get())) {
    // This case might be used by a "reset" or "revert to pending" action.
    // If task is already Pending, its reopen/start might do nothing.
    // If task is Completed/InProgress, its reopenTask should handle transition
    // to Pending/InProgress
    task_
        .reopenTask(); // A general "reopen" could lead to Pending or InProgress
                       // depending on the current state's reopen() logic.
                       // If specifically targeting Pending, ensure reopen leads
                       // there or use task.setState() directly. For simplicity,
                       // we assume reopen might put it back to Pending if
                       // appropriate. Or, if truly just setting state:
                       // task_.setState(std::make_shared<PendingState>());
    if (task_.getStateName() !=
        "Pending") { // If reopen didn't make it pending, force it.
      task_.setState(std::make_shared<PendingState>());
    }

  } else if (dynamic_cast<InProgressState *>(newState_.get())) {
    task_.startTask(); // This method on Task should transition its state to
                       // InProgressState
  } else if (dynamic_cast<CompletedState *>(newState_.get())) {
    task_.completeTask(); // This method on Task should transition its state to
                          // CompletedState
  } else {
    // Fallback for any other state types you might add, or if newState_ is just
    // a generic state. This is less likely if you always pass concrete target
    // states.
    task_.setState(newState_);
    std::cout << "Warning: SetTaskStateCommand executed with an unhandled "
                 "target state type, direct setState applied."
              << std::endl;
  }

  std::cout << "Command Executed: Task '" << task_.getTitle()
            << "' state changed from '"
            << (previousState_ ? previousState_->getName() : "null") << "' to '"
            << task_.getStateName() << "'." << std::endl;
}

void SetTaskStateCommand::undo() {
  if (previousState_) {
    task_.setState(previousState_); // Restore the previous state object
    std::cout << "Command Undone: Task '" << task_.getTitle()
              << "' state reverted to '" << task_.getStateName() << "'."
              << std::endl;
  } else {
    std::cout
        << "Cannot undo SetTaskStateCommand: No previous state was stored."
        << std::endl;
  }
}
