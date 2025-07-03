import { Card, CardBody, CardHeader } from "@heroui/react";

const DeviceSettings = () => {

  return (
    <div className="py-4">
      <Card>
        <CardHeader>
          <h3 className="text-lg font-semibold">Device Information</h3>
        </CardHeader>
        <CardBody>
          <div className="space-y-4">
            <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
              <div>
                <span className="font-medium">Device Type:</span>
                <p className="text-gray-600">NodeMCU (ESP8266)</p>
              </div>
              <div>
                <span className="font-medium">Firmware:</span>
                <p className="text-gray-600">NodeMCU Home Control</p>
              </div>
              <div>
                <span className="font-medium">Features:</span>
                <ul className="text-gray-600 text-sm">
                  <li>• WiFi Configuration Portal</li>
                  <li>• Captive Portal Support</li>
                  <li>• OTA Updates</li>
                  <li>• WebSocket Communication</li>
                </ul>
              </div>
              <div>
                <span className="font-medium">Access Points:</span>
                <ul className="text-gray-600 text-sm">
                  <li>• REST API Endpoints</li>
                  <li>• Real-time Status Updates</li>
                  <li>• Network Scanning</li>
                  <li>• Configuration Management</li>
                </ul>
              </div>
            </div>
          </div>
        </CardBody>
      </Card>
    </div>
  );
};

export default DeviceSettings;
