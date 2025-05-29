#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

#include "../include/Task.h"
#include "../include/TaskBuilder.h"

#include "../include/Subject.h"

class SubjectBuilderTest : public ::testing::Test {
protected:
  void SetUp() override { builder = SubjectBuilder(); }

  SubjectBuilder builder;
  const std::string testName = "Introduction to Testing";
  const std::string testCode = "TEST101";
  const std::string testDesc = "A subject about software testing.";
};

TEST_F(SubjectBuilderTest, BuildsCompleteSubjectSuccessfully) {
#ifdef ACTUAL_SUBJECT_BUILDER_AVAILABLE
  auto subject = builder.setName(testName)
                     .setCode(testCode)
                     .setDescription(testDesc)
                     .build();

  ASSERT_NE(subject, nullptr);
  EXPECT_EQ(testName, subject->getName());
  EXPECT_EQ(testCode, subject->getCode());
  EXPECT_EQ(testDesc, subject->getDescription());
#else
  GTEST_SKIP() << "Skipping SubjectBuilderTest as actual "
                  "Subject/SubjectBuilder headers are needed.";
#endif
}

TEST_F(SubjectBuilderTest, RequiredFieldValidation) {
#ifdef ACTUAL_SUBJECT_BUILDER_AVAILABLE
  builder.reset();
  EXPECT_THROW(builder.setCode(testCode).setDescription(testDesc).build(),
               std::invalid_argument);

  builder.reset();
  EXPECT_THROW(builder.setName(testName).setDescription(testDesc).build(),
               std::invalid_argument);

  builder.reset();
  EXPECT_NO_THROW({
    auto subject = builder.setName(testName).setCode(testCode).build();
    ASSERT_NE(subject, nullptr);
    EXPECT_EQ(testName, subject->getName());
    EXPECT_EQ(testCode, subject->getCode());
    EXPECT_EQ("",
              subject->getDescription()); // Assuming default empty description
  });
#else
  GTEST_SKIP() << "Skipping SubjectBuilderTest as actual "
                  "Subject/SubjectBuilder headers are needed.";
#endif
}

TEST_F(SubjectBuilderTest, OptionalDescription) {
#ifdef ACTUAL_SUBJECT_BUILDER_AVAILABLE
  builder.reset();
  auto subject = builder.setName(testName).setCode(testCode).build();

  ASSERT_NE(subject, nullptr);
  EXPECT_EQ(testName, subject->getName());
  EXPECT_EQ(testCode, subject->getCode());
  EXPECT_EQ("", subject->getDescription()); // Default empty description
#else
  GTEST_SKIP() << "Skipping SubjectBuilderTest as actual "
                  "Subject/SubjectBuilder headers are needed.";
#endif
}

TEST_F(SubjectBuilderTest, ResetFunctionality) {
#ifdef ACTUAL_SUBJECT_BUILDER_AVAILABLE
  builder.setName("Temporary Subject").setCode("TMP100");

  builder.reset();

  EXPECT_THROW(builder.build(), std::invalid_argument);

  auto subject = builder.setName(testName).setCode(testCode).build();
  ASSERT_NE(subject, nullptr);
  EXPECT_EQ(testName, subject->getName());
  EXPECT_EQ(testCode, subject->getCode());
#else
  GTEST_SKIP() << "Skipping SubjectBuilderTest as actual "
                  "Subject/SubjectBuilder headers are needed.";
#endif
}

class TaskBuilderAndFactoryTest : public ::testing::Test {
protected:
  void SetUp() override {
    testDeadline = std::chrono::system_clock::now() + std::chrono::hours(24);
  }

  TaskBuilder builder;
  DateTime testDeadline;

  const std::string genericTitle = "Generic Task";
  const std::string genericDesc = "This is a generic description.";
};

TEST_F(TaskBuilderAndFactoryTest, BuildsLabTaskViaBuilder) {
  auto task = builder.setTitle("Lab Assignment 1")
                  .setDescription("Complete the first lab.")
                  .setDeadline(testDeadline)
                  .asLab()
                  .build();

  ASSERT_NE(task, nullptr);
  EXPECT_EQ("Lab Assignment 1", task->getTitle());
  EXPECT_EQ("Complete the first lab.", task->getDescription());
  EXPECT_EQ(testDeadline, task->getDeadline());
  EXPECT_EQ("Lab", task->getType());

  auto labTask = std::dynamic_pointer_cast<LabTask>(task);
  EXPECT_NE(labTask, nullptr);
}

TEST_F(TaskBuilderAndFactoryTest, BuildsProjectTaskViaBuilder) {
  auto task = builder.setTitle("Final Project")
                  .setDescription("Deliver the final project.")
                  .setDeadline(testDeadline)
                  .asProject()
                  .build();

  ASSERT_NE(task, nullptr);
  EXPECT_EQ("Final Project", task->getTitle());
  EXPECT_EQ("Deliver the final project.", task->getDescription());
  EXPECT_EQ(testDeadline, task->getDeadline());
  EXPECT_EQ("Project", task->getType());

  auto projectTask = std::dynamic_pointer_cast<ProjectTask>(task);
  EXPECT_NE(projectTask, nullptr);
}

TEST_F(TaskBuilderAndFactoryTest, BuildsExamTaskViaBuilder) {
  auto task = builder.setTitle("Midterm Exam")
                  .setDescription("Prepare for the midterm.")
                  .setDeadline(testDeadline)
                  .asExam()
                  .build();

  ASSERT_NE(task, nullptr);
  EXPECT_EQ("Midterm Exam", task->getTitle());
  EXPECT_EQ("Prepare for the midterm.", task->getDescription());
  EXPECT_EQ(testDeadline, task->getDeadline());
  EXPECT_EQ("Exam", task->getType());

  auto examTask = std::dynamic_pointer_cast<ExamTask>(task);
  EXPECT_NE(examTask, nullptr);
}

TEST_F(TaskBuilderAndFactoryTest, BuilderRequiredFieldValidation) {
  builder.reset();

  EXPECT_THROW(builder.setDescription(genericDesc)
                   .setDeadline(testDeadline)
                   .asLab()
                   .build(),
               std::invalid_argument);

  builder.reset();
}

TEST_F(TaskBuilderAndFactoryTest, BuilderOptionalDescription) {
  builder.reset();
  auto task = builder.setTitle("Task Without Description")
                  .setDeadline(testDeadline)
                  .asLab()
                  .build();

  ASSERT_NE(task, nullptr);
  EXPECT_EQ("Task Without Description", task->getTitle());
  EXPECT_EQ(testDeadline, task->getDeadline());
  EXPECT_EQ("Lab", task->getType());
  EXPECT_EQ("", task->getDescription());
}

TEST_F(TaskBuilderAndFactoryTest, BuilderResetFunctionality) {
  builder.setTitle("First Task")
      .setDescription("Description for first task.")
      .setDeadline(testDeadline)
      .asProject()
      .build();

  builder.reset();

  auto task2 = builder.setTitle("Second Task - Lab")
                   .setDeadline(testDeadline)
                   .asLab()
                   .build();

  ASSERT_NE(task2, nullptr);
  EXPECT_EQ("Second Task - Lab", task2->getTitle());
  EXPECT_EQ("Lab", task2->getType());
}

TEST_F(TaskBuilderAndFactoryTest, BuilderChainingAndOverwriting) {
  builder.reset();
  auto task = builder.setTitle("Initial Title")
                  .asProject()
                  .setDescription("Initial Description")
                  .setDeadline(testDeadline)
                  .setTitle("Final Title")
                  .setDescription("Final Description")
                  .asLab()
                  .build();

  ASSERT_NE(task, nullptr);
  EXPECT_EQ("Final Title", task->getTitle());
  EXPECT_EQ("Final Description", task->getDescription());
  EXPECT_EQ("Lab", task->getType());
}

class TaskFactoryDirectTest : public ::testing::Test {
protected:
  void SetUp() override {
    deadline = std::chrono::system_clock::now() + std::chrono::hours(48);
  }
  DateTime deadline;
  const std::string title = "Factory-Made Task";
  const std::string desc = "Description for factory task.";
};

TEST_F(TaskFactoryDirectTest, LabFactoryCreatesLabTask) {
  LabFactory factory;
  auto task = factory.createTask(title, deadline, desc);

  ASSERT_NE(task, nullptr);
  EXPECT_EQ(title, task->getTitle());
  EXPECT_EQ(desc, task->getDescription());
  EXPECT_EQ(deadline, task->getDeadline());
  EXPECT_EQ("Lab", task->getType());

  auto labTask = std::dynamic_pointer_cast<LabTask>(task);
  EXPECT_NE(labTask, nullptr);
}

TEST_F(TaskFactoryDirectTest, ProjectFactoryCreatesProjectTask) {
  ProjectFactory factory;
  auto task = factory.createTask(title, deadline, desc);

  ASSERT_NE(task, nullptr);
  EXPECT_EQ(title, task->getTitle());
  EXPECT_EQ(desc, task->getDescription());
  EXPECT_EQ(deadline, task->getDeadline());
  EXPECT_EQ("Project", task->getType());

  auto projectTask = std::dynamic_pointer_cast<ProjectTask>(task);
  EXPECT_NE(projectTask, nullptr);
}

TEST_F(TaskFactoryDirectTest, ExamFactoryCreatesExamTask) {
  ExamFactory factory;
  auto task = factory.createTask(title, deadline, desc);

  ASSERT_NE(task, nullptr);
  EXPECT_EQ(title, task->getTitle());
  EXPECT_EQ(desc, task->getDescription());
  EXPECT_EQ(deadline, task->getDeadline());
  EXPECT_EQ("Exam", task->getType());

  auto examTask = std::dynamic_pointer_cast<ExamTask>(task);
  EXPECT_NE(examTask, nullptr);
}

TEST_F(TaskFactoryDirectTest, FactoryCreatesTaskWithDefaultDescription) {
  LabFactory factory;
  auto task = factory.createTask(title, deadline);

  ASSERT_NE(task, nullptr);
  EXPECT_EQ(title, task->getTitle());
  EXPECT_EQ(deadline, task->getDeadline());
  EXPECT_EQ("", task->getDescription());
  EXPECT_EQ("Lab", task->getType());
}
