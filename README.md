# Academic Progress Tracker

A C++ CLI application for students to track their academic progress, including courses, assignments, and notifications.

## Design Patterns Used

This project implements several key design patterns:

1. **Singleton Pattern**: 
   - `NotificationManager` is implemented as a singleton to ensure a single point of control for all notifications in the system.

2. **Factory Method Pattern**: 
   - `AssignmentFactory` and its concrete implementations (`HomeworkFactory`, `ProjectFactory`, `ExamFactory`) provide a way to create different types of assignments without exposing creation logic.

3. **Builder Pattern**: 
   - `CourseBuilder` and `AssignmentBuilder` provide a fluent interface for step-by-step creation of complex objects.

## Features

- Create and manage courses
- Create and track assignments of different types (homework, project, exam)
- Set deadlines for assignments
- Configure notifications for upcoming deadlines
- View detailed information about courses and assignments

## Building the Project

### Prerequisites

- C++17 compatible compiler
- CMake (version 3.10 or higher)

### Build Steps

```
cd st
mkdir build
cd build
cmake ..
make
```

This will create an executable called `academic_tracker` in the build directory.

## Usage

Run the program:

```
./academic_tracker
```

Follow the interactive menu to:

1. Create courses
2. Add assignments with deadlines
3. Set up notifications
4. View your academic progress

## Data Model

- **Course**: Represents an academic course with a name, code, and description
- **Assignment**: Base class for all assignments with title, description, and deadline
  - Types: HomeworkAssignment, ProjectAssignment, ExamAssignment
- **Notification**: Represents reminders for upcoming deadlines

## Future Enhancements

- Persistent storage (save/load data)
- Grade tracking
- Progress statistics and visualization
- Multiple notification types (email, calendar integration)
- Web/mobile interface

## License

This project is released under the MIT License.