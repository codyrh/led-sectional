# Arduino Cloud Usage Examples

This document provides practical examples of how to use the Arduino Cloud features with your LED sectional.

## Dashboard Setup Examples

### Basic Status Dashboard
Create a dashboard with these widgets:
- **Gauge**: `airports_processed` (0-50 range)
- **LED**: `cloud_connected` (green when connected)
- **Text**: `device_status` (shows current status)
- **Text**: `log_message` (shows latest activity)

### Weather Alerts Dashboard
Add these widgets for weather monitoring:
- **Gauge**: `lightning_alerts` (0-10 range, red zone above 3)
- **Gauge**: `high_wind_alerts` (0-10 range, orange zone above 2)
- **Gauge**: `very_high_wind_alerts` (0-5 range, red zone above 1)
- **Chart**: Historical trend of all alert types

### Control Dashboard
Add control widgets:
- **Switch**: `enable_lightning_alerts`
- **Switch**: `enable_wind_alerts`
- **Slider**: `brightness_override` (-1 to 255)
- **Button**: `force_update`
- **Text Input**: `config_message`

### Multiple Board Dashboard Examples

#### Master Overview Dashboard (for multiple boards)
Create a dashboard that shows all your boards:
- **Map Widget**: Show multiple board locations
- **Status Table**: 
  - Board Name | Status | Last Update | Alerts
  - Home | Active | 2 min ago | 0
  - Hangar | Active | 1 min ago | 2 lightning
  - Regional | Offline | 1 hour ago | -
- **Combined Chart**: Weather alerts across all locations
- **Quick Actions**: Force update all, disable all alerts

#### Location-Specific Dashboards
For each board location:
- **Board Identifier**: "LED Sectional - Home" header
- **Local Weather Focus**: Airports relevant to that location
- **Location-Specific Controls**: Brightness for that room/environment
- **Local Alerts**: Weather affecting that specific area

## Common Use Cases

### 1. Remote Monitoring During Travel
**Scenario**: You're away from home and want to check if your LED sectional is working.

**Dashboard View**:
- Check `cloud_connected` is true
- Verify `airports_processed` > 0
- Review `last_update` timestamp
- Check `device_status` shows "Active"

### 1a. Multi-Location Monitoring
**Scenario**: You have boards at home and hangar, want to check both while traveling.

**Master Dashboard Approach**:
1. Create overview dashboard showing both locations
2. Use color-coded status indicators:
   - Green: Both locations active
   - Yellow: One location has issues
   - Red: Multiple locations offline
3. Quick status summary: "2/2 boards active, 3 total alerts"

**Individual Dashboard Approach**:
1. Switch between "Home" and "Hangar" dashboards
2. Compare weather conditions between locations
3. Adjust settings independently for each location

### 2. Adjusting for Room Lighting
**Scenario**: The LEDs are too bright/dim for current room conditions.

**Control Actions**:
1. Set `brightness_override` to desired level (0-255)
2. Or set to -1 to return to automatic control
3. Check `log_message` for confirmation

### 3. Disabling Alerts During Sleep
**Scenario**: You want to disable blinking alerts at night.

**Control Actions**:
1. Set `enable_lightning_alerts` to false
2. Set `enable_wind_alerts` to false  
3. Re-enable in the morning

### 4. Force Update After Weather Change
**Scenario**: You see severe weather developing and want immediate update.

**Control Actions**:
1. Set `force_update` to true
2. Watch `log_message` for "Force update requested"
3. Check alert counts increase if weather detected

### 5. Troubleshooting Connection Issues
**Scenario**: Device seems unresponsive or showing old data.

**Diagnostic Steps**:
1. Check `cloud_connected` status
2. Review `last_update` timestamp
3. Send config command: "restart"
4. If WiFi issues, send: "reset_wifi"

## Mobile App Integration

### Using Arduino IoT Remote App
1. Download "Arduino IoT Remote" from app store
2. Log in with your Arduino account
3. Your Thing will appear automatically
4. Create custom mobile dashboard
5. Get push notifications for alerts

### Custom Mobile Notifications
Set up webhooks in Arduino Cloud:
1. Go to Integrations in your Thing
2. Add webhook for property changes
3. Configure to send to IFTTT, Zapier, etc.
4. Create mobile notifications for severe weather

## API Usage Examples

### Get Current Status (REST API)
```bash
curl -H "Authorization: Bearer YOUR_TOKEN" \
     https://api2.arduino.cc/iot/v2/things/YOUR_THING_ID/properties
```

### Update Settings Programmatically
```bash
curl -X PUT \
     -H "Authorization: Bearer YOUR_TOKEN" \
     -H "Content-Type: application/json" \
     -d '{"last_value": false}' \
     https://api2.arduino.cc/iot/v2/things/YOUR_THING_ID/properties/enable_wind_alerts
```

## Automation Examples

### Home Assistant Integration
```yaml
# configuration.yaml
sensor:
  - platform: rest
    name: led_sectional_status
    resource: https://api2.arduino.cc/iot/v2/things/YOUR_THING_ID/properties/device_status
    headers:
      Authorization: Bearer YOUR_TOKEN
    value_template: '{{ value_json.last_value }}'

automation:
  - alias: "LED Sectional Weather Alert"
    trigger:
      platform: state
      entity_id: sensor.led_sectional_lightning_alerts
    condition:
      condition: numeric_state
      entity_id: sensor.led_sectional_lightning_alerts
      above: 2
    action:
      service: notify.mobile_app
      data:
        message: "Severe weather detected on LED sectional!"
```

### IFTTT Integration
1. Create IFTTT account
2. Connect Arduino IoT service
3. Create applet: "If lightning_alerts > 3, then send notification"
4. Or: "If device goes offline, then send email"

## Data Analysis Examples

### Export Historical Data
1. Go to your Thing dashboard
2. Click on any chart widget
3. Click "Export" to download CSV
4. Analyze in Excel, Google Sheets, etc.

### Python Analysis Script
```python
import requests
import pandas as pd
import matplotlib.pyplot as plt

# Fetch data from Arduino Cloud API
headers = {'Authorization': 'Bearer YOUR_TOKEN'}
response = requests.get(
    'https://api2.arduino.cc/iot/v2/things/YOUR_THING_ID/properties/lightning_alerts/values',
    headers=headers
)

# Convert to DataFrame and plot
data = pd.DataFrame(response.json())
data['timestamp'] = pd.to_datetime(data['timestamp'])
data.plot(x='timestamp', y='value', title='Lightning Alerts Over Time')
plt.show()
```

## Best Practices

### Dashboard Organization
- Create separate dashboards for different purposes (monitoring vs control)
- Use color coding (green=good, yellow=warning, red=alert)
- Group related widgets together
- Add clear labels and units

### Alert Management
- Set reasonable thresholds to avoid alert fatigue
- Use different notification methods for different severity levels
- Test alerts regularly to ensure they work
- Document your alert procedures

### Security
- Regularly rotate API tokens
- Use read-only tokens where possible
- Monitor access logs in Arduino Cloud
- Keep firmware updated for security patches

### Performance
- Don't update cloud properties too frequently (respect rate limits)
- Use appropriate data types for properties
- Clean up old data periodically
- Monitor your Arduino Cloud usage quotas

### Multiple Board Management
- **Consistent Naming**: Use clear, consistent naming across all boards
- **Staged Updates**: Test firmware updates on one board before deploying to all
- **Centralized Monitoring**: Create master dashboards that show all board status
- **Documentation**: Keep a spreadsheet/document tracking each board's configuration
- **Backup Configurations**: Save each board's airport list and settings
- **Network Isolation**: Consider separate WiFi networks for different locations
- **Update Scheduling**: Stagger automatic updates to avoid all boards updating simultaneously

## Troubleshooting Common Issues

### "Property not updating"
- Check network connectivity
- Verify property names match exactly
- Check data types are correct
- Look for rate limiting messages

### "OTA update failed"
- Ensure stable power supply
- Check WiFi signal strength
- Verify enough flash memory available
- Try uploading smaller firmware

### "Webhook not firing"
- Test webhook URL independently
- Check webhook configuration
- Verify authentication tokens
- Look at Arduino Cloud logs

### "Dashboard showing old data"
- Refresh browser/app
- Check device connection status
- Verify property sync settings
- Clear browser cache if needed

### Multiple Board Specific Issues

### "One board working, others not"
- Check each board's individual Thing status
- Verify each board has correct credentials file
- Ensure device names are unique across all boards
- Check WiFi connectivity at each location
- Review firmware versions - ensure consistency

### "Properties mixing between boards"
- This indicates single Thing setup with naming conflicts
- Ensure property names include board identifier
- Consider switching to separate Things approach
- Check device associations in Arduino Cloud

### "Can't tell which board is which"
- Add board identifier properties to each Thing
- Use descriptive device names in Arduino Cloud
- Include location information in log messages
- Create clear dashboard labels and sections