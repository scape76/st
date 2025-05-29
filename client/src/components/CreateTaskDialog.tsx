import {
  Dialog,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import {
  Popover,
  PopoverContent,
  PopoverTrigger,
} from "@/components/ui/popover";
import { useForm } from "@tanstack/react-form";
import { useState } from "react";
import { Button } from "./ui/button";
import { CalendarIcon, PlusIcon } from "lucide-react";
import { Calendar } from "@/components/ui/calendar";
import { format } from "date-fns";
import { useRouter } from "@tanstack/react-router";
import { subjectRoute } from "@/routes/subject";
import { Textarea } from "./ui/textarea";

const taskTypes = ["Лабораторна", "Проєкт", "Екзамен"];

export function CreateTaskDialog() {
  const router = useRouter();
  const at = subjectRoute.useRouteContext({
    select: (c) => c.at,
  });

  const subjectCode = subjectRoute.useLoaderData({
    select: (data) => data.subject.code,
  });

  const [open, setOpen] = useState(false);

  const form = useForm({
    defaultValues: {
      title: "",
      description: "",
      deadline: "",
      type: 1,
    },
    onSubmit: async ({ value }) => {
      at.createTask(
        subjectCode,
        value.title,
        value.description,
        value.deadline,
        value.type,
      );
      await router.invalidate();
      setOpen(false);
      form.reset();
    },
  });

  return (
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogTrigger asChild>
        <Button variant="outline">
          <PlusIcon className="mr-2 h-4 w-4" />
          Додати Завдання
        </Button>
      </DialogTrigger>
      <DialogContent>
        <DialogHeader>
          <DialogTitle>Нове Завдання</DialogTitle>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <form.Field
            name="title"
            children={(field) => (
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor={field.name} className="text-right">
                  Назва
                </Label>
                <Input
                  id={field.name}
                  name={field.name}
                  value={field.state.value}
                  onBlur={field.handleBlur}
                  onChange={(e) => field.handleChange(e.target.value)}
                  className="col-span-3"
                  placeholder="Лабораторна 5"
                />
              </div>
            )}
          />

          <form.Field
            name="description"
            children={(field) => (
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor={field.name} className="text-right">
                  Опис
                </Label>
                <Textarea
                  placeholder="Structural Design patterns"
                  id={field.name}
                  name={field.name}
                  value={field.state.value}
                  onBlur={field.handleBlur}
                  onChange={(e) => field.handleChange(e.target.value)}
                  className="col-span-3"
                />
              </div>
            )}
          />

          <form.Field
            name="type"
            children={(field) => (
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor={field.name} className="text-right">
                  Тип
                </Label>
                <Select
                  value={field.state.value.toString()}
                  onValueChange={(value) => field.handleChange(Number(value))}
                >
                  <SelectTrigger id={field.name} className="col-span-3">
                    <SelectValue placeholder="Виберіть тип завдання" />
                  </SelectTrigger>
                  <SelectContent>
                    {taskTypes.map((type, i) => (
                      <SelectItem key={type} value={(i + 1).toString()}>
                        {type}
                      </SelectItem>
                    ))}
                  </SelectContent>
                </Select>
              </div>
            )}
          />

          <form.Field
            name="deadline"
            children={(field) => {
              const selectedDate = field.state.value
                ? new Date(field.state.value)
                : undefined;
              return (
                <div className="grid grid-cols-4 items-center gap-4">
                  <Label htmlFor={field.name} className="text-right">
                    Дедлайн
                  </Label>
                  <Popover>
                    <PopoverTrigger asChild>
                      <Button
                        id={field.name}
                        variant={"outline"}
                        className={`col-span-3 justify-start text-left font-normal ${
                          !field.state.value && "text-muted-foreground"
                        }`}
                      >
                        <CalendarIcon className="mr-2 h-4 w-4" />
                        {selectedDate ? (
                          format(selectedDate, "PPP")
                        ) : (
                          <span>Оберіть дату</span>
                        )}
                      </Button>
                    </PopoverTrigger>
                    <PopoverContent className="w-auto p-0">
                      <Calendar
                        mode="single"
                        selected={selectedDate}
                        onSelect={(date) =>
                          field.handleChange(
                            date ? format(date, "yyyy-MM-dd") : "",
                          )
                        }
                        initialFocus
                      />
                    </PopoverContent>
                  </Popover>
                </div>
              );
            }}
          />
        </div>
        <DialogFooter>
          <form.Subscribe
            selector={(state) => [state.canSubmit, state.isSubmitting]}
            children={([canSubmit, isSubmitting]) => (
              <Button
                type="button"
                onClick={form.handleSubmit}
                disabled={!canSubmit || isSubmitting}
              >
                {isSubmitting ? "Збереження..." : "Зберегти"}{" "}
              </Button>
            )}
          />
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}
