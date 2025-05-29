import { createRoute, Link } from "@tanstack/react-router";
import { rootRoute } from "./root";
import {
  Breadcrumb,
  BreadcrumbItem,
  BreadcrumbLink,
  BreadcrumbList,
  BreadcrumbPage,
  BreadcrumbSeparator,
} from "@/components/ui/breadcrumb";

export const resumeRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: "/resume/$id",
  loader: ({ context, params }) => {
    const resume = context.at.getResume(params.id);

    console.log("returned resume sio sd asd as d ", resume);

    return resume;
  },
  component: Route,
});

function Route() {
  const resume = resumeRoute.useLoaderData();

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
              <BreadcrumbPage>Резюме</BreadcrumbPage>
            </BreadcrumbItem>
            <BreadcrumbSeparator />
            <BreadcrumbItem>
              <BreadcrumbPage>{resume.title}</BreadcrumbPage>
            </BreadcrumbItem>
          </BreadcrumbList>
        </Breadcrumb>
      </div>
      <div
        className="prose lg:prose-xl"
        dangerouslySetInnerHTML={{
          __html: resume.body,
        }}
      ></div>
    </main>
  );
}
