import React from 'react';
import DeviceStatus from '@models/DeviceStatus';
import { formatUptime } from '@utils/time.util';
import { Chip } from '@heroui/react';
import { formatBytes } from '@utils/byte.util';

export type DeviceInformationsProps = {
  deviceStatus: DeviceStatus;
}

const DeviceInformations: React.FC<DeviceInformationsProps> = ({ deviceStatus }) => {
  const { chipId, freeHeap, uptime, firmwareVersion, buildNumber, buildTime, buildDate } = deviceStatus;
  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
      <div className="space-y-2">
        <h3 className="font-semibold text-sm text-gray-600 uppercase tracking-wide">Hardware</h3>
        <div>
          <span className="font-medium">Device:</span> NodeMCU (ESP8266)
        </div>
        <div>
          <span className="font-medium">Chip ID:</span>
          <span className="ml-2 font-mono text-sm">{chipId}</span>
        </div>
        <div>
          <span className="font-medium">Free Memory:</span>
          <span className="ml-2">{formatBytes(freeHeap)}</span>
        </div>
        <div>
          <span className="font-medium">Uptime:</span>
          <span className="ml-2">{formatUptime(uptime)}</span>
        </div>
      </div>

      <div className="space-y-2">
        <h3 className="font-semibold text-sm text-gray-600 uppercase tracking-wide">Firmware</h3>
        <div>
          <span className="font-medium">Version:</span>
          <span className="ml-2">{firmwareVersion}</span>
        </div>
        <div className="flex items-center gap-2">
          <span className="font-medium">Build:</span>
          <Chip
            size="sm"
            variant="flat"
            color="primary"
          >
            #{buildNumber}
          </Chip>
        </div>
        <div>
          <span className="font-medium">Build Date:</span>
          <span className="ml-2 text-sm">{buildDate}</span>
        </div>
        <div>
          <span className="font-medium">Build Time:</span>
          <span className="ml-2 text-sm">{buildTime}</span>
        </div>
      </div>

      <div className="space-y-2">
        <h3 className="font-semibold text-sm text-gray-600 uppercase tracking-wide">Status</h3>
        <div>
          <span className="font-medium">Last Updated:</span>
          <span className="ml-2 text-sm">{new Date().toLocaleString()}</span>
        </div>
        <div className="flex items-center gap-2">
          <span className="font-medium">Status:</span>
          <Chip
            size="sm"
            color="success"
            variant="flat"
          >
            Online
          </Chip>
        </div>
      </div>
    </div>
  );
};

export default DeviceInformations;
