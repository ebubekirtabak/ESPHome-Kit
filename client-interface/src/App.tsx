import { useState, useEffect } from 'react'
import {Button} from "@heroui/react";
import Connect from './components/Connect/connect';
import './App.css';

function App() {
  const [showConnect, setShowConnect] = useState(false)
  const [isAPMode, setIsAPMode] = useState(false)

  useEffect(() => {
    // Check if we're in AP mode by checking the current hostname/IP
    const checkAPMode = () => {
      const hostname = window.location.hostname;
      // AP mode typically uses 192.168.4.1 or the configured STA IP
      if (hostname.startsWith('192.168.4') || hostname === '192.168.4.1') {
        setIsAPMode(true);
        setShowConnect(true); // Auto-show WiFi setup in AP mode
      }
    };

    checkAPMode();
  }, []);

  return (
    <>
      <h1>NodeMCU Home Control</h1>
      {isAPMode && (
        <div className="bg-blue-100 border border-blue-400 text-blue-700 px-4 py-3 rounded mb-4">
          <strong>Setup Mode:</strong> Configure WiFi to connect your device to the internet.
        </div>
      )}
      <div className="flex gap-4 mb-4">
        <Button
          color="primary"
          variant="solid"
          onPress={() => setShowConnect(!showConnect)}
        >
          {showConnect ? 'Hide WiFi Setup' : 'WiFi Setup'}
        </Button>
      </div>
      {showConnect && <Connect />}
    </>
  )
}

export default App;
