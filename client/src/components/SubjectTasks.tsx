import { Badge } from "@/components/ui/badge";
import { Button } from "@/components/ui/button";
import * as Kanban from "@/components/ui/kanban";
import { subjectRoute } from "@/routes/subject";
import type { Task } from "@/types";
import { GripVertical } from "lucide-react";
import * as React from "react";

const COLUMN_TITLES: Record<string, string> = {
  backlog: "Backlog",
  inProgress: "In Progress",
  review: "Review",
  done: "Done",
};

export function SubjectTasks() {
  const tasks = subjectRoute.useLoaderData({
    select: (data) => data.tasks,
  });

  const [columns, setColumns] = React.useState<Record<string, Task[]>>({
    backlog: [
      {
        id: "1",
        title: "Add authentication",
        priority: "high",
        assignee: "John Doe",
        dueDate: "2024-04-01",
      },
      {
        id: "2",
        title: "Create API endpoints",
        priority: "medium",
        assignee: "Jane Smith",
        dueDate: "2024-04-05",
      },
      {
        id: "3",
        title: "Write documentation",
        priority: "low",
        assignee: "Bob Johnson",
        dueDate: "2024-04-10",
      },
    ],
    inProgress: [
      {
        id: "4",
        title: "Design system updates",
        priority: "high",
        assignee: "Alice Brown",
        dueDate: "2024-03-28",
      },
      {
        id: "5",
        title: "Implement dark mode",
        priority: "medium",
        assignee: "Charlie Wilson",
        dueDate: "2024-04-02",
      },
    ],
    done: [
      {
        id: "7",
        title: "Setup project",
        priority: "high",
        assignee: "Eve Davis",
        dueDate: "2024-03-25",
      },
      {
        id: "8",
        title: "Initial commit",
        priority: "low",
        assignee: "Frank White",
        dueDate: "2024-03-24",
      },
    ],
  });

  return (
    <Kanban.Root
      value={columns}
      onValueChange={setColumns}
      getItemValue={(item) => item.title}
    >
      <Kanban.Board className="grid auto-rows-fr grid-cols-3">
        {Object.entries(columns).map(([columnValue, tasks]) => (
          <TaskColumn key={columnValue} value={columnValue} tasks={tasks} />
        ))}
      </Kanban.Board>
      <Kanban.Overlay>
        {({ value, variant }) => {
          if (variant === "column") {
            const tasks = columns[value] ?? [];

            return <TaskColumn value={value} tasks={tasks} />;
          }

          const task = Object.values(columns)
            .flat()
            .find((task) => task.title === value);

          if (!task) return null;

          return <TaskCard task={task} />;
        }}
      </Kanban.Overlay>
    </Kanban.Root>
  );
}

interface TaskCardProps
  extends Omit<React.ComponentProps<typeof Kanban.Item>, "value"> {
  task: Task;
}

function TaskCard({ task, ...props }: TaskCardProps) {
  return (
    <Kanban.Item key={task.id} value={task.id} asChild {...props}>
      <div className="rounded-md border bg-card p-3 shadow-xs">
        <div className="flex flex-col gap-2">
          <div className="flex items-center justify-between gap-2">
            <span className="line-clamp-1 font-medium text-sm">
              {task.title}
            </span>
            <Badge
              variant={
                task.priority === "high"
                  ? "destructive"
                  : task.priority === "medium"
                    ? "default"
                    : "secondary"
              }
              className="pointer-events-none h-5 rounded-sm px-1.5 text-[11px] capitalize"
            >
              {task.priority}
            </Badge>
          </div>
          <div className="flex items-center justify-between text-muted-foreground text-xs">
            {task.assignee && (
              <div className="flex items-center gap-1">
                <div className="size-2 rounded-full bg-primary/20" />
                <span className="line-clamp-1">{task.assignee}</span>
              </div>
            )}
            {task.dueDate && (
              <time className="text-[10px] tabular-nums">{task.dueDate}</time>
            )}
          </div>
        </div>
      </div>
    </Kanban.Item>
  );
}

interface TaskColumnProps
  extends Omit<React.ComponentProps<typeof Kanban.Column>, "children"> {
  tasks: Task[];
}

function TaskColumn({ value, tasks, ...props }: TaskColumnProps) {
  return (
    <Kanban.Column value={value} {...props}>
      <div className="flex items-center justify-between">
        <div className="flex items-center gap-2">
          <span className="font-semibold text-sm">{COLUMN_TITLES[value]}</span>
          <Badge variant="secondary" className="pointer-events-none rounded-sm">
            {tasks.length}
          </Badge>
        </div>
        <Kanban.ColumnHandle asChild>
          <Button variant="ghost" size="icon">
            <GripVertical className="h-4 w-4" />
          </Button>
        </Kanban.ColumnHandle>
      </div>
      <div className="flex flex-col gap-2 p-0.5">
        {tasks.map((task) => (
          <TaskCard key={task.id} task={task} asHandle />
        ))}
      </div>
    </Kanban.Column>
  );
}
