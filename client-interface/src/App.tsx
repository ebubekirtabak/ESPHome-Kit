import { useState } from 'react'
import {Button} from "@heroui/react";
import Connect from './components/Connect/connect';
import './App.css';

function App() {
  const [showConnect, setShowConnect] = useState(false)

  return (
    <>
      <h1>NodeMCU Home Control</h1>
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
