#include "../include/Notification.h"
#include "../include/Subject.h" // Add full Subject definition
#include "../include/Task.h"
#include "../include/TaskBuilder.h"
#include "../include/TaskState.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

// Test fixture for integration tests of creational patterns
class CreationalPatternsIntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Get the notification manager singleton
    notificationManager = &NotificationManager::getInstance();

    // Clear any existing notifications
    auto notifications = notificationManager->getNotifications();
    for (size_t i = 0; i < notifications.size(); i++) {
      notificationManager->removeNotification(0);
    }

    // Create a deadline in the future
    testDeadline = std::chrono::system_clock::now() + std::chrono::hours(72);
  }

  void TearDown() override {
    // Clear notifications after the test
    auto notifications = notificationManager->getNotifications();
    for (size_t i = 0; i < notifications.size(); i++) {
      notificationManager->removeNotification(0);
    }
  }

  NotificationManager *notificationManager;
  DateTime testDeadline;
};

// Test integration of all creational patterns
TEST_F(CreationalPatternsIntegrationTest, AllCreationalPatternsIntegration) {
  // 1. Create a subject to add tasks to
  auto subject = std::make_shared<Subject>("Programming Principles", "CS101",
                                           "Introduction to Programming");

  // 2. Create a lab task using the TaskBuilder and LabFactory
  auto labTask = TaskBuilder()
                     .setTitle("Algorithm Analysis Lab")
                     .setDescription("Implement and analyze sorting algorithms")
                     .setDeadline(testDeadline)
                     .asLab()
                     .build();

  // Verify the lab task was correctly created
  EXPECT_EQ("Algorithm Analysis Lab", labTask->getTitle());
  EXPECT_EQ("Lab", labTask->getType());

  // 3. Create a project task using the TaskBuilder and ProjectFactory
  auto projectTask = TaskBuilder()
                         .setTitle("Semester Project")
                         .setDescription("Build a student management system")
                         .setDeadline(testDeadline)
                         .asProject()
                         .build();

  // Verify the project task was correctly created
  EXPECT_EQ("Semester Project", projectTask->getTitle());
  EXPECT_EQ("Project", projectTask->getType());

  // 4. Add tasks to subject
  subject->addTask(labTask);
  subject->addTask(projectTask);

  // Verify tasks were added
  auto subjectTasks = subject->getTasks();
  EXPECT_EQ(2, subjectTasks.size());

  // 5. Create and add a notification using the singleton NotificationManager
  auto notification = std::make_shared<DeadlineNotification>(
      "Don't forget your Algorithm Analysis lab!", labTask,
      2); // 2 days before deadline

  notificationManager->addNotification(notification);

  // Verify notification was added
  auto notifications = notificationManager->getNotifications();
  EXPECT_EQ(1, notifications.size());
  EXPECT_EQ("Don't forget your Algorithm Analysis lab!",
            notifications[0]->getMessage());

  // 5. Verify task state is managed by the NotStartedState singleton
  EXPECT_EQ("Not Started", labTask->getState()->getName());
  EXPECT_EQ("Not Started", projectTask->getState()->getName());

  // Should be the same singleton instance
  EXPECT_EQ(labTask->getState(), projectTask->getState());
}

// Test integration of Builder, Factory, and Singleton
TEST_F(CreationalPatternsIntegrationTest, BuilderFactorySingletonIntegration) {
  // 1. Use builder with factory to create a task
  auto lab = TaskBuilder()
                 .setTitle("Algorithm Lab")
                 .setDescription("Implement sorting algorithms")
                 .setDeadline(testDeadline)
                 .asLab()
                 .build();

  // 2. Verify task properties
  EXPECT_EQ("Algorithm Lab", lab->getTitle());
  EXPECT_EQ("Lab", lab->getType());

  // 3. Task should be initialized with NotStartedState singleton
  EXPECT_EQ("Not Started", lab->getState()->getName());

  // 4. Use notification manager singleton with the created task
  auto notification = std::make_shared<DeadlineNotification>(
      "Lab deadline approaching", lab, 1); // 1 day before deadline

  notificationManager->addNotification(notification);

  // 5. Verify the notification was added
  auto notifications = notificationManager->getNotifications();
  EXPECT_EQ(1, notifications.size());
  EXPECT_EQ("Lab deadline approaching", notifications[0]->getMessage());

  // 6. Test state changes
  lab->startTask();
  EXPECT_EQ("In Progress", lab->getState()->getName());

  // 7. Create another task with builder and factory
  auto exam = TaskBuilder()
                  .setTitle("Final Exam")
                  .setDeadline(testDeadline)
                  .asExam()
                  .build();

  // 8. Verify it also starts with NotStartedState singleton
  EXPECT_EQ("Not Started", exam->getState()->getName());

  // 9. Verify both tasks use different instances but same state object when
  // reset
  EXPECT_NE(lab.get(), exam.get()); // Different tasks

  // Reset lab task to Not Started state
  lab->setState(NotStartedState::getInstance());
  EXPECT_EQ("Not Started", lab->getState()->getName());
  EXPECT_EQ(lab->getState(), exam->getState()); // Same state instance
}
