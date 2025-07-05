/**
 * Utility functions for WiFi signal strength processing
 */

export const getSignalStrength = (rssi: number): string => {
  if (rssi >= -50) return 'Excellent';
  if (rssi >= -60) return 'Good';
  if (rssi >= -70) return 'Fair';
  return 'Weak';
};

export const getSignalColor = (rssi: number): "success" | "warning" | "danger" => {
  if (rssi >= -60) return 'success';
  if (rssi >= -70) return 'warning';
  return 'danger';
};
