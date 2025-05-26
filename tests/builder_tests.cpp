// #include "../include/Subject.h"
// #include "../include/TaskBuilder.h"
// #include <gtest/gtest.h>
// #include <stdexcept>

// // Test fixture for SubjectBuilder tests
// class SubjectBuilderTest : public ::testing::Test {
// protected:
//   void SetUp() override {}

//   SubjectBuilder builder;
// };

// // Test SubjectBuilder constructs a subject with all properties
// TEST_F(SubjectBuilderTest, BuildsCompleteSubjectSucessfully) {
//   auto subject = builder.setName("Test Subject")
//                      .setCode("TEST101")
//                      .setDescription("This is a test subject")
//                      .build();

//   EXPECT_EQ("Test Subject", subject->getName());
//   EXPECT_EQ("TEST101", subject->getCode());
//   EXPECT_EQ("This is a test subject", subject->getDescription());
// }

// // Test SubjectBuilder validates required fields
// TEST_F(SubjectBuilderTest, RequiredFieldValidation) {
//   // Missing name
//   EXPECT_THROW(builder.setCode("TEST101").build(), std::invalid_argument);

//   // Reset builder
//   builder = SubjectBuilder();

//   // Missing code
//   EXPECT_THROW(builder.setName("Test Subject").build(),
//   std::invalid_argument);
// }

// // Test SubjectBuilder with optional description
// TEST_F(SubjectBuilderTest, OptionalDescription) {
//   auto subject = builder.setName("Test Subject").setCode("TEST101").build();

//   EXPECT_EQ("Test Subject", subject->getName());
//   EXPECT_EQ("TEST101", subject->getCode());
//   EXPECT_EQ("", subject->getDescription()); // Default empty description
// }

// // Test fixture for TaskBuilder tests
// class TaskBuilderTest : public ::testing::Test {
// protected:
//   void SetUp() override {
//     testDeadline = std::chrono::system_clock::now() + std::chrono::hours(72);
//   }

//   TaskBuilder builder;
//   DateTime testDeadline;
// };

// // Test TaskBuilder constructs a lab task
// TEST_F(TaskBuilderTest, BuildsLabTask) {
//   auto task = builder.setTitle("Test Lab")
//                   .setDescription("This is a test lab")
//                   .setDeadline(testDeadline)
//                   .asLab()
//                   .build();

//   EXPECT_EQ("Test Lab", task->getTitle());
//   EXPECT_EQ("This is a test lab", task->getDescription());
//   EXPECT_EQ(testDeadline, task->getDeadline());
//   EXPECT_EQ("Lab", task->getType());

//   // Verify it's the right type
//   // Verify it's the right type
//   EXPECT_EQ("Lab", task->getType());
// }

// TEST_F(TaskBuilderTest, BuildsProjectTask) {
//   auto task = builder.setTitle("Test Project")
//                   .setDescription("This is a test project")
//                   .setDeadline(testDeadline)
//                   .asProject()
//                   .build();

//   EXPECT_EQ("Test Project", task->getTitle());
//   EXPECT_EQ("This is a test project", task->getDescription());
//   EXPECT_EQ(testDeadline, task->getDeadline());
//   EXPECT_EQ("Project", task->getType());
// }

// TEST_F(TaskBuilderTest, BuildsLabTask2) {
//   auto task = builder.setTitle("Test Lab")
//                   .setDescription("This is a test lab")
//                   .setDeadline(testDeadline)
//                   .asLab()
//                   .build();

//   EXPECT_EQ("Test Lab", task->getTitle());
//   EXPECT_EQ("This is a test lab", task->getDescription());
//   EXPECT_EQ(testDeadline, task->getDeadline());
//   EXPECT_EQ("Lab", task->getType());
// }

// // Test TaskBuilder validates required fields
// TEST_F(TaskBuilderTest, RequiredFieldValidation) {
//   EXPECT_THROW(builder.setDeadline(testDeadline).asLab().build(),
//                std::invalid_argument);

//   builder = TaskBuilder();

//   EXPECT_THROW(builder.setTitle("Test
//   Task").setDeadline(testDeadline).build(),
//                std::invalid_argument);
// }

// // Test TaskBuilder reset functionality
// TEST_F(TaskBuilderTest, ResetBuilder) {
//   // Build first task
//   auto task1 = builder.setTitle("Test Lab")
//                    .setDescription("This is a test lab")
//                    .setDeadline(testDeadline)
//                    .asLab()
//                    .build();

//   // Reset the builder
//   builder.reset();

//   // Try to build without required fields - should throw
//   EXPECT_THROW(builder.build(), std::invalid_argument);

//   // Build a second, different task
//   auto task2 =
//       builder.setTitle("Test
//       Exam").setDeadline(testDeadline).asExam().build();

//   // Verify they're different
//   EXPECT_EQ("Test Lab", task1->getTitle());
//   EXPECT_EQ("Lab", task1->getType());

//   EXPECT_EQ("Test Exam", task2->getTitle());
//   EXPECT_EQ("Exam", task2->getType());
// }

// // Test chaining multiple operations fluently
// TEST_F(TaskBuilderTest, ChainOperationsFluently) {
//   auto task = builder.setTitle("Chained Task")
//                   .setDescription("Created with chained operations")
//                   .setDescription("Description was rewritten")
//                   .setDescription("Description was rewritten again")
//                   .setDeadline(testDeadline)
//                   .asProject()
//                   .build();

//   EXPECT_EQ("Chained Task", task->getTitle());
//   EXPECT_EQ("Description was rewritten again", task->getDescription());
//   EXPECT_EQ("Project", task->getType());
// }

// // Test handling invalid deadline
// TEST_F(TaskBuilderTest, HandlesInvalidDeadline) {
//   // Set deadline in the past
//   auto pastDeadline = std::chrono::system_clock::now() -
//   std::chrono::hours(24);

//   // This should either throw an exception or handle it gracefully depending
//   on
//   // your design If your builder should reject past deadlines: EXPECT_THROW(
//   //    builder->setTitle("Past
//   //    Deadline")->setDeadline(pastDeadline)->asLab()->build(),
//   //    std::invalid_argument
//   // );

//   // If your builder accepts past deadlines (you might want to validate
//   // elsewhere):
//   auto task = builder.setTitle("Past Deadline")
//                   .setDeadline(pastDeadline)
//                   .asLab()
//                   .build();
//   EXPECT_EQ(pastDeadline, task->getDeadline());
// }
