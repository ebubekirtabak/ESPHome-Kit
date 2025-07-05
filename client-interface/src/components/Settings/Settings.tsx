import React, { useState } from 'react';
import { Card, CardBody, CardHeader, Tabs, Tab } from '@heroui/react';
import Connect from '@components/Connect/connect';
import DeviceSettings from '@components/DeviceSettings/DeviceSettings.component';
import NetworkSettings from '@components/NetworkSettings/NetworkSettings.component';

const Settings: React.FC = () => {
  const [selectedTab, setSelectedTab] = useState('wifi');

  return (
    <div className="max-w-4xl mx-auto p-4 space-y-6">
      <div className="flex justify-between items-center">
        <h1 className="text-3xl font-bold text-background">Settings</h1>
      </div>

      <Card>
        <CardBody>
          <Tabs
            selectedKey={selectedTab}
            onSelectionChange={(key) => setSelectedTab(key as string)}
            variant="underlined"
            classNames={{
              tabList: "gap-6 w-full relative rounded-none p-0 border-b border-divider",
              cursor: "w-full bg-primary",
              tab: "max-w-fit px-0 h-12",
              tabContent: "group-data-[selected=true]:text-primary"
            }}
          >
            <Tab
              key="wifi"
              title={
                <div className="flex items-center space-x-2">
                  <span>WiFi Configuration</span>
                </div>
              }
            >
              <div className="py-4">
                <Connect />
              </div>
            </Tab>

            <Tab
              key="network"
              title={
                <div className="flex items-center space-x-2">
                  <span>Network Settings</span>
                </div>
              }
            >
              <NetworkSettings />
            </Tab>

            <Tab
              key="device"
              title={
                <div className="flex items-center space-x-2">
                  <span>Device Settings</span>
                </div>
              }
            >
              <DeviceSettings />
            </Tab>
          </Tabs>
        </CardBody>
      </Card>
    </div>
  );
};

export default Settings;
