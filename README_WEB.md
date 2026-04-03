# CS2 Skin Changer - Web Interface

A modern, lightweight web-based skin changer for Counter-Strike 2 with beautiful animations and real-time updates.

## Features

✨ **Modern UI**
- Beautiful dark theme with gradient accents
- Smooth animations and transitions
- Responsive design for all screen sizes
- Real-time status updates

🎮 **Full Functionality**
- Knife model selection (15+ knife types)
- Glove selection (8 glove types)
- Paint kit customization
- Wear value control
- Seed value adjustment

🔌 **Real-Time Updates**
- WebSocket connection for instant status
- Live skin application tracking
- Connection status indicator
- Statistics dashboard

⚡ **Lightweight & Fast**
- Node.js Express backend
- Minimal dependencies
- Fast response times
- Efficient memory usage

## System Requirements

- Node.js 16.x or higher
- npm (included with Node.js)
- Windows/Linux/macOS

## Installation

### 1. Install Dependencies

```bash
cd C:\Users\qveezzx\Documents\Changer
npm install express ws cors dotenv
```

### 2. Start the Server

```bash
# Development mode (with auto-reload)
npm install --save-dev nodemon
npx nodemon web-server.js

# Or production mode
node web-server.js
```

### 3. Open in Browser

Navigate to: **http://localhost:3000**

## Quick Start

1. **Select a Knife**
   - Choose from 15+ knife types in the dropdown
   - Preview updates in real-time

2. **Select Gloves** (Optional)
   - Choose from 8 glove types or "No Gloves"
   - Preview shows your selection

3. **Customize Paint**
   - Adjust paint kit index
   - Set wear value (0.0 = Factory New, 1.0 = Battle-Scarred)
   - Configure seed if needed

4. **Apply**
   - Click "APPLY SKINS" button
   - Watch the stats update
   - Skins applied instantly!

5. **Reset** (if needed)
   - Click "RESET" to restore default skins
   - All settings return to defaults

## API Documentation

### REST Endpoints

#### Get Status
```bash
GET /api/status
```
Returns current skinchanger status and configuration.

Response:
```json
{
  "status": "active",
  "config": {
    "knife": { "defIndex": 500, "name": "Bayonet" },
    "gloves": { "defIndex": 5027, "name": "Bloodhound Gloves" },
    "paintKit": 1234,
    "wear": 0.15,
    "seed": 42,
    "isApplied": true,
    "lastUpdate": "2024-01-15T10:30:45.123Z"
  },
  "timestamp": "2024-01-15T10:30:45.123Z"
}
```

#### Get Available Knives
```bash
GET /api/knives
```

Response:
```json
[
  { "id": 0, "name": "Default Knife" },
  { "id": 500, "name": "Bayonet" },
  { "id": 505, "name": "Bowie Knife" },
  ...
]
```

#### Get Available Gloves
```bash
GET /api/gloves
```

Response:
```json
[
  { "id": 0, "name": "No Gloves" },
  { "id": 5027, "name": "Bloodhound Gloves" },
  ...
]
```

#### Apply Skins
```bash
POST /api/skin/apply

Content-Type: application/json
{
  "knife": { "defIndex": 500, "name": "Bayonet" },
  "gloves": { "defIndex": 5027, "name": "Bloodhound Gloves" },
  "paintKit": 1234,
  "wear": 0.15,
  "seed": 42
}
```

Response:
```json
{
  "success": true,
  "config": { ... }
}
```

#### Reset Skins
```bash
POST /api/skin/reset
```

Response:
```json
{
  "success": true,
  "config": { ... }
}
```

### WebSocket Events

#### Connection
```javascript
ws = new WebSocket('ws://localhost:3000');
```

#### Receive Initial State
```json
{
  "type": "initial-state",
  "config": { ... }
}
```

#### Receive Skin Applied Event
```json
{
  "type": "skin-applied",
  "config": { ... }
}
```

#### Receive Skin Reset Event
```json
{
  "type": "skin-reset",
  "config": { ... }
}
```

## Configuration

The skinchanger saves its configuration to `skinconfig.json`:

```json
{
  "knife": {
    "defIndex": 500,
    "name": "Bayonet"
  },
  "gloves": {
    "defIndex": 5027,
    "name": "Bloodhound Gloves"
  },
  "paintKit": 1234,
  "wear": 0.15,
  "seed": 42,
  "isApplied": true,
  "lastUpdate": "2024-01-15T10:30:45.123Z"
}
```

Configuration persists between sessions and is automatically loaded on startup.

## Architecture

### Backend (Node.js + Express)
- REST API for skin management
- WebSocket server for real-time updates
- JSON-based configuration storage
- CORS enabled for cross-origin requests

### Frontend (Vanilla JavaScript)
- Single-page application (SPA)
- Dynamic UI updates
- WebSocket client
- Responsive design with CSS Grid

### File Structure
```
├── web-server.js          # Express server + WebSocket
├── index.html             # Web UI (complete + CSS + JS)
├── skinconfig.json        # Configuration (auto-created)
├── package.json           # Dependencies
└── README_WEB.md          # This file
```

## Advanced Usage

### Custom Knife Definitions

Edit the knives array in `web-server.js`:

```javascript
const knives = [
  { id: 0, name: 'Default Knife' },
  { id: 500, name: 'Bayonet' },
  // Add more knife types here
];
```

### Custom Glove Definitions

Edit the gloves array in `web-server.js`:

```javascript
const gloves = [
  { id: 0, name: 'No Gloves' },
  { id: 5027, name: 'Bloodhound Gloves' },
  // Add more glove types here
];
```

### Port Configuration

To use a different port, modify `web-server.js`:

```javascript
const PORT = 3001;  // Change from 3000 to 3001
```

Then update the frontend:

```javascript
const API_URL = 'http://localhost:3001';
const WS_URL = 'ws://localhost:3001';
```

## Troubleshooting

### Port Already in Use

```bash
# Find process using port 3000
netstat -ano | findstr :3000

# Kill process (replace PID with actual process ID)
taskkill /PID <PID> /F
```

### WebSocket Connection Failed

1. Ensure server is running
2. Check firewall settings
3. Verify port 3000 is accessible
4. Check browser console for errors

### Skins Not Applying

1. Verify CS2 is running
2. Check that the C++ backend is connected
3. Review console for error messages
4. Ensure proper offsets are configured

### Configuration Won't Save

1. Check file permissions
2. Verify write access to directory
3. Ensure JSON is valid format
4. Check disk space

## Development

### Enable Debugging

```bash
# Run with debug output
DEBUG=* node web-server.js
```

### CORS Issues

If using from a different origin, CORS is already enabled. For additional origins:

```javascript
app.use(cors({
  origin: ['http://localhost:3000', 'http://yourdomainhere.com'],
  credentials: true
}));
```

### Performance Optimization

The current implementation uses:
- In-memory client tracking for WebSocket
- Efficient JSON serialization
- Minimal database overhead
- Optimized CSS with GPU acceleration

## Security Considerations

⚠️ **WARNING**: This tool is for LOCAL USE ONLY

Current security measures:
- CORS restricted to localhost
- No authentication (assuming local network)
- Input validation on all endpoints
- XSS protection through proper escaping

For remote deployment, add:
- Authentication/authorization
- Rate limiting
- HTTPS/WSS encryption
- Input sanitization
- CORS whitelist

## Future Enhancements

Planned features:
- [ ] User profiles and presets
- [ ] Skin marketplace integration
- [ ] Keyboard shortcuts
- [ ] Hotkeys for quick apply
- [ ] Rarity-based filtering
- [ ] Float value presets (Factory, Minimal Wear, etc.)
- [ ] Team statistics
- [ ] Custom themes
- [ ] Mobile app
- [ ] Multi-language support

## Integration with C++ Backend

The web interface communicates with the C++ skinchanger via:
1. REST API (for commands)
2. WebSocket (for real-time status)
3. JSON config files (for persistence)

The C++ backend must be running and listening for IPC communication.

## License

Part of the CS2 External Skinchanger project.

## Support

For issues or questions:
1. Check the troubleshooting section
2. Review console logs
3. Verify C++ backend is running
4. Check GitHub issues

---

**Version**: 1.0.0  
**Last Updated**: 2024-01-15  
**Status**: ✅ Production Ready
