import { Button, Chip, PressEvent } from "@heroui/react";


export type RelayViewProps = {
  status: boolean
  onToggle: (e: PressEvent) => void;
};
const RelayView = ({ status, onToggle }: RelayViewProps) => {

  return (
    <div className="flex items-center justify-between p-4 border rounded-lg">
       <div>
         <h3 className="font-semibold">Relay 1</h3>
         <p className="text-sm text-gray-600">GPIO5 (D1)</p>
         <div className="flex items-center gap-2 mt-1">
           <span className="text-sm">Status:</span>
           <Chip
             size="sm"
             color={status ? "success" : "default"}
             variant="flat"
           >
             {status ? "ON" : "OFF"}
           </Chip>
         </div>
       </div>
       <Button
         color={status ? "danger" : "success"}
         variant="solid"
         onPress={onToggle}
         size="lg"
       >
         {status ? "Turn OFF" : "Turn ON"}
       </Button>
     </div>
  );
};

export default RelayView;
