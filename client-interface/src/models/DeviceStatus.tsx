type DeviceStatus = {
  connected: boolean;
  ssid: string;
  ip: string;
  rssi: number;
  firmwareVersion: string;
  buildNumber: number;
  buildDate: string;
  buildTime: string;
  freeHeap: number;
  uptime: number;
  chipId: string;
};

export default DeviceStatus;
