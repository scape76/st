import { createRoute, Link } from "@tanstack/react-router";
import { Button } from "../components/ui/button";
import { rootRoute } from "./root";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { PlusIcon } from "lucide-react";
import { CreateSubjectDialog } from "@/components/CreateSubjectDialog";
import { CreateInternshipDialog } from "@/components/CreateInternshipDialog";
import type { Internship } from "@/types";
import { useState } from "react";
import { Badge } from "@/components/ui/badge";
import {
  formatDateDisplay,
  getStatusVariant,
  InternshipInfoDialog,
} from "@/components/InternshipInfoDialog";

const indexRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: "/",
  loader: ({ context }) => {
    return {
      subjects: context.at.getAllSubjects(),
      resumes: context.at.getAllResumes(),
      internships: context.at.getAllInternships(),
    };
  },
  component: Route,
});

function Route() {
  const { subjects, internships, resumes } = indexRoute.useLoaderData();

  console.log("internships ", internships);

  const [selectedInternship, setSelectedInternship] =
    useState<Internship | null>(null);

  const [isInternshipInfoOpen, setIsInternshipInfoOpen] = useState(false);

  const handleInternshipCardClick = (internship: Internship) => {
    setSelectedInternship(internship);
    setIsInternshipInfoOpen(true);
  };

  return (
    <main className="container mx-auto p-8">
      <div className="flex justify-between sm:items-center flex-col sm:flex-row gap-2">
        <h1 className="text-2xl font-bold">Мої Предмети</h1>
        <CreateSubjectDialog />
      </div>
      <div className="mt-6 grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-2 md:gap-4">
        {subjects.map((subject) => {
          return (
            <Link
              to="/subject/$code"
              params={{
                code: subject.code,
              }}
            >
              <Card key={subject.code}>
                <CardHeader>
                  <CardTitle>{subject.name}</CardTitle>
                  <CardDescription>{subject.code}</CardDescription>
                </CardHeader>
                <CardContent>{subject.description}</CardContent>
              </Card>
            </Link>
          );
        })}
      </div>
      <div className="flex justify-between sm:items-center flex-col sm:flex-row gap-2 mt-6">
        <h1 className="text-2xl font-bold">Мої Резюме</h1>

        <Button variant="outline" asChild>
          <Link to="/create-resume">
            <PlusIcon />
            Нове Резюме
          </Link>
        </Button>
      </div>
      <div className="mt-6 grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-2 md:gap-4">
        {resumes.map((resume) => (
          <Link
            key={resume.id}
            to="/resume/$id"
            params={{ id: resume.id }}
            className="block hover:shadow-lg transition-shadow rounded-lg"
          >
            <Card>
              <CardHeader>
                <CardTitle>{resume.title}</CardTitle>
              </CardHeader>
              <CardContent>
                <p className="line-clamp-3 text-sm text-muted-foreground">
                  {resume.body || "Опис відсутній."}
                </p>
              </CardContent>
            </Card>
          </Link>
        ))}
      </div>
      <div className="flex justify-between sm:items-center flex-col sm:flex-row gap-2 mt-6">
        <h1 className="text-2xl font-bold">Мої Стажування</h1>

        <CreateInternshipDialog />
      </div>
      <div className="mt-6 grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-2 md:gap-4">
        {internships.map((internship) => (
          <Card
            key={internship.id}
            onClick={() => handleInternshipCardClick(internship)}
            className="cursor-pointer hover:shadow-lg transition-shadow"
          >
            <CardHeader>
              <CardTitle>{internship.position}</CardTitle>
              <CardDescription>{internship.company}</CardDescription>
            </CardHeader>
            <CardContent>
              <div className="flex items-center">
                <span className="text-sm mr-2">Статус:</span>
                <Badge variant={getStatusVariant(internship.status)}>
                  {internship.status}
                </Badge>
              </div>
              <p className="text-sm text-muted-foreground mt-2">
                {formatDateDisplay(internship.startDate)} -{" "}
                {formatDateDisplay(internship.endDate)}
              </p>
            </CardContent>
          </Card>
        ))}
        {internships.length === 0 && (
          <p className="text-muted-foreground col-span-full">
            Стажування не знайдено.
          </p>
        )}
      </div>

      <InternshipInfoDialog
        internship={selectedInternship}
        open={isInternshipInfoOpen}
        onOpenChange={setIsInternshipInfoOpen}
      />
    </main>
  );
}

export { indexRoute };
