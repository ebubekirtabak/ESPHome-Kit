import { useState, useEffect } from 'react'
import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom'
import Layout from './components/Layout/Layout'
import Dashboard from './components/Dashboard/Dashboard'
import Settings from './components/Settings/Settings'
import Connect from './components/Connect/connect'
import './App.css'

function App() {
  const [isAPMode, setIsAPMode] = useState(false)

  useEffect(() => {
    const checkAPMode = () => {
      const hostname = window.location.hostname;
      if (hostname.startsWith('192.168.4') || hostname === '192.168.4.1') {
        setIsAPMode(true);
      }
    };

    checkAPMode();
  }, []);

  if (isAPMode) {
    return (
      <div className="min-h-screen bg-gray-50 p-4">
        <div className="max-w-2xl mx-auto">
          <h1 className="text-3xl font-bold mb-4">NodeMCU WiFi Setup</h1>
          <div className="bg-blue-100 border border-blue-400 text-blue-700 px-4 py-3 rounded mb-6">
            <strong>Setup Mode:</strong> Configure WiFi to connect your device to the internet.
          </div>
          <Connect />
        </div>
      </div>
    );
  }

  return (
    <Router>
      <Routes>
        <Route path="/" element={<Layout />}>
          <Route index element={<Navigate to="/dashboard" replace />} />
          <Route path="dashboard" element={<Dashboard />} />
          <Route path="settings" element={<Settings />} />
        </Route>
      </Routes>
    </Router>
  )
}

export default App;
