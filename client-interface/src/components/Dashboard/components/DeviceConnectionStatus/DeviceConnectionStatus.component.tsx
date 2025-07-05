import React from 'react';
import DeviceStatus from '@models/DeviceStatus';
import { Chip } from '@heroui/react';
import { getSignalColor, getSignalStrength } from '@utils/signal.util';


export type DeviceConnectionStatusProps = {
  deviceStatus: DeviceStatus;
};

const DeviceConnectionStatus: React.FC<DeviceConnectionStatusProps> = ({
  deviceStatus
}) => {

  const { connected, ssid, ip, rssi } = deviceStatus;
  return (
    <div className="grid grid-cols-1 md:grid-cols-2 gap-4" >
      <div className="space-y-3">
        <div className="flex items-center gap-2">
          <span className="font-medium">Connection Status:</span>
          <Chip
            color={connected ? 'success' : 'danger'}
            variant="flat"
          >
            {connected ? 'Connected' : 'Disconnected'}
          </Chip>
        </div>

        {connected && (
          <>
            <div>
              <span className="font-medium">Network:</span>
              <span className="ml-2">{ssid}</span>
            </div>
            <div>
              <span className="font-medium">IP Address:</span>
              <span className="ml-2 font-mono">{ip}</span>
            </div>
          </>
        )}
      </div>

      {connected && (
        <div className="space-y-3">
          <div className="flex items-center gap-2">
            <span className="font-medium">Signal Strength:</span>
            <Chip
              color={getSignalColor(rssi)}
              variant="flat"
              size="sm"
            >
              {rssi} dBm ({getSignalStrength(rssi)})
            </Chip>
          </div>
        </div>
      )}
    </div >
  );
};

export default DeviceConnectionStatus;
