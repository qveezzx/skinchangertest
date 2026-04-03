# Installation & Deployment Guide

## Quick Installation (Web Interface)

### Step 1: Prerequisites Check

Verify you have:
- Windows 10/11
- Internet connection (for npm packages)
- ~200 MB free disk space

### Step 2: Install Node.js (if needed)

1. Download from https://nodejs.org/
2. Choose LTS version (16.x or higher)
3. Run installer with default settings
4. Verify installation:
   ```bash
   node --version
   npm --version
   ```

### Step 3: Start Web Server

**Easiest Method** - Double-click batch file:
```
C:\Users\qveezzx\Documents\Changer\START_WEB_SERVER.bat
```

**Or use command line**:
```bash
cd C:\Users\qveezzx\Documents\Changer
node web-server.js
```

### Step 4: Access Web Interface

1. Open your browser
2. Go to: **http://localhost:3000**
3. You should see the beautiful skinchanger UI

### Step 5: Use Skinchanger

1. Select knife from dropdown
2. Select gloves from dropdown
3. Adjust paint kit, wear, seed as desired
4. Click "APPLY SKINS" button
5. Watch the stats update in real-time!

---

## Detailed Installation

### For Windows Users

#### Using Batch File (Recommended)

1. Navigate to: `C:\Users\qveezzx\Documents\Changer\`
2. Double-click `START_WEB_SERVER.bat`
3. Wait for "Server running on http://localhost:3000"
4. Batch file will:
   - Check for Node.js
   - Install dependencies (first run only)
   - Start the server
   - Keep terminal open for logs

#### Using Command Prompt

```batch
REM Navigate to project
cd C:\Users\qveezzx\Documents\Changer

REM Install dependencies (first time only)
npm install express ws cors dotenv

REM Start server
node web-server.js

REM Expected output:
REM ╔════════════════════════════════════════════╗
REM ║   CS2 Skin Changer - Web Interface v1.0   ║
REM ╚════════════════════════════════════════════╝
REM 🎮 Server running on http://localhost:3000
REM 📡 WebSocket ready for real-time updates
```

#### Using PowerShell

```powershell
# Navigate to project
Set-Location "C:\Users\qveezzx\Documents\Changer"

# Install dependencies
npm install express ws cors dotenv

# Start server
node web-server.js
```

### For Linux/macOS Users

```bash
# Navigate to project
cd ~/Documents/Changer  # or wherever you cloned it

# Install dependencies
npm install express ws cors dotenv

# Start server
node web-server.js
```

---

## Configuration

### Changing Server Port

Edit `web-server.js` (line ~30):

```javascript
// OLD
const PORT = 3000;

// NEW
const PORT = 3001;  // or any port you prefer
```

Then update frontend URLs in `index.html` (around line 380):

```javascript
// OLD
const API_URL = 'http://localhost:3000';
const WS_URL = 'ws://localhost:3000';

// NEW
const API_URL = 'http://localhost:3001';
const WS_URL = 'ws://localhost:3001';
```

### Adding Custom Knives

Edit `web-server.js` app.get('/api/knives') section:

```javascript
app.get('/api/knives', (req, res) => {
  const knives = [
    { id: 0, name: 'Default Knife' },
    { id: 500, name: 'Bayonet' },
    // ADD YOUR CUSTOM KNIVES HERE
    { id: 999, name: 'Your Custom Knife' },
  ];
  res.json(knives);
});
```

### Adding Custom Gloves

Similarly, edit `web-server.js` app.get('/api/gloves') section:

```javascript
app.get('/api/gloves', (req, res) => {
  const gloves = [
    { id: 0, name: 'No Gloves' },
    { id: 5027, name: 'Bloodhound Gloves' },
    // ADD YOUR CUSTOM GLOVES HERE
    { id: 9999, name: 'Your Custom Gloves' },
  ];
  res.json(gloves);
});
```

### Customizing UI Theme

Edit CSS variables in `index.html` (around line 25):

```css
:root {
    --primary: #1a1a2e;        /* Dark background */
    --secondary: #16213e;      /* Card background */
    --accent: #0f3460;         /* Button hover */
    --highlight: #e94560;      /* Primary accent (red) */
    --success: #00d4aa;        /* Success color (green) */
    --text: #eaeaea;           /* Main text */
    --text-secondary: #a8a8a8; /* Secondary text */
    --border: #2d3561;         /* Border color */
}
```

Change these colors to customize the entire theme!

---

## Deployment Options

### Option 1: Local Development (Current)

✅ Easiest setup
✅ Perfect for testing
⚠️ Only accessible from this computer

**Setup**: Just run `START_WEB_SERVER.bat` and access `http://localhost:3000`

### Option 2: Remote Server (Advanced)

For accessing from other computers on the network:

1. Find your computer's IP address:
   ```bash
   ipconfig
   # Look for IPv4 Address (e.g., 192.168.1.100)
   ```

2. Update firewall (Windows Defender):
   - Allow Node.js through firewall
   - Port 3000 TCP

3. Others can access via: `http://YOUR_IP:3000`

### Option 3: Cloud Deployment (Optional)

For hosting on cloud services (Heroku, Azure, AWS, etc.):

1. Add PORT environment variable support:
   ```javascript
   const PORT = process.env.PORT || 3000;
   ```

2. Update package.json with cloud platform requirements

3. Deploy via platform-specific CLI

### Option 4: Docker Container (Advanced)

Create `Dockerfile`:

```dockerfile
FROM node:18-slim

WORKDIR /app

COPY package.json .
RUN npm install

COPY web-server.js .
COPY index.html .

EXPOSE 3000

CMD ["node", "web-server.js"]
```

Build and run:
```bash
docker build -t cs2-skinchanger .
docker run -p 3000:3000 cs2-skinchanger
```

---

## Building C++ Version (Optional)

If you want to compile and deploy the C++ version:

### Prerequisites

1. **Visual Studio 2022** (with C++ development tools)
   - Download: https://visualstudio.microsoft.com/
   - Select "Desktop development with C++"

2. **Windows SDK**
   - Installed automatically with Visual Studio

3. **NuGet** (should be included)

### Build Steps

```bash
# Navigate to project
cd C:\Users\qveezzx\Documents\Changer

# Open Visual Studio
start "src\ext-cs2-skin-changer.sln"

# Or build from command line
msbuild "src\ext-cs2-skin-changer.sln" /m /p:Configuration=Release /p:Platform=x64

# Output will be in:
# src\x64\Release\ext-cs2-skin-changer.exe
```

### Run C++ Version

```bash
src\x64\Release\ext-cs2-skin-changer.exe
```

---

## Troubleshooting Installation

### Issue: "Node.js not found"

**Solution**:
1. Install Node.js from https://nodejs.org/
2. Restart command prompt
3. Verify: `node --version`
4. Try again

### Issue: "Port 3000 already in use"

**Solution A** - Use different port:
1. Edit `web-server.js` line 30
2. Change `const PORT = 3000;` to `const PORT = 3001;`
3. Update URLs in `index.html`
4. Restart server

**Solution B** - Kill process using port:
```bash
# Find process
netstat -ano | findstr :3000

# Kill process (replace PID)
taskkill /PID <PID> /F
```

### Issue: "Cannot find module 'express'"

**Solution**:
```bash
cd C:\Users\qveezzx\Documents\Changer
npm install express ws cors dotenv
```

### Issue: "WebSocket connection failed"

**Solution**:
1. Ensure server is running (should see startup message)
2. Check firewall allows Node.js
3. Try accessing http://localhost:3000 first
4. Check browser console (F12) for errors

### Issue: "Batch file won't run"

**Solution**:
1. Right-click `START_WEB_SERVER.bat`
2. Select "Run as Administrator"
3. Or manually run from command prompt:
   ```bash
   cd C:\Users\qveezzx\Documents\Changer
   node web-server.js
   ```

---

## Updating & Maintenance

### Updating Dependencies

```bash
cd C:\Users\qveezzx\Documents\Changer

# Check for updates
npm outdated

# Update all
npm update

# Update specific package
npm install express@latest
```

### Clearing Configuration

To reset all saved settings:

```bash
# Delete config file
del skinconfig.json

# Next run will create fresh config
```

### Checking Logs

Server logs are displayed in command window where it's running.

To save logs to file:
```bash
node web-server.js > skinchanger.log 2>&1
```

### Performance Monitoring

Monitor resource usage while server running:
```bash
# Windows Task Manager
tasklist | findstr node.exe

# Or use system monitor
taskmgr
```

---

## Testing Installation

### Quick Test Checklist

- [ ] Node.js installed (`node --version`)
- [ ] Server starts without errors
- [ ] Browser loads http://localhost:3000
- [ ] UI displays correctly
- [ ] Can select knife without errors
- [ ] Can select gloves without errors
- [ ] Paint kit adjusts without errors
- [ ] Status badge shows "Connected"
- [ ] Apply button responds to clicks
- [ ] Settings saved to skinconfig.json

### Browser Console Check

1. Open http://localhost:3000
2. Press F12 to open developer tools
3. Check Console tab for errors
4. Should see WebSocket connected message

### API Test

```bash
# Test API endpoint
curl http://localhost:3000/api/status

# Should return JSON:
# {"status":"active","config":{...},"timestamp":"..."}
```

---

## Uninstallation

To completely remove:

```bash
# Delete project folder
rmdir /s /q C:\Users\qveezzx\Documents\Changer

# Or if you want to keep source but remove Node modules
cd C:\Users\qveezzx\Documents\Changer
rmdir /s /q node_modules

# If you want to remove Node.js entirely
# Use Windows Control Panel > Programs > Uninstall
```

---

## Support & Resources

### Documentation Files
- [README_WEB.md](README_WEB.md) - Complete web interface guide
- [QUICKSTART_COMPLETE.md](QUICKSTART_COMPLETE.md) - Project overview
- [PROJECT_STATUS.md](PROJECT_STATUS.md) - Status report
- [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) - Technical details

### External Resources
- Node.js Docs: https://nodejs.org/docs/
- Express.js: https://expressjs.com/
- WebSocket: https://developer.mozilla.org/en-US/docs/Web/API/WebSocket
- CSS Guide: https://developer.mozilla.org/en-US/docs/Web/CSS/

### Getting Help

1. Check browser console (F12)
2. Check server terminal output
3. Review troubleshooting section above
4. Check documentation files
5. Verify prerequisites are installed

---

## Performance Tips

### Optimize Server

```javascript
// In web-server.js, add compression
const compression = require('compression');
app.use(compression());

// Npm install compression first
npm install compression
```

### Reduce File Size

Already optimized! Index.html is single-file with embedded CSS/JS.

### Network Optimization

- WebSocket reuses connection (efficient)
- Config cached on client side
- Minimal API calls

---

## Security Notes

⚠️ **This tool is for LOCAL USE ONLY**

Current security:
- ✅ CORS restricted to localhost
- ✅ Input validation on all endpoints
- ✅ XSS protection via proper escaping
- ⚠️ No authentication (assumes local network)

For production/remote use, add:
- Authentication
- Rate limiting
- HTTPS/WSS
- Input sanitization
- Proper CORS whitelist

---

## Next Steps

1. ✅ Install Node.js (if needed)
2. ✅ Run START_WEB_SERVER.bat
3. ✅ Access http://localhost:3000
4. ✅ Start using!

---

**Version**: 1.0.0  
**Last Updated**: 2024-01-15  
**Status**: ✅ Ready for Installation
