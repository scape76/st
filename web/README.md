# Academic Progress Tracker - WebAssembly Integration

This directory contains the web frontend for the Academic Progress Tracker application, which uses WebAssembly to provide C++ functionality in the browser.

## Overview

The WebAssembly integration allows you to:
- Create and manage subjects
- Create and manage tasks
- Mark tasks as completed with grades
- View task details and deadlines
- Use the same grading system as the desktop application

## Prerequisites

To build and run this application, you need:

- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) (version 3.1.0 or newer)
- CMake (version 3.10 or newer)
- A modern web browser with WebAssembly support
- A local web server to serve the files (e.g., Python's http.server)

## Building the WebAssembly Module

1. Ensure you have Emscripten SDK installed and activated in your terminal
2. Navigate to the root directory of the project
3. Run the build script:

```bash
./build_wasm.sh
```

This script will:
- Configure the project with CMake and Emscripten
- Build the WebAssembly module
- Copy the generated files to the `web` directory

## Running the Web Application

After building the WebAssembly module, you can run the web application:

1. Start a local web server in the `web` directory:

```bash
cd web
python3 -m http.server 8000
```

2. Open your browser and navigate to `http://localhost:8000`

## API Reference

The WebAssembly module exposes the following JavaScript functions:

### Subject Management

- `initApp()` - Initialize the application
- `createSubject(name, code, description)` - Create a new subject
- `getAllSubjects()` - Get all subjects
- `getSubject(index)` - Get a specific subject by index

### Task Management

- `createTask(subjectIndex, title, description, deadlineStr, taskType)` - Create a new task
- `getTasksForSubject(subjectIndex)` - Get all tasks for a subject
- `markTaskAsCompleted(subjectIndex, taskIndex, marks)` - Mark a task as completed with marks
- `getGradeForMarks(marks)` - Get the grade representation for marks

## Integration with JavaScript

For easier integration, you can use the provided `app.js` module which wraps the WebAssembly functions in a more JavaScript-friendly API:

```javascript
import { createAcademicTracker } from './app.js';

async function init() {
  const tracker = await createAcademicTracker();
  
  // Create a subject
  const subjectIndex = tracker.createSubject('Computer Science', 'CS101', 'Introduction to Computer Science');
  
  // Create a task
  const taskIndex = tracker.createTask(
    subjectIndex,
    'Assignment 1',
    'Write a Hello World program',
    '2023-12-01T12:00:00',
    1 // Lab
  );
  
  // Get all subjects
  const subjects = tracker.getAllSubjects();
  console.log(subjects);
}

init();
```

## Notes

- This WebAssembly integration provides the core functionality of the desktop application in a web browser
- Local storage is not yet implemented, so data will be lost when the page is refreshed
- For production use, consider adding proper error handling and data persistence

## Troubleshooting

If you encounter issues:

1. Make sure Emscripten is properly installed and activated
2. Check that all required dependencies are installed
3. Look for error messages in the browser console
4. Ensure you're using a modern browser with WebAssembly support