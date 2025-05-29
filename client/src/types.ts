type Subject = {
  name: string;
  code: string;
  description: string;
};

export type InternshipStatusString =
  | "Pending"
  | "Started"
  | "Ended"
  | "Cancelled"
  | "Unknown";

// 0: PENDING, 1: STARTED, 2: ENDED, 3: CANCELLED
export type InternshipStatusInt = 0 | 1 | 2 | 3;

type LabType = "lab" | "exam" | "project";

type Task = {
  title: string;
  description: string;
  deadline: string;
  type: LabType;
  completed: boolean;
  marks: number;
};

type Resume = {
  id: string;
  title: string;

  body: string;
};

type Internship = {
  id: string;
  company: string;
  position: string;
  status: InternshipStatusString;
  startDate: string;
  endDate: string;
};

export type AcademicTracker = {
  seed: () => void;
  createSubject: (name: string, code: string, description: string) => void;
  getAllSubjects: () => Subject[];
  getSubject: (code: string) => Subject;
  getSubjectTasks: (subjectCode: string) => Task[];

  createTask: (
    subjectCode: string,
    title: string,
    description: string,
    deadline: string,
    type: LabType,
  ) => void;

  convertMarkdownToHtml: (markdown: string) => string;
  convertAsciiDocToHtml: (asciidoc: string) => string;

  createNewResume: (title: string, body: string) => Resume["id"];
  getAllResumes: () => Resume[];
  getResume: (id: string) => Resume;
  createInternship: (
    company: string,
    position: string,
    status: InternshipStatusInt,
    startDate: string,
    endDate: string,
  ) => string;
  getAllInternships: () => Internship[];
};
