// #include "../include/Notification.h" // For NotificationManager
// #include "../include/Task.h"
// #include <gtest/gtest.h>
// #include <thread>
// #include <vector>

// // Test fixture for NotificationManager tests
// class NotificationManagerTest : public ::testing::Test {
// protected:
//   void SetUp() override {
//     // Get the instance
//     manager = &NotificationManager::getInstance();

//     // Clear any existing notifications
//     auto notifications = manager->getNotifications();
//     for (size_t i = 0; i < notifications.size(); i++) {
//       manager->removeNotification(0); // Always remove the first one
//     }
//   }

//   void TearDown() override {
//     // Clear notifications again after the test
//     auto notifications = manager->getNotifications();
//     for (size_t i = 0; i < notifications.size(); i++) {
//       manager->removeNotification(0);
//     }
//   }

//   NotificationManager *manager;
// };

// // Test that getInstance always returns the same instance
// TEST_F(NotificationManagerTest, GetInstanceReturnsSameInstance) {
//   NotificationManager &instance1 = NotificationManager::getInstance();
//   NotificationManager &instance2 = NotificationManager::getInstance();

//   // Pointers should be identical
//   EXPECT_EQ(&instance1, &instance2);
// }

// // Test concurrent access to the singleton
// TEST_F(NotificationManagerTest, ConcurrentAccess) {
//   std::vector<NotificationManager *> instances(10);
//   std::vector<std::thread> threads;

//   // Create 10 threads, each getting the singleton instance
//   for (int i = 0; i < 10; i++) {
//     threads.emplace_back([&instances, i]() {
//       instances[i] = &NotificationManager::getInstance();
//     });
//   }

//   // Join all threads
//   for (auto &thread : threads) {
//     thread.join();
//   }

//   // All instances should be the same
//   for (size_t i = 1; i < instances.size(); i++) {
//     EXPECT_EQ(instances[0], instances[i]);
//   }
// }

// // Test adding and retrieving notifications
// TEST_F(NotificationManagerTest, AddAndRetrieveNotifications) {
//   // Create a dummy task and notification
//   auto task = std::make_shared<LabTask>(
//       "Test Task", std::chrono::system_clock::now() + std::chrono::hours(24),
//       "Test Description");

//   auto notification = std::make_shared<Notification>(
//       "Test notification", std::chrono::system_clock::now(), task);

//   // Add notification to manager
//   manager->addNotification(notification);

//   // Verify it was added correctly
//   auto notifications = manager->getNotifications();
//   EXPECT_EQ(1, notifications.size());
//   EXPECT_EQ("Test notification", notifications[0]->getMessage());

//   // Test retrieving by task
//   auto taskNotifications = manager->getNotificationsForTask(task);
//   EXPECT_EQ(1, taskNotifications.size());
//   EXPECT_EQ("Test notification", taskNotifications[0]->getMessage());
// }

// // Test removing notifications
// TEST_F(NotificationManagerTest, RemoveNotifications) {
//   // Create a dummy task and notifications
//   auto task = std::make_shared<LabTask>(
//       "Test Task", std::chrono::system_clock::now() +
//       std::chrono::hours(24));

//   auto notification1 = std::make_shared<Notification>(
//       "Notification 1", std::chrono::system_clock::now(), task);
//   auto notification2 = std::make_shared<Notification>(
//       "Notification 2", std::chrono::system_clock::now(), task);

//   // Add both notifications
//   manager->addNotification(notification1);
//   manager->addNotification(notification2);

//   // Check we have two notifications
//   EXPECT_EQ(2, manager->getNotifications().size());

//   // Remove the first notification
//   manager->removeNotification(0);

//   // Verify we have one left and it's the second one
//   auto notifications = manager->getNotifications();
//   EXPECT_EQ(1, notifications.size());
//   EXPECT_EQ("Notification 2", notifications[0]->getMessage());
// }

// // Test fixture for NotStartedState singleton tests
// class NotStartedStateTest : public ::testing::Test {
// protected:
//   void SetUp() override {
//     // Get instance of the singleton
//     state = NotStartedState::getInstance();
//   }

//   std::shared_ptr<TaskState> state;
// };

// // Test that getInstance always returns the same instance
// TEST_F(NotStartedStateTest, GetInstanceReturnsSameInstance) {
//   auto instance1 = NotStartedState::getInstance();
//   auto instance2 = NotStartedState::getInstance();

//   // Pointers should be identical
//   EXPECT_EQ(instance1.get(), instance2.get());
// }

// // Test concurrent access to the singleton
// TEST_F(NotStartedStateTest, ConcurrentAccess) {
//   std::vector<std::shared_ptr<TaskState>> instances(10);
//   std::vector<std::thread> threads;

//   // Create 10 threads, each getting the singleton instance
//   for (int i = 0; i < 10; i++) {
//     threads.emplace_back(
//         [&instances, i]() { instances[i] = NotStartedState::getInstance();
//         });
//   }

//   // Join all threads
//   for (auto &thread : threads) {
//     thread.join();
//   }

//   // All instances should be the same
//   for (size_t i = 1; i < instances.size(); i++) {
//     EXPECT_EQ(instances[0].get(), instances[i].get());
//   }
// }

// // Test NotStartedState behavior
// TEST_F(NotStartedStateTest, StateProperties) {
//   // Test state name and color
//   EXPECT_EQ("Not Started", state->getName());
//   EXPECT_EQ("Gray", state->getColor());
// }

// // Test state transition
// TEST_F(NotStartedStateTest, StateTransition) {
//   // Create a concrete task (LabTask) and set the state
//   auto task = std::make_shared<LabTask>(
//       "Test Task", std::chrono::system_clock::now() +
//       std::chrono::hours(24));

//   // Get initial state
//   auto initialState = task->getState();
//   EXPECT_EQ("Not Started", initialState->getName());

//   // Task should be in NotStartedState by default
//   EXPECT_EQ(0.0f, task->getProgress());

//   // Start the task and check state changes
//   task->startTask();
//   auto newState = task->getState();
//   EXPECT_NE(initialState->getName(), newState->getName());
// }
