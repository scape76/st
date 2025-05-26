// #include "../include/Task.h"
// #include <gtest/gtest.h>
// #include <memory>

// // Test fixture for TaskFactory tests
// class TaskFactoryTest : public ::testing::Test {
// protected:
//   void SetUp() override {
//     labFactory = std::make_unique<LabFactory>();
//     projectFactory = std::make_unique<ProjectFactory>();
//     examFactory = std::make_unique<ExamFactory>();

//     // Common test data
//     testTitle = "Test Task";
//     testDescription = "This is a test description";
//     testDeadline = std::chrono::system_clock::now() + std::chrono::hours(48);
//   }

//   std::unique_ptr<TaskFactory> labFactory;
//   std::unique_ptr<TaskFactory> projectFactory;
//   std::unique_ptr<TaskFactory> examFactory;

//   std::string testTitle;
//   std::string testDescription;
//   DateTime testDeadline;
// };

// // Test LabFactory creates the right type
// TEST_F(TaskFactoryTest, LabFactoryCreatesLabTask) {
//   auto task = labFactory->createTask(testTitle, testDeadline,
//   testDescription);

//   // Verify the task is created with correct properties
//   EXPECT_EQ(testTitle, task->getTitle());
//   EXPECT_EQ(testDescription, task->getDescription());
//   EXPECT_EQ(testDeadline, task->getDeadline());

//   // Verify it's the right type
//   EXPECT_EQ("Lab", task->getType());

//   // Verify it can be cast to LabTask
//   auto labTask = std::dynamic_pointer_cast<LabTask>(task);
//   EXPECT_NE(nullptr, labTask);
// }

// // Test ProjectFactory creates the right type
// TEST_F(TaskFactoryTest, ProjectFactoryCreatesProjectTask) {
//   auto task =
//       projectFactory->createTask(testTitle, testDeadline, testDescription);

//   // Verify the task is created with correct properties
//   EXPECT_EQ(testTitle, task->getTitle());
//   EXPECT_EQ(testDescription, task->getDescription());
//   EXPECT_EQ(testDeadline, task->getDeadline());

//   // Verify it's the right type
//   EXPECT_EQ("Project", task->getType());

//   // Verify it can be cast to ProjectTask
//   auto projectTask = std::dynamic_pointer_cast<ProjectTask>(task);
//   EXPECT_NE(nullptr, projectTask);
// }

// TEST_F(TaskFactoryTest, ExamFactoryCreatesExamTask) {
//   auto task = examFactory->createTask(testTitle, testDeadline,
//   testDescription);

//   // Verify the task is created with correct properties
//   EXPECT_EQ(testTitle, task->getTitle());
//   EXPECT_EQ(testDescription, task->getDescription());
//   EXPECT_EQ(testDeadline, task->getDeadline());

//   EXPECT_EQ("Exam", task->getType());

//   auto examTask = std::dynamic_pointer_cast<ExamTask>(task);
//   EXPECT_NE(nullptr, examTask);
// }

// // Test creating tasks with default description
// TEST_F(TaskFactoryTest, CreateTaskWithDefaultDescription) {
//   auto task = labFactory->createTask(testTitle, testDeadline);

//   EXPECT_EQ(testTitle, task->getTitle());
//   EXPECT_EQ("", task->getDescription()); // Default description should be
//   empty EXPECT_EQ(testDeadline, task->getDeadline()); EXPECT_EQ("Lab",
//   task->getType());

//   // Verify with other factories too
//   auto projectTask = projectFactory->createTask("Project Test",
//   testDeadline); EXPECT_EQ("", projectTask->getDescription());

//   auto examTask = examFactory->createTask("Exam Test", testDeadline);
//   EXPECT_EQ("", examTask->getDescription());
// }

// TEST_F(TaskFactoryTest, PolymorphicBehavior) {
//   auto lab = labFactory->createTask("Lab", testDeadline);
//   auto project = projectFactory->createTask("Project", testDeadline);
//   auto exam = examFactory->createTask("Exam", testDeadline);

//   std::vector<std::shared_ptr<Task>> tasks;
//   tasks.push_back(lab);
//   tasks.push_back(project);
//   tasks.push_back(exam);

//   EXPECT_EQ("Lab", tasks[0]->getType());
//   EXPECT_EQ("Project", tasks[1]->getType());
//   EXPECT_EQ("Exam", tasks[2]->getType());
// }
