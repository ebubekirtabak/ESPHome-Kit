import React from "react";
import { Card, CardBody, CardHeader } from "@heroui/react";


export type NetworkSettingsProps = {};

const NetworkSettings = ({ }: NetworkSettingsProps) => {
  return (
    <div className="py-4">
      <Card>
        <CardHeader>
          <h3 className="text-lg font-semibold">Advanced Network Configuration</h3>
        </CardHeader>
        <CardBody>
          <div className="space-y-4">
            <p className="text-gray-600">
              Advanced network settings like static IP, DNS, and gateway configuration
              are managed through the WiFi configuration file.
            </p>

            <div className="bg-gray-50 p-4 rounded-lg text-background">
              <h4 className="font-medium mb-2">Supported Configuration Fields:</h4>
              <ul className="space-y-1 text-sm">
                <li><code className="bg-gray-200 px-1 rounded">local_IP</code> - Static IP address</li>
                <li><code className="bg-gray-200 px-1 rounded">gateway</code> - Network gateway</li>
                <li><code className="bg-gray-200 px-1 rounded">subnet</code> - Subnet mask</li>
                <li><code className="bg-gray-200 px-1 rounded">primaryDNS</code> - Primary DNS server</li>
                <li><code className="bg-gray-200 px-1 rounded">secondaryDNS</code> - Secondary DNS server</li>
              </ul>
            </div>

            <div className="bg-blue-50 p-4 rounded-lg text-background">
              <h4 className="font-medium mb-2">Example Configuration:</h4>
              <pre className="text-sm bg-white p-2 rounded border overflow-x-auto">
                {`{
                    "ssid": "MyWiFi",
                    "password": "mypassword",
                    "local_IP": "192.168.1.100",
                    "gateway": "192.168.1.1",
                    "subnet": "255.255.255.0",
                    "primaryDNS": "8.8.8.8",
                    "secondaryDNS": "8.8.4.4"
                  }`}
              </pre>
            </div>
          </div>
        </CardBody>
      </Card>
    </div>
  );
};

export default NetworkSettings;
