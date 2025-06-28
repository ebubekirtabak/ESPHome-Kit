import React, { useState, useEffect } from 'react';
import { Button } from '@heroui/button';
import { Card, CardBody, CardHeader } from '@heroui/card';
import { Select, SelectItem } from '@heroui/select';
import { Input } from '@heroui/input';
import { Spinner } from '@heroui/spinner';
import { Chip } from '@heroui/chip';
import './connect.css';

interface WiFiNetwork {
  ssid: string;
  rssi: number;
  secure: boolean;
}

interface WiFiStatus {
  connected: boolean;
  ssid: string;
  ip: string;
  rssi: number;
}

const Connect: React.FC = () => {
  const [networks, setNetworks] = useState<WiFiNetwork[]>([]);
  const [selectedNetwork, setSelectedNetwork] = useState<string>('');
  const [password, setPassword] = useState<string>('');
  const [isLoading, setIsLoading] = useState<boolean>(false);
  const [isScanning, setIsScanning] = useState<boolean>(false);
  const [status, setStatus] = useState<WiFiStatus | null>(null);
  const [message, setMessage] = useState<string>('');
  const [messageType, setMessageType] = useState<'success' | 'error' | ''>('');

  useEffect(() => {
    fetchStatus();
    scanNetworks();
  }, []);

  const fetchStatus = async () => {
    try {
      const response = await fetch('/api/status');
      const data = await response.json();
      setStatus(data);
    } catch (error) {
      console.error('Failed to fetch WiFi status:', error);
    }
  };  const scanNetworks = async () => {
    setIsScanning(true);
    try {
      // Try the advanced scan first
      let response = await fetch('/api/scan');
      let data = await response.json();

      // If advanced scan fails, try simple scan
      if (!response.ok || data.error) {
        console.log('Advanced scan failed, trying simple scan...');
        response = await fetch('/api/scan-simple');
        data = await response.json();
      }

      // Handle different response formats
      if (Array.isArray(data)) {
        setNetworks(data);
        if (data.length === 0) {
          showMessage('No WiFi networks found', 'error');
        }
      } else if (data.networks && Array.isArray(data.networks)) {
        setNetworks(data.networks);
      } else if (data.message) {
        showMessage(data.message, 'error');
        setNetworks([]);
      } else if (data.error) {
        showMessage(data.error, 'error');
        setNetworks([]);
      } else {
        showMessage('Unexpected response format', 'error');
        setNetworks([]);
      }
    } catch (error) {
      console.error('Failed to scan networks:', error);
      showMessage('Failed to scan WiFi networks. Check device connection.', 'error');
      setNetworks([]);
    } finally {
      setIsScanning(false);
    }
  };

  const handleSave = async () => {
    if (!selectedNetwork || !password) {
      showMessage('Please select a network and enter password', 'error');
      return;
    }

    setIsLoading(true);
    try {
      const response = await fetch('/api/wifi', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          ssid: selectedNetwork,
          password: password,
        }),
      });

      const result = await response.json();

      if (response.ok) {
        showMessage('WiFi credentials saved successfully! Device will restart...', 'success');
        setPassword('');
        setTimeout(() => {
          fetchStatus();
        }, 5000);
      } else {
        showMessage(result.message || 'Failed to save WiFi credentials', 'error');
      }
    } catch (error) {
      console.error('Error saving WiFi credentials:', error);
      showMessage('Failed to save WiFi credentials', 'error');
    } finally {
      setIsLoading(false);
    }
  };

  const testConnection = async () => {
    try {
      const response = await fetch('/api/test');
      const data = await response.json();
      console.log('Test response:', data);
      showMessage('Connection test successful', 'success');
    } catch (error) {
      console.error('Test failed:', error);
      showMessage('Connection test failed', 'error');
    }
  };

  const showMessage = (text: string, type: 'success' | 'error') => {
    setMessage(text);
    setMessageType(type);
    setTimeout(() => {
      setMessage('');
      setMessageType('');
    }, 5000);
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

  return (
    <div className="max-w-2xl mx-auto p-4 space-y-6">
      {/* Current Status */}
      <Card>
        <CardHeader>
          <h2 className="text-xl font-bold">WiFi Status</h2>
        </CardHeader>
        <CardBody>
          {status ? (
            <div className="space-y-2">
              <div className="flex items-center gap-2">
                <span>Status:</span>
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
                    <span className="font-medium">Network:</span> {status.ssid}
                  </div>
                  <div>
                    <span className="font-medium">IP Address:</span> {status.ip}
                  </div>
                  <div className="flex items-center gap-2">
                    <span className="font-medium">Signal:</span>
                    <Chip
                      color={getSignalColor(status.rssi)}
                      variant="flat"
                      size="sm"
                    >
                      {status.rssi} dBm ({getSignalStrength(status.rssi)})
                    </Chip>
                  </div>
                </>
              )}
            </div>
          ) : (
            <Spinner size="sm" />
          )}
        </CardBody>
      </Card>

      {/* WiFi Configuration */}
      <Card>
        <CardHeader className="flex justify-between items-center">
          <h2 className="text-xl font-bold">Connect to WiFi</h2>
          <div className="flex gap-2">
            <Button
              size="sm"
              variant="flat"
              onPress={testConnection}
            >
              Test
            </Button>
            <Button
              size="sm"
              variant="bordered"
              onPress={scanNetworks}
              isLoading={isScanning}
              disabled={isScanning}
            >
              {isScanning ? 'Scanning...' : 'Refresh'}
            </Button>
          </div>
        </CardHeader>
        <CardBody className="space-y-4">
          {/* Network Selection */}
          <div>
            <Select
              label="Select WiFi Network"
              placeholder="Choose a network"
              selectedKeys={selectedNetwork ? [selectedNetwork] : []}
              onSelectionChange={(keys: any) => {
                const selected = Array.from(keys)[0] as string;
                setSelectedNetwork(selected);
              }}
              isLoading={isScanning}
            >
              {networks.map((network) => (
                <SelectItem key={network.ssid}>
                  <div className="flex justify-between items-center w-full">
                    <div className="flex items-center gap-2">
                      <span>{network.ssid}</span>
                      {network.secure && (
                        <span className="text-xs text-gray-500">🔒</span>
                      )}
                    </div>
                    <Chip
                      color={getSignalColor(network.rssi)}
                      variant="flat"
                      size="sm"
                    >
                      {getSignalStrength(network.rssi)}
                    </Chip>
                  </div>
                </SelectItem>
              ))}
            </Select>
          </div>

          {/* Password Input */}
          <div>
            <Input
              type="password"
              label="Password"
              placeholder="Enter WiFi password"
              value={password}
              onChange={(e: any) => setPassword(e.target.value)}
              disabled={!selectedNetwork}
            />
          </div>

          {/* Save Button */}
          <Button
            color="primary"
            onPress={handleSave}
            isLoading={isLoading}
            disabled={!selectedNetwork || !password || isLoading}
            className="w-full"
          >
            {isLoading ? 'Saving...' : 'Save WiFi Configuration'}
          </Button>

          {/* Message */}
          {message && (
            <div className={`p-3 rounded-lg ${
              messageType === 'success'
                ? 'bg-green-100 text-green-800 border border-green-200'
                : 'bg-red-100 text-red-800 border border-red-200'
            }`}>
              {message}
            </div>
          )}
        </CardBody>
      </Card>

      {/* Network List */}
      {networks.length > 0 && (
        <Card>
          <CardHeader>
            <h3 className="text-lg font-semibold">Available Networks ({networks.length})</h3>
          </CardHeader>
          <CardBody>
            <div className="space-y-2">
              {networks.map((network, index) => (
                <div
                  key={`${network.ssid}-${index}`}
                  className="flex justify-between items-center p-2 rounded border hover:bg-gray-50"
                >
                  <div className="flex items-center gap-2">
                    <span className="font-medium">{network.ssid}</span>
                    {network.secure && (
                      <span className="text-xs text-gray-500">🔒</span>
                    )}
                  </div>
                  <div className="flex items-center gap-2">
                    <span className="text-sm text-gray-600">
                      {network.rssi} dBm
                    </span>
                    <Chip
                      color={getSignalColor(network.rssi)}
                      variant="flat"
                      size="sm"
                    >
                      {getSignalStrength(network.rssi)}
                    </Chip>
                  </div>
                </div>
              ))}
            </div>
          </CardBody>
        </Card>
      )}
    </div>
  );
};

export default Connect;
