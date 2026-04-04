# 🎮 Process Detection Implementation - Ready to Add to main.cpp

## What This Does

- ✅ Waits for CS2.exe to launch before starting
- ✅ Waits for Steam.exe before initializing overlay
- ✅ Cleaner startup sequence with logging
- ✅ Better error handling
- ✅ Can be added to existing main.cpp in ~5 minutes

---

## 📋 Complete Code to Add

### Step 1: Add Header Section

Add this to the top of `main.cpp` (after existing includes):

```cpp
#include <tlhelp32.h>
#include <string>
#include <thread>
#include <chrono>
```

### Step 2: Add Process Detection Namespace

Add this before the `main()` function:

```cpp
// ============================================================
// PROCESS DETECTION NAMESPACE
// ============================================================

namespace ProcessDetection
{
    /// <summary>
    /// Finds a process by its executable name
    /// </summary>
    /// <param name="processName">Name of the executable (e.g., "cs2.exe")</param>
    /// <returns>Process ID if found, 0 if not found</returns>
    DWORD FindProcessByName(const std::string& processName)
    {
        PROCESSENTRY32 pe32 = {};
        pe32.dwSize = sizeof(PROCESSENTRY32);
        
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE)
            return 0;
        
        if (Process32First(hProcessSnap, &pe32))
        {
            do {
                // Case-insensitive comparison
                if (_stricmp(pe32.szExeFile, processName.c_str()) == 0)
                {
                    CloseHandle(hProcessSnap);
                    return pe32.th32ProcessID;
                }
            } while (Process32Next(hProcessSnap, &pe32));
        }
        
        CloseHandle(hProcessSnap);
        return 0;
    }
    
    /// <summary>
    /// Waits for a process to launch with optional timeout
    /// </summary>
    /// <param name="processName">Name of the executable to wait for</param>
    /// <param name="timeoutSeconds">Timeout in seconds (-1 for infinite wait)</param>
    /// <returns>Process ID if found, 0 if timeout</returns>
    DWORD WaitForProcess(const std::string& processName, int timeoutSeconds = -1)
    {
        int elapsed = 0;
        
        while (timeoutSeconds < 0 || elapsed < timeoutSeconds)
        {
            DWORD pid = FindProcessByName(processName);
            if (pid != 0)
            {
                std::cout << "[Loader] ✓ Found " << processName << " (PID: " << pid << ")" << std::endl;
                return pid;
            }
            
            // Print progress every 5 seconds
            if (elapsed % 5 == 0)
                std::cout << "[Loader] ⏳ Waiting for " << processName << "... (" << elapsed << "s)" << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
            elapsed++;
        }
        
        std::cout << "[Loader] ✗ Timeout: " << processName << " not found after " << timeoutSeconds << " seconds" << std::endl;
        return 0;
    }
    
    /// <summary>
    /// Waits for both CS2 and Steam processes
    /// Shows detailed startup information
    /// </summary>
    void WaitForBothCS2AndSteam()
    {
        std::cout << std::endl;
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
        std::cout << "  CS2 SKIN CHANGER - STARTUP SEQUENCE" << std::endl;
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
        std::cout << std::endl;
        
        // Wait for CS2
        std::cout << "[Loader] Step 1/2: Waiting for CS2 to launch..." << std::endl;
        std::cout << "[Loader] Please start Counter-Strike 2 from Steam" << std::endl;
        std::cout << std::endl;
        
        DWORD cs2PID = WaitForProcess("cs2.exe");
        
        if (cs2PID == 0)
        {
            std::cerr << "[Loader] ✗ ERROR: CS2 not found!" << std::endl;
            std::cerr << "[Loader] Please make sure Counter-Strike 2 is running" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            exit(1);
        }
        
        std::cout << std::endl;
        std::cout << "[Loader] Step 2/2: Waiting for Steam overlay..." << std::endl;
        
        // Wait for Steam (optional, with timeout)
        DWORD steamPID = WaitForProcess("steam.exe", 30);
        
        if (steamPID == 0)
        {
            std::cout << "[Loader] ⚠ Warning: Steam not found within 30 seconds" << std::endl;
            std::cout << "[Loader] Continuing anyway..." << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
        std::cout << "[Loader] ✓ All systems ready!" << std::endl;
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
        std::cout << "[Loader] CS2 PID: " << cs2PID << std::endl;
        if (steamPID != 0)
            std::cout << "[Loader] Steam PID: " << steamPID << std::endl;
        std::cout << "[Loader] Status: Initializing skin changer..." << std::endl;
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
        std::cout << std::endl;
    }
}

// ============================================================
// END PROCESS DETECTION NAMESPACE
// ============================================================
```

### Step 3: Modify main() Function

At the very beginning of `main()`, add:

```cpp
int main()
{
    // === PROCESS DETECTION AND STARTUP ===
    // Wait for CS2 and Steam processes before initializing
    ProcessDetection::WaitForBothCS2AndSteam();
    
    // === REST OF EXISTING CODE CONTINUES ===
    mem.Write<uint16_t>(Sigs::RegenerateWeaponSkins + 0x52, Offsets::m_AttributeManager + Offsets::m_Item + Offsets::m_AttributeList + Offsets::m_Attributes);
    
    skindb->Dump();
    Updater::UpdateOffsets();
    
    InitMenu();
    
    configManager->Setup();
    configManager->AutoLoad();

    std::cout << "SkinChanger Started\n";

    while (true)
    {
        Sleep(5);
        // ... rest of loop code ...
    }
    
    return 0;
}
```

---

## 🎯 What This Adds

### Process Detection Features
- ✅ Detects when CS2.exe launches
- ✅ Detects when Steam.exe launches
- ✅ Timeout handling (optional)
- ✅ Detailed logging
- ✅ Visual progress indicators

### Startup Sequence
```
1. Tool starts
   ↓
2. Waits for CS2.exe
   ↓
3. Once CS2 detected, waits for Steam.exe (optional, 30s timeout)
   ↓
4. Both ready → Initializes skin changer
   ↓
5. Ready to use!
```

### Console Output
```
════════════════════════════════════════════════════════════
  CS2 SKIN CHANGER - STARTUP SEQUENCE
════════════════════════════════════════════════════════════

[Loader] Step 1/2: Waiting for CS2 to launch...
[Loader] Please start Counter-Strike 2 from Steam

[Loader] ⏳ Waiting for cs2.exe... (0s)
[Loader] ⏳ Waiting for cs2.exe... (5s)
[Loader] ✓ Found cs2.exe (PID: 12345)

[Loader] Step 2/2: Waiting for Steam overlay...
[Loader] ✓ Found steam.exe (PID: 54321)

════════════════════════════════════════════════════════════
[Loader] ✓ All systems ready!
════════════════════════════════════════════════════════════
[Loader] CS2 PID: 12345
[Loader] Steam PID: 54321
[Loader] Status: Initializing skin changer...
════════════════════════════════════════════════════════════
```

---

## 🔧 How It Works

### FindProcessByName()
```
Takes process name (e.g., "cs2.exe")
↓
Creates snapshot of all running processes
↓
Searches for matching process
↓
Returns PID (Process ID) if found, 0 if not
```

### WaitForProcess()
```
Checks if process is running
↓ If not found, wait 1 second
↓ Check again (repeat)
↓ Continue until process found or timeout
↓ Return PID or 0
```

### WaitForBothCS2AndSteam()
```
Display welcome message
↓
Wait for CS2 (infinite timeout)
↓
Wait for Steam (30 second timeout)
↓
Display ready message
↓
Continue with initialization
```

---

## 📊 Performance Impact

| Aspect | Impact |
|--------|--------|
| **CPU Usage** | Minimal (1 check per second) |
| **Memory** | ~1 KB snapshot |
| **Startup Time** | +0-5 seconds (depends on when CS2 starts) |
| **Runtime** | Zero impact (only runs at startup) |

---

## 🎯 Use Cases

### Scenario 1: Tool Starts Before CS2
```
1. User runs tool
2. Tool waits and shows "Please start CS2"
3. User launches CS2 from Steam
4. Tool detects it, initializes
5. Tool is ready to use
```

### Scenario 2: Tool Starts After CS2
```
1. CS2 already running
2. User runs tool
3. Tool immediately detects CS2
4. Tool waits for Steam overlay
5. Tool initializes
```

### Scenario 3: No Steam Detected
```
1. CS2 running
2. Steam not running (offline mode)
3. Tool waits 30 seconds for Steam
4. Timeout - shows warning
5. Continues anyway (Steam optional)
```

---

## ⚙️ Configuration Options

### Modify Timeouts

Change Steam timeout:
```cpp
// Current: 30 seconds
DWORD steamPID = WaitForProcess("steam.exe", 30);

// Make it longer:
DWORD steamPID = WaitForProcess("steam.exe", 60);

// Make it infinite:
DWORD steamPID = WaitForProcess("steam.exe", -1);
```

### Make Steam Required

```cpp
if (steamPID == 0)
{
    std::cerr << "[Loader] FATAL: Steam is required!" << std::endl;
    exit(1);
}
```

### Add More Processes to Detect

```cpp
DWORD discordPID = ProcessDetection::WaitForProcess("Discord.exe", 10);
DWORD obsPID = ProcessDetection::WaitForProcess("obs64.exe", 10);
```

---

## 🧪 Testing

### To Test This Code

1. **Before CS2 is running**:
   - Run tool
   - Should say "Waiting for CS2"
   - Start CS2
   - Tool should detect it

2. **After CS2 is running**:
   - Start CS2
   - Run tool
   - Should detect CS2 immediately
   - Should wait for Steam
   - Should initialize

3. **With Steam offline**:
   - Run CS2 offline
   - Run tool
   - Should warn about Steam
   - Should still work

---

## 🐛 Troubleshooting

### "Waiting for cs2.exe..." forever

**Possible causes**:
- CS2 process name changed (unlikely)
- Running in safe mode
- Process name is different

**Solutions**:
```cpp
// Check actual process name
// Use Task Manager to verify executable name
// May need to change "cs2.exe" to something else
```

### "Steam not found" warning

**This is normal**:
- Steam overlay not required for basic functionality
- Optional warning, doesn't block tool
- Tool continues normally

---

## 📝 Full Example

Here's the complete code addition:

```cpp
// At top of main.cpp
#include <tlhelp32.h>
#include <string>
#include <thread>
#include <chrono>

// Before main() function
namespace ProcessDetection
{
    DWORD FindProcessByName(const std::string& processName)
    {
        PROCESSENTRY32 pe32 = {};
        pe32.dwSize = sizeof(PROCESSENTRY32);
        
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE)
            return 0;
        
        if (Process32First(hProcessSnap, &pe32))
        {
            do {
                if (_stricmp(pe32.szExeFile, processName.c_str()) == 0)
                {
                    CloseHandle(hProcessSnap);
                    return pe32.th32ProcessID;
                }
            } while (Process32Next(hProcessSnap, &pe32));
        }
        
        CloseHandle(hProcessSnap);
        return 0;
    }
    
    DWORD WaitForProcess(const std::string& processName, int timeoutSeconds = -1)
    {
        int elapsed = 0;
        
        while (timeoutSeconds < 0 || elapsed < timeoutSeconds)
        {
            DWORD pid = FindProcessByName(processName);
            if (pid != 0)
            {
                std::cout << "[Loader] ✓ Found " << processName << " (PID: " << pid << ")" << std::endl;
                return pid;
            }
            
            if (elapsed % 5 == 0)
                std::cout << "[Loader] ⏳ Waiting for " << processName << "..." << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
            elapsed++;
        }
        
        return 0;
    }
    
    void WaitForBothCS2AndSteam()
    {
        std::cout << std::endl;
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
        std::cout << "  CS2 SKIN CHANGER - STARTUP SEQUENCE" << std::endl;
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
        
        std::cout << "[Loader] Step 1/2: Waiting for CS2 to launch..." << std::endl;
        DWORD cs2PID = WaitForProcess("cs2.exe");
        
        if (cs2PID == 0)
        {
            std::cerr << "[Loader] ERROR: CS2 not found!" << std::endl;
            exit(1);
        }
        
        std::cout << "[Loader] Step 2/2: Waiting for Steam..." << std::endl;
        DWORD steamPID = WaitForProcess("steam.exe", 30);
        
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
        std::cout << "[Loader] ✓ Ready! Starting skin changer..." << std::endl;
        std::cout << "════════════════════════════════════════════════════════════" << std::endl;
    }
}

// In main() - add at very beginning
int main()
{
    ProcessDetection::WaitForBothCS2AndSteam();
    
    // ... rest of existing code ...
}
```

---

## 🚀 Next Steps

1. **Add the code** (5 minutes)
2. **Rebuild** (2 minutes)
3. **Test**:
   - Start tool without CS2
   - Tool should wait
   - Start CS2
   - Tool should initialize
4. **Done!**

---

## ✅ What This Enables

- ✅ Tool waits for CS2 before initializing
- ✅ Cleaner startup sequence
- ✅ Better error handling
- ✅ User-friendly messages
- ✅ Foundation for future enhancements

---

## 📞 Questions?

See related files:
- `STEAM_OVERLAY_INTEGRATION_GUIDE.md` - Full overlay integration guide
- `KNIFE_FIX_QUICK_REFERENCE.txt` - Knife fix reference
- `BUILD_AND_TEST_KNIFE_FIX.md` - Build instructions

---

**Ready to add process detection?** Copy the code and rebuild! 🚀
