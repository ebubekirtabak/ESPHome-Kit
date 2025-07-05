import React, { useState, useEffect } from 'react';
import { Card, CardBody, CardHeader, Chip, Button, Spinner } from '@heroui/react';
import DeviceStatus from '@/models/DeviceStatus';
import DeviceConnectionStatus from './components/DeviceConnectionStatus/DeviceConnectionStatus.component';
import DeviceInformations from './components/DeviceInformations/DeviceInformations.component';

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
        <h1 className="text-3xl font-bold text-background">Dashboard</h1>
        <Button
          color="primary"
          variant="bordered"
          onPress={fetchStatus}
          size="sm"
        >
          Refresh
        </Button>
      </div>
      <Card>
        <CardHeader>
          <h2 className="text-xl font-bold">Device Status</h2>
        </CardHeader>
        <CardBody>
          {status ? (
            <DeviceConnectionStatus deviceStatus={status} />
          ) : (
            <div className="text-center py-8">
              <p>Unable to load device status</p>
            </div>
          )}
        </CardBody>
      </Card>
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
      <Card>
        <CardHeader>
          <h2 className="text-xl font-bold">System Information</h2>
        </CardHeader>
        <CardBody>
          {status ? (
            <DeviceInformations deviceStatus={status} />
          ) : (
            <div className="text-center py-4">
              <p>Loading system information...</p>
            </div>
          )}
        </CardBody>
      </Card>
    </div>
  );
};

export default Dashboard;
