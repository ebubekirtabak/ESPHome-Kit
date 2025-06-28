import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg';
import { Button } from '@heroui/button';
import Connect from './components/Connect/connect';
import './App.css'

function App() {
  const [count, setCount] = useState(0)
  const [showConnect, setShowConnect] = useState(false)

  return (
    <>
      <div>
        <a href="https://vite.dev" target="_blank">
          <img src={viteLogo} className="logo" alt="Vite logo" />
        </a>
        <a href="https://react.dev" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
      <h1>NodeMCU Home Control</h1>
      
      <div className="card">
        <div className="flex gap-4 mb-4">
          <Button 
            color="primary" 
            variant="solid"
            onPress={() => setShowConnect(!showConnect)}
          >
            {showConnect ? 'Hide WiFi Setup' : 'WiFi Setup'}
          </Button>
          
          <button onClick={() => setCount((count) => count + 1)}>
            count is {count}
          </button>
        </div>
        
        {showConnect && <Connect />}
        
        <p>
          Edit <code>src/App.tsx</code> and save to test HMR
        </p>
      </div>
      <p className="read-the-docs">
        Click on the Vite and React logos to learn more
      </p>
    </>
  )
}

export default App;
