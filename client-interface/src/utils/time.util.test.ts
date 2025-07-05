import { describe, it, expect } from 'vitest';
import { formatUptime } from './time.util';

describe('formatUptime', () => {
  it('should format seconds correctly', () => {
    expect(formatUptime(1000)).toBe('1s');
    expect(formatUptime(30000)).toBe('30s');
    expect(formatUptime(59000)).toBe('59s');
  });

  it('should format minutes and seconds correctly', () => {
    expect(formatUptime(60000)).toBe('1m 0s');
    expect(formatUptime(90000)).toBe('1m 30s');
    expect(formatUptime(3570000)).toBe('59m 30s');
  });

  it('should format hours and minutes correctly', () => {
    expect(formatUptime(3600000)).toBe('1h 0m');
    expect(formatUptime(5400000)).toBe('1h 30m');
    expect(formatUptime(86370000)).toBe('23h 59m');
  });

  it('should format days, hours, and minutes correctly', () => {
    expect(formatUptime(86400000)).toBe('1d 0h 0m');
    expect(formatUptime(90000000)).toBe('1d 1h 0m');
    expect(formatUptime(93600000)).toBe('1d 2h 0m');
    expect(formatUptime(266400000)).toBe('3d 2h 0m');
  });

  it('should handle edge cases correctly', () => {
    expect(formatUptime(0)).toBe('0s');
    expect(formatUptime(500)).toBe('0s');
    expect(formatUptime(999)).toBe('0s');
  });

  it('should handle large values correctly', () => {
    const weekInMs = 7 * 24 * 60 * 60 * 1000;
    expect(formatUptime(weekInMs)).toBe('7d 0h 0m');

    const monthInMs = 30 * 24 * 60 * 60 * 1000;
    expect(formatUptime(monthInMs)).toBe('30d 0h 0m');
  });

  it('should handle complex time combinations', () => {
    // 2 days, 14 hours, 35 minutes, 42 seconds
    const complexTime = (2 * 24 * 60 * 60 * 1000) + (14 * 60 * 60 * 1000) + (35 * 60 * 1000) + (42 * 1000);
    expect(formatUptime(complexTime)).toBe('2d 14h 35m');

    // 1 hour, 1 minute, 1 second
    const oneEach = (1 * 60 * 60 * 1000) + (1 * 60 * 1000) + (1 * 1000);
    expect(formatUptime(oneEach)).toBe('1h 1m');
  });

  it('should handle negative values gracefully', () => {
    // While negative uptime doesn't make sense in practice,
    // the function should handle it gracefully
    expect(formatUptime(-1000)).toBe('-1s');
    expect(formatUptime(-60000)).toBe('-60s');
  });

  it('should handle fractional milliseconds', () => {
    expect(formatUptime(1500.7)).toBe('1s');
    expect(formatUptime(59999.9)).toBe('59s');
  });
});
