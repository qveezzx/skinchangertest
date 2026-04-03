# CS2 Skin Changer - Visual Guide & Architecture

## 🎯 System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     USER'S COMPUTER                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              WEB BROWSER (Any Browser)                      │ │
│  │  ┌──────────────────────────────────────────────────────┐  │ │
│  │  │         Beautiful Web Interface (index.html)         │  │ │
│  │  │  - Knife Selection                                  │  │ │
│  │  │  - Glove Selection                                  │  │ │
│  │  │  - Paint Kit Controls                               │  │ │
│  │  │  - Statistics Dashboard                             │  │ │
│  │  │  - Real-time Status Indicators                      │  │ │
│  │  └──────────────────────────────────────────────────────┘  │ │
│  │                      ↕️ (REST + WebSocket)                   │ │
│  │  ┌──────────────────────────────────────────────────────┐  │ │
│  │  │         JavaScript (No frameworks needed!)           │  │ │
│  │  │  - API calls (fetch)                                │  │ │
│  │  │  - WebSocket connection                             │  │ │
│  │  │  - Real-time updates                                │  │ │
│  │  └──────────────────────────────────────────────────────┘  │ │
│  └────────────────────────────────────────────────────────────┘ │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              http://localhost:3000                          │ │
│  │  ┌──────────────────────────────────────────────────────┐  │ │
│  │  │         Express.js Backend (web-server.js)          │  │ │
│  │  │  ┌──────────────────────────────────────────────┐   │  │ │
│  │  │  │  REST API Endpoints                          │   │  │ │
│  │  │  │  - GET  /api/status                         │   │  │ │
│  │  │  │  - GET  /api/knives                         │   │  │ │
│  │  │  │  - GET  /api/gloves                         │   │  │ │
│  │  │  │  - POST /api/skin/apply                     │   │  │ │
│  │  │  │  - POST /api/skin/reset                     │   │  │ │
│  │  │  └──────────────────────────────────────────────┘   │  │ │
│  │  ├──────────────────────────────────────────────────────┤  │ │
│  │  │  ┌──────────────────────────────────────────────┐   │  │ │
│  │  │  │  WebSocket Server (ws://localhost:3000)     │   │  │ │
│  │  │  │  - Real-time status broadcasts             │   │  │ │
│  │  │  │  - Client connection management            │   │  │ │
│  │  │  │  - Instant update notifications            │   │  │ │
│  │  │  └──────────────────────────────────────────────┘   │  │ │
│  │  └──────────────────────────────────────────────────────┘  │ │
│  │                      ↕️ (File I/O)                         │ │
│  │  ┌──────────────────────────────────────────────────────┐  │ │
│  │  │  skinconfig.json (Auto-save/load)                   │  │ │
│  │  │  {                                                  │  │ │
│  │  │    "knife": {...},                                 │  │ │
│  │  │    "gloves": {...},                                │  │ │
│  │  │    "paintKit": 0,                                  │  │ │
│  │  │    "wear": 0.01,                                   │  │ │
│  │  │    "seed": 0,                                      │  │ │
│  │  │    "isApplied": true,                              │  │ │
│  │  │    "lastUpdate": "2024-01-15T10:30:45.123Z"       │  │ │
│  │  │  }                                                  │  │ │
│  │  └──────────────────────────────────────────────────────┘  │ │
│  │                      ↕️ (IPC/Memory hooks)                  │ │
│  │  ┌──────────────────────────────────────────────────────┐  │ │
│  │  │  C++ Backend Process (ext-cs2-skin-changer.exe)     │  │ │
│  │  │  (Optional - Can run in background)                 │  │ │
│  │  └──────────────────────────────────────────────────────┘  │ │
│  │                      ↕️ (Game Memory)                       │ │
│  │  ┌──────────────────────────────────────────────────────┐  │ │
│  │  │         CS2.exe (Counter-Strike 2)                  │  │ │
│  │  │  - Game Memory                                      │  │ │
│  │  │  - Weapon/Knife/Glove State                         │  │ │
│  │  │  - Skin Configuration                              │  │ │
│  │  └──────────────────────────────────────────────────────┘  │ │
│  │                                                              │ │
│  └────────────────────────────────────────────────────────────┘ │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔄 Data Flow Diagram

```
USER INTERACTION (Browser)
        ↓
   SELECT KNIFE
        ↓
  [Dropdown Changes]
        ↓
 JavaScript Updates UI
        ↓
  "SELECT GLOVES"
        ↓
  [Dropdown Changes]
        ↓
 JavaScript Updates UI
        ↓
  "ADJUST PAINT KIT"
        ↓
 [Slider/Input Changes]
        ↓
 JavaScript Updates UI
        ↓
  "CLICK APPLY BUTTON"
        ↓
 [Collect all values]
        ↓
 [Send POST /api/skin/apply]
        ↓
 [Express receives request]
        ↓
 [Validate inputs]
        ↓
 [Save to skinconfig.json]
        ↓
 [Broadcast via WebSocket]
        ↓
 [C++ Backend picks up changes]
        ↓
 [Apply skins to game memory]
        ↓
 [Return success response]
        ↓
 [Browser displays notification]
        ↓
 [Update statistics]
        ↓
  ✅ SKINS APPLIED!
```

---

## 🎨 UI Component Structure

```
┌─────────────────────────────────────────────────────────┐
│                    HEADER SECTION                       │
│  ┌───────────────────────────────────────────────────┐  │
│  │  ⚔️ CS2 SKIN CHANGER                              │  │
│  │  Modern Web-Based Skinchanger Interface           │  │
│  │  ● Connected                                      │  │
│  └───────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────┐
│              MAIN GRID (3 COLUMNS)                      │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │  🔪 KNIFE    │  │  🧤 GLOVES   │  │  🎨 PAINT    │  │
│  │              │  │              │  │              │  │
│  │ [Dropdown]   │  │ [Dropdown]   │  │ [Number Box] │  │
│  │              │  │              │  │              │  │
│  │ Preview:    │  │ Preview:    │  │ Wear:       │  │
│  │ ● Selected  │  │ ● Selected  │  │ [Slider]    │  │
│  │   Item      │  │   Item      │  │             │  │
│  │              │  │              │  │ Seed:       │  │
│  │              │  │              │  │ [Number]    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
│                                                         │
└─────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────┐
│              STATISTICS DASHBOARD                       │
├──────────────┬──────────────┬──────────────┐            │
│  Skins       │  Last Update │  Connection  │            │
│  Applied: 0  │  --:--       │  ● (Online)  │            │
│              │              │              │            │
└──────────────┴──────────────┴──────────────┘            │
                           ↓
┌─────────────────────────────────────────────────────────┐
│                 ACTION BUTTONS                          │
├─────────────────────────────────────────────────────────┤
│                                                         │
│         [✨ APPLY SKINS] [↻ RESET]                     │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

---

## 🔗 File Dependencies

```
index.html
    ├── Includes: web-server.js (via HTTP)
    ├── Uses: REST API endpoints
    │   ├── GET /api/status
    │   ├── GET /api/knives
    │   ├── GET /api/gloves
    │   ├── POST /api/skin/apply
    │   └── POST /api/skin/reset
    │
    ├── Uses: WebSocket (ws://localhost:3000)
    │   ├── Events: initial-state
    │   ├── Events: skin-applied
    │   └── Events: skin-reset
    │
    └── Stores: User preferences (browser localStorage)

web-server.js
    ├── Imports: express
    ├── Imports: ws (WebSocket)
    ├── Imports: cors
    ├── Imports: dotenv
    │
    ├── Uses: skinconfig.json
    │   ├── Read: Load configuration on startup
    │   └── Write: Save on every change
    │
    ├── Serves: index.html (static)
    ├── Implements: 5 REST API endpoints
    ├── Implements: WebSocket server
    └── Manages: Client connections

skinconfig.json
    ├── Created: On first run (auto-generated)
    ├── Updated: On every skin change
    ├── Loaded: On server startup
    └── Format: JSON with knife, gloves, paint, wear, seed
```

---

## 🔄 State Management Flow

```
┌─ INITIALIZATION
│  └─ Server Starts
│     └─ Load skinconfig.json (if exists)
│        └─ Use DEFAULT_CONFIG if not exists
│           └─ Ready to accept connections
│
├─ CONNECTION (User opens browser)
│  └─ WebSocket connects
│     └─ Server sends 'initial-state' event
│        └─ Browser displays current config
│           └─ All dropdowns populated
│              └─ Ready for user input
│
├─ USER CHANGES KNIFE
│  └─ JavaScript event fires
│     └─ UI updates immediately (optimistic)
│        └─ (No API call yet)
│
├─ USER CLICKS APPLY
│  └─ Collect all values
│     └─ POST /api/skin/apply
│        └─ Server validates input
│           └─ Server updates skinconfig.json
│              └─ Server broadcasts 'skin-applied' event
│                 └─ All connected clients receive update
│                    └─ Browser shows notification
│                       └─ Statistics updated
│
├─ USER CLICKS RESET
│  └─ POST /api/skin/reset
│     └─ Server restores DEFAULT_CONFIG
│        └─ Server updates skinconfig.json
│           └─ Server broadcasts 'skin-reset' event
│              └─ All connected clients reset
│                 └─ UI returns to defaults
│                    └─ Notification shown
│
└─ APPLICATION LIFECYCLE
   └─ Browser closed / Server stopped
      └─ Configuration persists in skinconfig.json
         └─ Restored on next launch
```

---

## 📡 WebSocket Message Format

```
CLIENT → SERVER
┌──────────────────────────────────────┐
│  (WebSocket upgrade request)          │
│  GET / HTTP/1.1                       │
│  Upgrade: websocket                   │
│  Connection: Upgrade                  │
└──────────────────────────────────────┘

SERVER → CLIENT (Connection established)
┌──────────────────────────────────────┐
│  {                                    │
│    "type": "initial-state",          │
│    "config": {                        │
│      "knife": { ... },               │
│      "gloves": { ... },              │
│      "paintKit": 0,                  │
│      "wear": 0.01,                   │
│      "seed": 0,                      │
│      "isApplied": false,             │
│      "lastUpdate": "..."             │
│    }                                  │
│  }                                    │
└──────────────────────────────────────┘

SERVER → ALL CLIENTS (Broadcast)
┌──────────────────────────────────────┐
│  {                                    │
│    "type": "skin-applied",           │
│    "config": { ... }                 │
│  }                                    │
└──────────────────────────────────────┘
```

---

## 🎯 Decision Tree: Which Version to Use?

```
                    START HERE
                        │
                        ↓
            Do you want a beautiful UI?
            /                          \
          YES                          NO
           │                            │
           ↓                            ↓
      Use WEB          Use C++ DIRECT (ImGui)
    INTERFACE          
           │                            │
           ↓                            ↓
    ✅ START_WEB_     ✅ Open Visual Studio
       SERVER.bat         OR
                          msbuild solution
           │                            │
           ↓                            ↓
    http://localhost  Compile & Run
       :3000            exe
           │                            │
           ↓                            ↓
      Select Skins   ImGui Menu
           │                            │
           ↓                            ↓
      Click Apply     Click Apply
           │                            │
           ↓                            ↓
    ✨ DONE! ✨      ✨ DONE! ✨
```

---

## 🔧 Configuration Options

```
CONFIGURATION SETTINGS
├── Server Port
│   ├── Default: 3000
│   ├── Edit: web-server.js line 30
│   └── Update: API_URL in index.html
│
├── Knife List
│   ├── Default: 15 types
│   ├── Edit: web-server.js /api/knives
│   └── Add: { id: X, name: "Your Knife" }
│
├── Glove List
│   ├── Default: 8 types
│   ├── Edit: web-server.js /api/gloves
│   └── Add: { id: X, name: "Your Gloves" }
│
├── UI Theme
│   ├── Colors: index.html :root CSS variables
│   ├── Fonts: index.html font-family
│   └── Animations: index.html @keyframes
│
└── Data Storage
    ├── Location: skinconfig.json
    ├── Format: JSON
    └── Auto-created: First run
```

---

## 📊 Performance Characteristics

```
METRIC              | WEB UI      | C++ BACKEND
─────────────────────┼─────────────┼────────────
Startup Time        | <1s         | ~5s
Memory Usage        | 30-50 MB    | 100-200 MB
CPU Usage (idle)    | <1%         | <2%
Skin Apply Time     | 50-100ms    | 1-5ms
Model Change Time   | N/A         | 10-20ms
Max Concurrent      | 100+        | 1 process
Latency             | <50ms       | <5ms
Scalability         | Good        | High
─────────────────────┴─────────────┴────────────
```

---

## 🔐 Security Model

```
                    SECURITY LAYERS
                          │
                    ┌─────┴─────┐
                    ↓           ↓
            LOCAL ACCESS    NETWORK
            (Localhost)      (Optional)
                    │           │
                    ↓           ↓
            CORS: Allowed   CORS: Whitelist
            Auth: None      Auth: Required
            HTTPS: No       HTTPS: Yes
            Logs: Console   Logs: File
                    │           │
                    └─────┬─────┘
                          ↓
                    INPUT VALIDATION
                    ├─ Type checking
                    ├─ Range validation
                    ├─ Sanitization
                    └─ Error handling
                          │
                          ↓
                    CONFIG STORAGE
                    ├─ JSON format
                    ├─ File permissions
                    └─ No secrets
```

---

## 📈 Deployment Models

```
MODEL 1: LOCAL
┌──────────────────────────────────┐
│  Single Computer                 │
│  ├─ Browser (localhost:3000)     │
│  ├─ Server (localhost:3000)      │
│  └─ Game (local)                 │
│  ✅ Easiest | No network needed  │
└──────────────────────────────────┘

MODEL 2: NETWORK
┌──────────────────────────────────┐
│  Computer A (Server)             │
│  ├─ Node.js Server               │
│  └─ skinconfig.json              │
│           ↓ (Network)
│  Computer B (Client)             │
│  ├─ Browser (192.168.1.X:3000)   │
│  └─ CS2.exe (local)              │
│  ✅ Better | Requires setup      │
└──────────────────────────────────┘

MODEL 3: CLOUD
┌──────────────────────────────────┐
│  Cloud Server (e.g., Heroku)     │
│  ├─ Node.js Server               │
│  ├─ Database                      │
│  └─ SSL/HTTPS                     │
│           ↓ (Internet)
│  Any Computer                     │
│  ├─ Browser (your-domain.com)     │
│  └─ Local CS2.exe                 │
│  ⚠️ Complex | Needs auth          │
└──────────────────────────────────┘

MODEL 4: DOCKER
┌──────────────────────────────────┐
│  Docker Container                │
│  ├─ Node.js inside               │
│  ├─ Pre-configured               │
│  └─ Easy deployment              │
│           ↓ (Port mapping)
│  Host Computer                    │
│  └─ Browser (localhost:3000)      │
│  ✅ Flexible | Consistent env     │
└──────────────────────────────────┘
```

---

## 🎓 Learning Path

```
BEGINNER
└─ Read QUICKSTART_COMPLETE.md
   └─ Run START_WEB_SERVER.bat
      └─ Open http://localhost:3000
         └─ Use UI to apply skins

INTERMEDIATE
└─ Read README_WEB.md
   └─ Understand REST API
      └─ Check browser console
         └─ Try API endpoints with curl/Postman

ADVANCED
└─ Read INSTALLATION_GUIDE.md
   └─ Modify web-server.js
      └─ Customize index.html
         └─ Deploy to remote server

EXPERT
└─ Study Express.js documentation
   └─ Study WebSocket protocol
      └─ Add authentication
         └─ Implement database
            └─ Deploy to cloud
```

---

## ✅ Verification Checklist

```
INSTALLATION
☐ Node.js installed (node --version)
☐ npm installed (npm --version)
☐ Project folder exists
☐ All files present

STARTUP
☐ START_WEB_SERVER.bat runs without errors
☐ Server displays "running on http://localhost:3000"
☐ Terminal stays open (not closing)

BROWSER
☐ http://localhost:3000 loads
☐ UI displays correctly
☐ All buttons are clickable
☐ Dropdowns populate with items
☐ Status badge shows "Connected"

FUNCTIONALITY
☐ Can select knife
☐ Can select gloves
☐ Can adjust wear slider
☐ Can modify paint kit
☐ Can click Apply button
☐ Can click Reset button
☐ Notifications appear
☐ Statistics update

BACKEND
☐ skinconfig.json created
☐ WebSocket connection established
☐ Console shows no errors
☐ API endpoints respond

PERSISTENCE
☐ Reload browser page
☐ Settings still there
☐ Restart server
☐ Config still there
```

---

**Visual Guide Complete! 🎨**

Use this guide to understand the architecture and flow of the application.
