import { createRoute, Link, useRouter } from "@tanstack/react-router";
import { rootRoute } from "./root";

import { SubjectTasks } from "@/components/SubjectTasks";

import {
  Breadcrumb,
  BreadcrumbItem,
  BreadcrumbLink,
  BreadcrumbList,
  BreadcrumbPage,
  BreadcrumbSeparator,
} from "@/components/ui/breadcrumb";

import { CreateTaskDialog } from "@/components/CreateTaskDialog";
import { Button } from "@/components/ui/button";
import { UndoIcon } from "lucide-react";

const subjectRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: "/subject/$code",
  loader: ({ context, params }) => {
    const subject = context.at.getSubject(params.code);
    const tasks = context.at.getSubjectTasks(params.code);

    return {
      subject,
      tasks,
    };
  },
  component: Route,
  remountDeps: ({}) => [],
  notFoundComponent: () => "not Found!!",
});

function Route() {
  const router = useRouter();

  const { subject, tasks } = subjectRoute.useLoaderData();
  const at = subjectRoute.useRouteContext({
    select: (c) => c.at,
  });

  return (
    <main className="container mx-auto p-8">
      <div className="flex justify-between sm:items-center flex-col sm:flex-row gap-2 mb-6">
        <Breadcrumb>
          <BreadcrumbList>
            <BreadcrumbItem>
              <BreadcrumbLink asChild>
                <Link to="/">Панель</Link>
              </BreadcrumbLink>
            </BreadcrumbItem>
            <BreadcrumbSeparator />
            <BreadcrumbItem>
              <BreadcrumbPage>{subject.name}</BreadcrumbPage>
            </BreadcrumbItem>
          </BreadcrumbList>
        </Breadcrumb>
        <div className="flex items-center gap-2">
          <Button
            variant={"ghost"}
            onClick={() => {
              at.undoLastTaskCommand();
              router.invalidate();
            }}
          >
            <UndoIcon />
            Undo
          </Button>
          <CreateTaskDialog />
        </div>
      </div>
      <SubjectTasks
        tasks={tasks}
        key={tasks.map((t, i) => `${i}${t.stateName}`).join()}
      />
    </main>
  );
}

export { subjectRoute };
