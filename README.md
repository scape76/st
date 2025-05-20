# Academic Progress Tracker

A C++ CLI application for students to track their academic progress, including courses, assignments, and notifications.

## Design Patterns Used

This project implements several Gang of Four (GoF) design patterns:

### Creational Patterns

1. **Singleton Pattern**: 
   - `NotificationManager` is implemented as a singleton to ensure a single point of control for all notifications in the system.

2. **Factory Method Pattern**: 
   - `AssignmentFactory` and its concrete implementations (`HomeworkFactory`, `ProjectFactory`, `ExamFactory`) provide a way to create different types of assignments without exposing creation logic.

3. **Builder Pattern**: 
   - `CourseBuilder` and `AssignmentBuilder` provide a fluent interface for step-by-step creation of complex objects.

### Structural Patterns

4. **Decorator Pattern**:
   - `AssignmentDecorator` serves as a base decorator that adds functionality to assignments.
   - `PriorityAssignmentDecorator` adds priority levels to assignments.
   - `TaggedAssignmentDecorator` adds custom tags to assignments.
   - `ColoredAssignmentDecorator` adds color-coding to assignments.

5. **Composite Pattern**:
   - `CompositeAssignment` allows assignments to be composed of multiple sub-assignments.
   - Enables creating complex assignment structures with parent-child relationships.
   - Supports operations that can be performed uniformly across the entire assignment hierarchy.

6. **Adapter Pattern**:
   - `UniversityGradingSystemAdapter` adapts the external university grading system to our application's interface.
   - `OnlinePlatformGradingSystemAdapter` adapts a different external learning platform system to the same interface.
   - Allows the application to work with multiple external systems through a consistent interface.

7. **Facade Pattern**:
   - `GradingSystemFacade` provides a simplified interface to the complex subsystem of grading systems.
   - Encapsulates the interactions with different grading systems behind a unified interface.
   - Makes it easy to switch between different grading systems without changing client code.

### Behavioral Patterns

8. **State Pattern**:
   - `AssignmentState` hierarchy manages how assignments behave based on their current state.
   - States include `NotStartedState`, `InProgressState`, `CompletedState`, and `OverdueState`.
   - Each state encapsulates state-specific behavior and handles transitions.
   - Allows assignments to change their behavior when their internal state changes.
   - Implementation involves state-specific methods like `start()`, `makeProgress()`, `complete()`, and `checkDeadline()`.
   - Context (Assignment) delegates behavior to current state object.
   - States manage their own transitions, keeping transition logic out of Assignment class.

9. **Observer Pattern**:
   - Implements a publish-subscribe mechanism for notifications.
   - `Observer` interface with concrete observers like `StudentObserver`, `DeadlineObserver`, `GradeObserver`, and `ProgressObserver`.
   - `Subject` interface allows objects to attach, detach, and notify observers.
   - Enables loose coupling between assignments and entities interested in their changes.
   - `ObservableAssignment` and `ObservableCourse` classes act as subjects that notify observers about events.
   - Events include deadline approaching, state changes, progress updates, and grade assignments.
   - Observers receive contextual information with each notification and respond accordingly.

10. **Template Method Pattern**:
    - `AssignmentProcess` defines a skeleton algorithm for assignment submission and evaluation.
    - `HomeworkProcess`, `ProjectProcess`, and `ExamProcess` implement specific steps differently.
    - Standardizes the overall workflow while allowing variation in individual steps.
    - Uses hook methods for optional customization of the process.
    - Template method `processAssignment()` orchestrates the algorithm steps: validate → check prerequisites → process submission → evaluate → provide feedback → update records.
    - Each step is implemented differently by subclasses while maintaining the overall workflow.
    - Hook methods `beforeSubmission()` and `afterCompletion()` provide optional extension points.

11. **Memento Pattern**:
    - Implements save/restore functionality for the application state.
    - `AssignmentMemento` and `CourseMemento` capture internal states without violating encapsulation.
    - `AcademicProgressMemento` stores complete snapshots of the system with multiple courses and assignments.
    - `MementoCaretaker` manages saved states and handles persistence to files.
    - Enables undo/redo and save/load operations without exposing object internals.
    - Originator classes (`Assignment` and `Course`) control the creation of and restoration from mementos.
    - File-based persistence implemented to save and load application state between sessions.
    - Implementation preserves object relationships when restoring from mementos.

## Features

- Create and manage courses
- Create and track assignments of different types (homework, project, exam)
- Set deadlines for assignments
- Configure notifications for upcoming deadlines
- View detailed information about courses and assignments
- Mark assignments as completed and record grades
- Create complex assignments with sub-assignments (composite assignments)
- Add priority levels, tags, and color-coding to assignments
- Submit assignments to external grading systems
- Switch between different grading systems
- Track assignment states (Not Started, In Progress, Completed, Overdue) with automatic state transitions
- Receive notifications through different observer types (Student, Deadline, Grade, Progress)
- Process assignments using standardized workflows with customizable steps for different assignment types
- Save application state and restore it later (including to/from files) using mementos
- Support for undo/redo functionality via saved application states
- RESTful API for frontend integration with comprehensive endpoints for all features

## API Architecture

The system includes a complete API layer that enables frontend integration using several Gang of Four design patterns:

### API Components

1. **APIFacade**: Unified entry point to all system functionality (Facade Pattern)
   - Encapsulates the backend complexity behind a simple interface
   - Aggregates related operations into a cohesive API surface
   - Manages conversion between internal models and external DTOs

2. **Command Layer**: Request handling system (Command Pattern)
   - Each API endpoint corresponds to a specific Command object
   - `CommandDispatcher` routes requests to appropriate command handlers
   - Commands encapsulate operations and ensure separation of concerns

3. **Data Transfer Objects**: Standardized data exchange (Adapter Pattern)
   - DTOs provide a clean contract between frontend and backend
   - Adapters convert between internal models and external representations
   - Ensures proper encapsulation of backend implementation details

4. **Web Server**: HTTP communication management
   - Responds to RESTful requests (GET, POST, PUT, DELETE, PATCH)
   - Incorporates middleware for logging, authentication, and CORS
   - Provides standardized error handling and response formatting

### API Endpoints

The API exposes comprehensive endpoints for all system features:

- `/api/courses` - Course management operations
- `/api/courses/:id/assignments` - Assignment operations
- `/api/notifications` - Notification management
- `/api/states` - Saved state operations
- `/api/grading-systems` - Grading system operations
- `/api/courses/:id/assignments/:id/state` - State transitions
- `/api/courses/:id/composite-assignments` - Composite pattern operations
- `/api/courses/:id/assignments/:id/decorate` - Decorator pattern operations
- `/api/courses/:id/assignments/:id/process` - Template method operations
- `/api/courses/:id/assignments/:id/subscribe` - Observer pattern operations

### Real-time Updates

The API supports real-time updates using the Observer pattern:
- WebSocket connections for live notification delivery
- Event-based architecture for state changes
- Support for subscription to specific entities and event types

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