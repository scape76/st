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
import { indexRoute } from "../routes/index";
import { useRouter } from "@tanstack/react-router";
import type { InternshipStatusInt } from "@/types";

const internshipStatuses = [
  "pending",
  "started",
  "ended",
  "cancelled",
] as const;
type InternshipStatus = (typeof internshipStatuses)[number];

export function CreateInternshipDialog() {
  const router = useRouter();
  const at = indexRoute.useRouteContext({
    select: (c) => c.at,
  });

  const [open, setOpen] = useState(false);

  const form = useForm({
    defaultValues: {
      company: "",
      position: "",
      status: "",
      startDate: "",
      endDate: "",
    },
    onSubmit: async ({ value }) => {
      at.createInternship(
        value.company,
        value.position,
        internshipStatuses.indexOf(
          value.status as InternshipStatus,
        ) as InternshipStatusInt,
        value.startDate,
        value.endDate,
      );
      router.invalidate({
        filter: (r) => r.fullPath === "/",
      });
      setOpen(false);
      form.reset();
    },
  });

  return (
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogTrigger asChild>
        <Button variant="outline">
          <PlusIcon className="mr-2 h-4 w-4" />
          Додати Стажування
        </Button>
      </DialogTrigger>
      <DialogContent>
        <DialogHeader>
          <DialogTitle>Нове Стажування</DialogTitle>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <form.Field
            name="company"
            children={(field) => (
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor={field.name} className="text-right">
                  Компанія
                </Label>
                <Input
                  id={field.name}
                  name={field.name}
                  value={field.state.value}
                  onBlur={field.handleBlur}
                  onChange={(e) => field.handleChange(e.target.value)}
                  className="col-span-3"
                  placeholder="Назва компанії"
                />
              </div>
            )}
          />

          <form.Field
            name="position"
            children={(field) => (
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor={field.name} className="text-right">
                  Посада
                </Label>
                <Input
                  id={field.name}
                  name={field.name}
                  value={field.state.value}
                  onBlur={field.handleBlur}
                  onChange={(e) => field.handleChange(e.target.value)}
                  className="col-span-3"
                  placeholder="Назва посади"
                />
              </div>
            )}
          />

          <form.Field
            name="status"
            children={(field) => (
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor={field.name} className="text-right">
                  Статус
                </Label>
                <Select
                  value={field.state.value}
                  onValueChange={(value) =>
                    field.handleChange(value as InternshipStatus)
                  }
                >
                  <SelectTrigger id={field.name} className="col-span-3">
                    <SelectValue placeholder="Виберіть статус" />
                  </SelectTrigger>
                  <SelectContent>
                    {internshipStatuses.map((status) => (
                      <SelectItem key={status} value={status}>
                        {status.charAt(0).toUpperCase() + status.slice(1)}
                      </SelectItem>
                    ))}
                  </SelectContent>
                </Select>
              </div>
            )}
          />

          <form.Field
            name="startDate"
            children={(field) => {
              const selectedDate = field.state.value
                ? new Date(field.state.value)
                : undefined;
              return (
                <div className="grid grid-cols-4 items-center gap-4">
                  <Label htmlFor={field.name} className="text-right">
                    Дата початку
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

          <form.Field
            name="endDate"
            children={(field) => {
              const selectedDate = field.state.value
                ? new Date(field.state.value)
                : undefined;
              const startDateValue = form.getFieldValue("startDate");
              return (
                <div className="grid grid-cols-4 items-center gap-4">
                  <Label
                    htmlFor={field.name}
                    className="text-right text-nowrap"
                  >
                    Дата закінчення
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
                        disabled={(date) =>
                          startDateValue
                            ? date <
                              new Date(
                                new Date(startDateValue).setDate(
                                  new Date(startDateValue).getDate() + 1,
                                ),
                              )
                            : false
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
