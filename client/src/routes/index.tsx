import { createRoute, Link, useRouter } from "@tanstack/react-router";
import { Button } from "../components/ui/button";
import { rootRoute } from "./root";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { useForm } from "@tanstack/react-form";
import { Label } from "@/components/ui/label";
import { Input } from "@/components/ui/input";
import { Textarea } from "@/components/ui/textarea";
import { useState } from "react";

const indexRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: "/",
  loader: ({ context }) => {
    return context.at.getAllSubjects();
  },
  component: Route,
});

function Route() {
  const subjects = indexRoute.useLoaderData();

  console.log("subjects", subjects);

  return (
    <main className="container mx-auto p-8">
      <div className="flex justify-between sm:items-center flex-col sm:flex-row gap-2">
        <h1 className="text-2xl font-bold">Мої Предмети</h1>
        <CreateNewDialog />
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
    </main>
  );
}

function CreateNewDialog() {
  const [open, setOpen] = useState(false);
  const router = useRouter();

  const at = indexRoute.useRouteContext({
    select: (c) => c.at,
  });

  const form = useForm({
    defaultValues: {
      name: "",
      code: "",
      description: "",
    },
    onSubmit: ({ value }) => {
      console.log("value is ", value);
      at.createSubject(value.name, value.code, value.description);
      router.invalidate({
        filter: (c) => c.fullPath === "/",
      });
      setOpen(false);
    },
  });

  return (
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogTrigger asChild>
        <Button variant="outline">Створити Новий</Button>
      </DialogTrigger>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>Новий Предмет</DialogTitle>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="name" className="text-right">
              Назва
            </Label>
            <form.Field
              name="name"
              children={(field) => (
                <Input
                  id={field.name}
                  value={field.state.value}
                  className="col-span-3"
                  onChange={(e) => field.handleChange(e.target.value)}
                />
              )}
            />
          </div>
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="code" className="text-right">
              Код
            </Label>
            <form.Field
              name="code"
              children={(field) => (
                <Input
                  id={field.name}
                  value={field.state.value}
                  className="col-span-3"
                  onChange={(e) => field.handleChange(e.target.value)}
                />
              )}
            />
          </div>
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="code" className="text-right">
              Опис
            </Label>
            <form.Field
              name="description"
              children={(field) => (
                <Textarea
                  id={field.name}
                  value={field.state.value}
                  className="col-span-3"
                  onChange={(e) => field.handleChange(e.target.value)}
                />
              )}
            />
          </div>
        </div>
        <DialogFooter>
          <Button type="submit" onClick={form.handleSubmit}>
            Зберегти
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}

export { indexRoute };
