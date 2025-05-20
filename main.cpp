#include "Dtos.h"
#include "Exceptions.h"
#include "IdGenerator.h" // To show Singleton usage directly (though it's mostly internal)
#include "NotificationFactory.h" // For SimpleNotificationFactory
#include "SystemFacade.h"
#include <iostream>
#include <vector>

void demonstrateCreationalPatterns() {
  // Singleton: IdGenerator is used internally by AssignmentBuilder and
  // NotificationFactory. We can get its instance to show it's a singleton.
  IdGenerator &idGen1 = IdGenerator::getInstance();
  IdGenerator &idGen2 = IdGenerator::getInstance();
  std::cout << "Singleton Demo: IdGenerator instance 1 address: " << &idGen1
            << std::endl;
  std::cout << "Singleton Demo: IdGenerator instance 2 address: " << &idGen2
            << std::endl;
  std::cout << "Next Assignment ID from Singleton: "
            << idGen1.generateAssignmentId() << std::endl; // Consumes an ID
  std::cout << "Next Notification ID from Singleton: "
            << idGen1.generateNotificationId() << std::endl; // Consumes an ID
  // Resetting for the facade by re-getting instance and consuming a few more if
  // needed (not ideal, just for demo) In a real app, IDs consumed are just
  // consumed.
  IdGenerator::getInstance().generateAssignmentId();
  IdGenerator::getInstance().generateAssignmentId();
  IdGenerator::getInstance().generateNotificationId();
  IdGenerator::getInstance().generateNotificationId();

  std::cout << "\n--- System Operations ---\n" << std::endl;

  // Factory Method: SimpleNotificationFactory creating Notification objects
  auto simpleFactory = std::make_unique<SimpleNotificationFactory>();
  SystemFacade facade(std::move(
      simpleFactory)); // Facade uses the factory via NotificationService

  // Setup
  try {
    facade.addSubject(101, "Math");
    facade.addSubject(102, "History");
  } catch (const std::exception &e) {
    std::cerr << "Error during setup: " << e.what() << std::endl;
  }

  std::cout << std::endl;

  // Create Assignment (uses Builder internally)
  int mathAssignmentId = 0;
  try {
    AssignmentDto mathAssignmentDto;
    mathAssignmentDto.name = "Algebra Homework 1";
    mathAssignmentDto.deadline = "2025-05-25 23:59";
    mathAssignmentDto.notificationSettings.push_back(
        {"Reminder: Algebra due soon!", "1 day before deadline"});
    mathAssignmentDto.notificationSettings.push_back(
        {"Final Reminder: Algebra due NOW!", "on deadline"});
    mathAssignmentId = facade.createAssignment(101, mathAssignmentDto);

    AssignmentDto historyAssignmentDto;
    historyAssignmentDto.name = "Essay on Ancient Rome";
    historyAssignmentDto.deadline = "2025-06-10 23:59";
    // No notifications for this one initially
    facade.createAssignment(102, historyAssignmentDto);

  } catch (const std::exception &e) {
    std::cerr << "Error creating assignment: " << e.what() << std::endl;
  }

  facade.displayAllAssignments();
  facade.displayAllNotifications();

  // Send a notification explicitly (find a notification ID to send)
  // Assuming the first notification scheduled for mathAssignmentId is ID 3 (if
  // IdGen started at 1 for notifs and 2 were consumed above) This is fragile; a
  // better way would be to get notification IDs from the facade or dataStore.
  // For demo, let's find a notification for mathAssignmentId
  int notificationToSendId = -1;
  // Temporary access to DataStore for demo - not good practice for main.
  // Instead, SystemFacade should provide a method to get notification IDs for
  // an assignment. For now, we'll just try ID 3 (as per consumption above).
  if (mathAssignmentId > 0) {
    // This is a simplified way to get a notification ID.
    // In a real app, you'd query for notification IDs related to an assignment.
    // Assuming notification ID 3 was created for math assignment.
    notificationToSendId = 3; // Based on IdGenerator consumption.
  }

  if (notificationToSendId != -1) {
    std::cout << "\nAttempting to send specific notification ID: "
              << notificationToSendId << std::endl;
    try {
      facade.sendNotification(notificationToSendId);
    } catch (const std::exception &e) {
      std::cerr << "Error sending specific notification: " << e.what()
                << std::endl;
    }
  }

  facade.displayAllNotifications();

  // Simulate time passing and send "due" notifications
  facade.sendAllDueNotifications();

  facade.displayAllNotifications();

  // Mark assignment as completed
  if (mathAssignmentId > 0) {
    try {
      facade.markAssignmentAsCompleted(mathAssignmentId, 95); // With a grade
    } catch (const std::exception &e) {
      std::cerr << "Error marking assignment as completed: " << e.what()
                << std::endl;
    }
  }

  facade.displayAllAssignments();
  facade.displayAllNotifications(); // Should show math notifications as
                                    // cancelled/removed

  // Try to send notifications again (math ones should be gone or skipped)
  facade.sendAllDueNotifications();
  facade.displayAllNotifications();

  // Example of an error:
  try {
    std::cout << "\nTrying to mark non-existent assignment:" << std::endl;
    facade.markAssignmentAsCompleted(999, 100);
  } catch (const std::exception &e) {
    std::cerr << "Caught expected error: " << e.what() << std::endl;
  }
  try {
    std::cout << "\nTrying to create assignment for non-existent subject:"
              << std::endl;
    AssignmentDto invalidDto;
    invalidDto.name = "Phantom Task";
    invalidDto.deadline = "2025-12-31 23:59";
    facade.createAssignment(999, invalidDto);
  } catch (const std::exception &e) {
    std::cerr << "Caught expected error: " << e.what() << std::endl;
  }
}

int main() {
  demonstrateCreationalPatterns();
  return 0;
}
