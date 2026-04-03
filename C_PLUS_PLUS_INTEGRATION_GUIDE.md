# C++ Integration Guide - Single EXE Deployment

## 🎯 Goal
Create a single executable that:
1. Starts web server internally
2. Opens browser automatically
3. Hosts web UI on localhost:3000
4. Communicates with game memory
5. User sees a professional application

---

## 🏗️ Architecture

```
CS2-SkinChanger.exe
├─ Embedded Node.js Runtime
│  └─ web-server.js + Dependencies
├─ C++ Skinchanger Logic
│  └─ Memory operations
│  └─ Game interface
└─ Web UI (HTML/CSS/JS)
   └─ index.html
   └─ skinconfig.json
```

---

## 📋 Integration Steps

### Step 1: Embed Node.js Runtime

**Option A: Node.js Standalone (Recommended)**
```cpp
// In main.cpp (C++ project)
#include <windows.h>
#include <shellapi.h>

bool StartWebServer() {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    
    // Create hidden console for Node.js process
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    // Run node web-server.js in background
    BOOL result = CreateProcess(
        "node.exe",
        "web-server.js",
        NULL, NULL, FALSE,
        CREATE_NEW_PROCESS_GROUP,
        NULL,
        GetCurrentDirectory(),
        &si, &pi
    );
    
    if (result) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        Sleep(2000); // Wait for server to start
        return true;
    }
    return false;
}
```

**Option B: Embed Node.js (Advanced)**
- Use node-js library to embed Node.js directly
- Reduces dependencies
- More complex build process

### Step 2: Auto-Open Browser

```cpp
void OpenWebInterface() {
    HINSTANCE result = ShellExecuteA(
        NULL,
        "open",
        "http://localhost:3000",
        NULL,
        NULL,
        SW_SHOW
    );
    
    if ((int)result <= 32) {
        // Fallback
        system("start http://localhost:3000");
    }
}
```

### Step 3: Main Application Flow

```cpp
int main() {
    // 1. Initialize skinchanger
    InitializeMemory();
    
    // 2. Start web server
    if (!StartWebServer()) {
        MessageBox(NULL, "Failed to start web server", "Error", MB_OK);
        return 1;
    }
    
    // 3. Open browser
    OpenWebInterface();
    
    // 4. Main loop
    while (true) {
        // 5. Poll for config changes
        CheckAndApplySkinChanges();
        
        // 6. Monitor game
        MonitorGameState();
        
        Sleep(5);
    }
    
    return 0;
}
```

### Step 4: Configuration Polling

```cpp
void CheckAndApplySkinChanges() {
    static std::string lastConfig = "";
    
    // Read skinconfig.json from current directory
    std::ifstream configFile("skinconfig.json");
    if (configFile) {
        std::stringstream buffer;
        buffer << configFile.rdbuf();
        std::string currentConfig = buffer.str();
        
        if (currentConfig != lastConfig) {
            lastConfig = currentConfig;
            
            // Parse JSON
            auto config = nlohmann::json::parse(currentConfig);
            
            // Apply weapon skin if changed
            if (config.contains("weapon")) {
                uint16_t weaponID = config["weapon"]["id"];
                ApplyWeaponSkin(weaponID);
            }
            
            // Apply knife if changed
            if (config.contains("knife")) {
                uint16_t knifeID = config["knife"]["defIndex"];
                ApplyKnifeSkin(knifeID);
            }
            
            // Apply gloves if changed
            if (config.contains("gloves")) {
                uint16_t glovesID = config["gloves"]["defIndex"];
                ApplyGloveSkin(glovesID);
            }
            
            // Apply paint kit
            if (config.contains("paintKit")) {
                uint32_t paintKit = config["paintKit"];
                uint32_t wear = config["wear"];
                ApplyPaintKit(paintKit, wear);
            }
        }
    }
}
```

### Step 5: IPC Communication

```cpp
// Optional: Direct IPC instead of file polling
#include <winsock2.h>

void SendStatusToWeb(const std::string& status) {
    // Connect to localhost:3000
    // Send status as WebSocket message
    // This enables better integration
}
```

---

## 📦 File Structure for Distribution

```
CS2-SkinChanger/
├── CS2-SkinChanger.exe          (Main executable)
├── web-server.js                (Node.js server)
├── index.html                   (Web UI - embedded or external)
├── skinconfig.json              (Config file)
├── config.json                  (App config)
└── node_modules/                (Dependencies)
    ├── express/
    ├── ws/
    └── cors/
```

---

## 🛠️ Build Process

### Visual Studio Project Setup

1. **Add Node.js Detection**
```xml
<!-- .vcxproj -->
<ItemGroup>
    <NodeExecutable Include="$(NodePath)\node.exe" />
</ItemGroup>
```

2. **Copy Resources**
```xml
<PostBuildEvent>
    <Command>
        copy "$(ProjectDir)web-server.js" "$(OutDir)"
        copy "$(ProjectDir)index.html" "$(OutDir)"
        copy "$(ProjectDir)package.json" "$(OutDir)"
    </Command>
</PostBuildEvent>
```

3. **Bundle Dependencies**
```batch
REM Pre-build: Install npm packages
call npm install --prefix "$(ProjectDir)"
```

---

## 🔗 Communication Protocol

### Method 1: File Polling (Simple)
- C++ reads `skinconfig.json`
- Updates when file changes
- Pros: Simple, reliable
- Cons: File I/O overhead

### Method 2: WebSocket (Advanced)
- Backend broadcasts changes
- C++ listens via WebSocket
- Pros: Real-time, efficient
- Cons: More complex

### Method 3: Named Pipes (Windows-Specific)
- Inter-process communication
- Bidirectional messaging
- Pros: Secure, efficient
- Cons: Windows only

**Recommended**: Use Method 1 (File Polling) for initial implementation, upgrade to Method 2 later.

---

## 🚀 Deployment

### Step 1: Prepare Files
```bash
# In your project directory
npm install express ws cors dotenv

# Create release package
mkdir CS2-SkinChanger
copy ext-cs2-skin-changer.exe CS2-SkinChanger/
copy web-server.js CS2-SkinChanger/
copy index.html CS2-SkinChanger/
xcopy node_modules CS2-SkinChanger/node_modules /E
```

### Step 2: Build C++ Project
```bash
# Visual Studio
msbuild "ext-cs2-skin-changer.sln" /p:Configuration=Release /p:Platform=x64

# Copy to deployment folder
copy "src\x64\Release\ext-cs2-skin-changer.exe" "CS2-SkinChanger\"
```

### Step 3: Package for Distribution
```bash
# Create ZIP for distribution
7z a CS2-SkinChanger-v2.0.zip CS2-SkinChanger/

# Users extract and run:
# double-click CS2-SkinChanger.exe
```

---

## ⚙️ Configuration File

### config.json (Application Config)
```json
{
  "server": {
    "port": 3000,
    "autoOpen": true,
    "hideConsole": true
  },
  "paths": {
    "web": "./",
    "config": "./skinconfig.json",
    "logs": "./logs/"
  },
  "game": {
    "processName": "cs2.exe",
    "memoryOffset": "0x..."
  }
}
```

---

## 🔒 Security Considerations

### For Single EXE:
1. **No External Dependencies** - Everything bundled
2. **Localhost Only** - No network access needed
3. **Process Isolation** - Separate Node.js process
4. **Admin Privileges** - Required for memory operations
5. **File Permissions** - Set restrictions on config file

### Code Example:
```cpp
bool EnsureLocalhost() {
    // Verify server only listening on 127.0.0.1
    // Prevent external connections
    // Log all activities
    return true;
}
```

---

## 📊 Resource Usage

**Expected Usage**:
- Memory: 150-300 MB (C++ + Node.js)
- CPU: <2% idle
- Disk: ~100 MB (with dependencies)

**Optimization**:
- Use minimal Node.js runtime
- Lazy load modules
- Cache static files
- Cleanup temp files

---

## 🐛 Troubleshooting

### Node.js Not Found
```cpp
// Check if node.exe exists in PATH
if (!FileExists("node.exe")) {
    // Use embedded Node.js path
    CreateProcess("./node/node.exe", ...);
}
```

### Port 3000 Already in Use
```cpp
// Find available port
int port = 3000;
while (!IsPortAvailable(port)) {
    port++;
}
// Update config with new port
```

### Browser Won't Open
```cpp
// Fallback methods
ShellExecuteA(...); // Method 1
system("start http://localhost:3000"); // Method 2
WinExec("explorer http://localhost:3000", SW_SHOW); // Method 3
```

---

## 📝 Implementation Timeline

| Phase | Task | Est. Time |
|-------|------|-----------|
| 1 | Web server startup in C++ | 2-3 hours |
| 2 | Auto-open browser | 1 hour |
| 3 | Configuration polling | 2-3 hours |
| 4 | Testing & debugging | 2-3 hours |
| 5 | Packaging & deployment | 1-2 hours |
| **Total** | | **8-12 hours** |

---

## ✅ Testing Checklist

- [ ] EXE starts without errors
- [ ] Web server starts in background
- [ ] Browser opens to localhost:3000
- [ ] Web UI loads completely
- [ ] Can select weapons
- [ ] Can select knives
- [ ] Can select gloves
- [ ] Can adjust paint kit
- [ ] Apply button works
- [ ] Reset button works
- [ ] Skins apply to game
- [ ] Config persists
- [ ] Process closes cleanly
- [ ] No zombie processes
- [ ] All logs are captured

---

## 🎯 Success Criteria

Users should be able to:
1. ✅ Download single EXE
2. ✅ Double-click to run
3. ✅ See web UI open automatically
4. ✅ Select and apply skins
5. ✅ Close EXE cleanly
6. ✅ No installation needed

---

## 📚 Resources

- [Node.js Win API](https://github.com/npm/npm/issues)
- [Windows Process Creation](https://docs.microsoft.com/en-us/windows/win32/procthread/creating-processes)
- [ShellExecute Alternatives](https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutea)

---

**Integration Guide v1.0**  
**Status**: Ready for Implementation  
**Complexity**: Medium  
**Est. Development**: 8-12 hours  

**Next Step**: Implement file polling integration in C++ main loop!

---

## Quick Start Code Template

```cpp
// main.cpp - Integration template
#include <windows.h>
#include <shellapi.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Global process handle
PROCESS_INFORMATION g_serverProcess;

void StartWebServer() {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    CreateProcess("node.exe", "web-server.js",
                  NULL, NULL, FALSE, 0, NULL, NULL, &si, &g_serverProcess);
    Sleep(2000); // Wait for startup
}

void OpenBrowser() {
    ShellExecuteA(NULL, "open", "http://localhost:3000", 
                  NULL, NULL, SW_SHOW);
}

void ApplySkinChanges() {
    std::ifstream file("skinconfig.json");
    if (file) {
        json config = json::parse(file);
        // Apply skins from config
    }
}

int main() {
    InitializeMemory();
    StartWebServer();
    OpenBrowser();
    
    while (true) {
        ApplySkinChanges();
        Sleep(100);
    }
    
    TerminateProcess(g_serverProcess.hProcess, 0);
    return 0;
}
```

Good luck with the integration! 🚀
