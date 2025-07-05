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
  relay1State?: boolean;
  relay2State?: boolean;
};

export default DeviceStatus;
