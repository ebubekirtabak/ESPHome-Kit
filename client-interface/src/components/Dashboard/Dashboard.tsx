import React, { useState, useEffect } from 'react';
import { Card, CardBody, CardHeader, Chip, Button, Spinner } from '@heroui/react';

interface DeviceStatus {
  connected: boolean;
  ssid: string;
  ip: string;
  rssi: number;
}

const Dashboard: React.FC = () => {
  const [status, setStatus] = useState<DeviceStatus | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    fetchStatus();
    const interval = setInterval(fetchStatus, 30000);
    return () => clearInterval(interval);
  }, []);

  const fetchStatus = async () => {
    try {
      const response = await fetch('/api/status');
      const data = await response.json();
      setStatus(data);
    } catch (error) {
      console.error('Failed to fetch status:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const testConnection = async () => {
    try {
      const response = await fetch('/api/test');
      const data = await response.json();
      console.log('Test response:', data);
    } catch (error) {
      console.error('Test failed:', error);
    }
  };

  const getSignalStrength = (rssi: number): string => {
    if (rssi >= -50) return 'Excellent';
    if (rssi >= -60) return 'Good';
    if (rssi >= -70) return 'Fair';
    return 'Weak';
  };

  const getSignalColor = (rssi: number): "success" | "warning" | "danger" => {
    if (rssi >= -60) return 'success';
    if (rssi >= -70) return 'warning';
    return 'danger';
  };

  if (isLoading) {
    return (
      <div className="flex justify-center items-center min-h-[400px]">
        <Spinner size="lg" />
      </div>
    );
  }

  return (
    <div className="max-w-4xl mx-auto p-4 space-y-6">
      <div className="flex justify-between items-center">
        <h1 className="text-3xl font-bold">Dashboard</h1>
        <Button
          color="primary"
          variant="bordered"
          onPress={fetchStatus}
          size="sm"
        >
          Refresh
        </Button>
      </div>

      {/* Device Status */}
      <Card>
        <CardHeader>
          <h2 className="text-xl font-bold">Device Status</h2>
        </CardHeader>
        <CardBody>
          {status ? (
            <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
              <div className="space-y-3">
                <div className="flex items-center gap-2">
                  <span className="font-medium">Connection Status:</span>
                  <Chip
                    color={status.connected ? 'success' : 'danger'}
                    variant="flat"
                  >
                    {status.connected ? 'Connected' : 'Disconnected'}
                  </Chip>
                </div>

                {status.connected && (
                  <>
                    <div>
                      <span className="font-medium">Network:</span>
                      <span className="ml-2">{status.ssid}</span>
                    </div>
                    <div>
                      <span className="font-medium">IP Address:</span>
                      <span className="ml-2 font-mono">{status.ip}</span>
                    </div>
                  </>
                )}
              </div>

              {status.connected && (
                <div className="space-y-3">
                  <div className="flex items-center gap-2">
                    <span className="font-medium">Signal Strength:</span>
                    <Chip
                      color={getSignalColor(status.rssi)}
                      variant="flat"
                      size="sm"
                    >
                      {status.rssi} dBm ({getSignalStrength(status.rssi)})
                    </Chip>
                  </div>
                </div>
              )}
            </div>
          ) : (
            <div className="text-center py-8">
              <p>Unable to load device status</p>
            </div>
          )}
        </CardBody>
      </Card>

      {/* Quick Actions */}
      <Card>
        <CardHeader>
          <h2 className="text-xl font-bold">Quick Actions</h2>
        </CardHeader>
        <CardBody>
          <div className="flex flex-wrap gap-3">
            <Button
              color="primary"
              variant="flat"
              onPress={testConnection}
            >
              Test Connection
            </Button>
            <Button
              color="secondary"
              variant="flat"
              onPress={() => window.location.reload()}
            >
              Refresh Page
            </Button>
          </div>
        </CardBody>
      </Card>

      {/* System Information */}
      <Card>
        <CardHeader>
          <h2 className="text-xl font-bold">System Information</h2>
        </CardHeader>
        <CardBody>
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div>
              <span className="font-medium">Device:</span> NodeMCU (ESP8266)
            </div>
            <div>
              <span className="font-medium">Firmware:</span> NodeMCU Home Control
            </div>
            <div>
              <span className="font-medium">Last Updated:</span> {new Date().toLocaleString()}
            </div>
          </div>
        </CardBody>
      </Card>
    </div>
  );
};

export default Dashboard;
