import React from 'react';
import { Outlet, Link, useLocation } from 'react-router-dom';
import { Navbar, NavbarBrand, NavbarContent, NavbarItem, Button } from '@heroui/react';

const Layout: React.FC = () => {
  const location = useLocation();

  return (
    <div className="min-h-screen bg-gray-50">
      <Navbar isBordered>
        <NavbarBrand>
          <h1 className="font-bold text-inherit">NodeMCU Home Control</h1>
        </NavbarBrand>

        <NavbarContent className="hidden sm:flex gap-4" justify="center">
          <NavbarItem isActive={location.pathname === '/' || location.pathname === '/dashboard'}>
            <Link to="/dashboard">
              <Button
                variant={location.pathname === '/' || location.pathname === '/dashboard' ? 'solid' : 'light'}
                color="primary"
                size="sm"
              >
                Dashboard
              </Button>
            </Link>
          </NavbarItem>

          <NavbarItem isActive={location.pathname === '/settings'}>
            <Link to="/settings">
              <Button
                variant={location.pathname === '/settings' ? 'solid' : 'light'}
                color="primary"
                size="sm"
              >
                Settings
              </Button>
            </Link>
          </NavbarItem>
        </NavbarContent>

        <NavbarContent justify="end">
          <NavbarItem>
            <Button
              as={Link}
              to="/settings"
              color="primary"
              variant="flat"
              size="sm"
            >
              WiFi Setup
            </Button>
          </NavbarItem>
        </NavbarContent>
      </Navbar>

      <main className="container mx-auto px-4 py-6">
        <Outlet />
      </main>
    </div>
  );
};

export default Layout;
