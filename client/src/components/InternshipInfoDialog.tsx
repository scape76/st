import {
  Dialog,
  DialogContent,
  DialogHeader,
  DialogTitle,
  DialogDescription,
} from "@/components/ui/dialog";
import { Badge } from "@/components/ui/badge";
import { format } from "date-fns";
import type { Internship, InternshipStatusString } from "@/types";

interface InternshipInfoDialogProps {
  internship: Internship | null;
  open: boolean;
  onOpenChange: (open: boolean) => void;
}

export const formatDateDisplay = (
  dateString: string | null | undefined,
): string => {
  if (!dateString) return "N/A";
  try {
    return format(new Date(dateString), "PPP");
  } catch (error) {
    return dateString;
  }
};

export const getStatusVariant = (
  status: InternshipStatusString,
): "default" | "secondary" | "destructive" | "outline" => {
  switch (status) {
    case "Started":
      return "default";
    case "Pending":
      return "secondary";
    case "Ended":
      return "outline";
    case "Cancelled":
      return "destructive";
    case "Unknown":
    default:
      return "secondary";
  }
};

export function InternshipInfoDialog({
  internship,
  open,
  onOpenChange,
}: InternshipInfoDialogProps) {
  if (!internship) {
    return null;
  }

  return (
    <Dialog open={open} onOpenChange={onOpenChange}>
      <DialogContent className="sm:max-w-md">
        <DialogHeader>
          <DialogTitle>{internship.position}</DialogTitle>
          <DialogDescription>{internship.company}</DialogDescription>
        </DialogHeader>
        <div className="mt-4 space-y-4">
          <div>
            <h4 className="text-sm font-medium text-muted-foreground">
              Статус
            </h4>
            <Badge
              variant={getStatusVariant(internship.status)}
              className="mt-1"
            >
              {internship.status}
            </Badge>
          </div>
          <div>
            <h4 className="text-sm font-medium text-muted-foreground">
              Дата початку
            </h4>
            <p className="text-sm">{formatDateDisplay(internship.startDate)}</p>
          </div>
          <div>
            <h4 className="text-sm font-medium text-muted-foreground">
              Дата закінчення
            </h4>
            <p className="text-sm">{formatDateDisplay(internship.endDate)}</p>
          </div>
        </div>
      </DialogContent>
    </Dialog>
  );
}
