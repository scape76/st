import { createRoute, Link, useNavigate } from "@tanstack/react-router";
import { rootRoute } from "./root";
import { useForm } from "@tanstack/react-form";
import {
  Breadcrumb,
  BreadcrumbItem,
  BreadcrumbLink,
  BreadcrumbList,
  BreadcrumbPage,
  BreadcrumbSeparator,
} from "@/components/ui/breadcrumb";
import { Input } from "@/components/ui/input";
import { Textarea } from "@/components/ui/textarea";
import { Button } from "@/components/ui/button";
import {
  Select,
  SelectContent,
  SelectGroup,
  SelectItem,
  SelectLabel,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";

import * as z from "zod";
import { Label } from "@/components/ui/label";
import { useCallback, useState } from "react";

export const createResumeRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: "/create-resume",
  component: Route,
});

const resumeSchema = z.object({
  title: z
    .string()
    .min(2, "Назва не може бути меншою за 2 символи")
    .max(100, "Назва занадто довга"),
  body: z
    .string()
    .min(10, "Тіло резюме занадто коротке")
    .max(1000, "Тіло резюме занадто довге"),
  conversionType: z.union([z.literal("markdown"), z.literal("asciidoc")]),
});

function Route() {
  const at = createResumeRoute.useRouteContext({
    select: (c) => c.at,
  });

  const navigate = useNavigate();

  const convertToHtml = useCallback(
    ({
      title,
      body,
      conversionType,
    }: {
      title: string;
      body: string;
      conversionType: "markdown" | "asciidoc";
    }) => {
      if (conversionType === "markdown") {
        let base = title ? `# ${title} \n` : "";

        base += body;

        return at.convertMarkdownToHtml(base);
      } else {
        let base = title ? `= ${title} \n` : "";

        base += body;

        return at.convertAsciiDocToHtml(base);
      }
    },
    [at],
  );

  const [preview, setPreview] = useState("");

  const form = useForm({
    defaultValues: {
      title: "",
      conversionType: "markdown",
      body: "",
    },
    validators: {
      onChange: resumeSchema,
    },
    onSubmit: ({ value }) => {
      const id = at.createNewResume(value.title, convertToHtml(value));

      navigate({
        to: "/resume/$id",
        params: {
          id,
        },
      });
    },
    listeners: {
      onChange: (values) => {
        const title = values.formApi.getFieldValue("title");
        const body = values.formApi.getFieldValue("body");
        const conversionType = values.formApi.getFieldValue("conversionType");

        const html = convertToHtml({
          title,
          body,
          conversionType: conversionType as any,
        });

        setPreview(html);
      },
    },
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
              <BreadcrumbPage>Створити Резюме</BreadcrumbPage>
            </BreadcrumbItem>
          </BreadcrumbList>
        </Breadcrumb>
        <Button
          type="submit"
          disabled={!form.state.isFormValid}
          onClick={form.handleSubmit}
        >
          Зберегти
        </Button>
      </div>
      <div className="flex flex-col gap-4">
        <div className="flex flex-col gap-2">
          <Label htmlFor="title">Назва</Label>
          <form.Field
            name="title"
            children={(field) => (
              <Input
                id={field.name}
                value={field.state.value}
                className="col-span-3"
                onChange={(e) => field.handleChange(e.target.value)}
                placeholder="OCaml Developer"
              />
            )}
          />
        </div>

        <form.Field
          name="conversionType"
          children={(field) => (
            <Select
              value={field.state.value}
              onValueChange={(s) => field.handleChange(s as any)}
            >
              <SelectTrigger className="w-[180px]">
                <SelectValue placeholder="Тип конвертації" />
              </SelectTrigger>
              <SelectContent>
                <SelectGroup>
                  <SelectLabel>Тип Конвертації</SelectLabel>
                  <SelectItem value="markdown">Markdown</SelectItem>
                  <SelectItem value="asciidoc">AsciiDoc</SelectItem>
                </SelectGroup>
              </SelectContent>
            </Select>
          )}
        />

        <div className="flex flex-col gap-2">
          <Label htmlFor="body">Резюме</Label>
          <form.Field
            name="body"
            children={(field) => (
              <Textarea
                id={field.name}
                value={field.state.value}
                className="col-span-3"
                onChange={(e) => field.handleChange(e.target.value)}
                placeholder="# I am a passionate developer"
              />
            )}
          />
        </div>

        <div
          className="prose lg:prose-xl"
          dangerouslySetInnerHTML={{
            __html: preview,
          }}
        ></div>
      </div>
    </main>
  );
}
