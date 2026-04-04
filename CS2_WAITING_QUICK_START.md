# 🎮 Quick Implementation - CS2 Waiting State

## 📋 Simple Steps

### Step 1: Add to Top of menu.h (After line 12)

```cpp
// State management for CS2 waiting
enum class AppState {
    WAITING_FOR_CS2,
    LOADING,
    READY
};

static AppState currentAppState = AppState::WAITING_FOR_CS2;
static bool showBetaWarning = false;
static float loadingProgress = 0.0f;
static auto loadingStartTime = std::chrono::high_resolution_clock::now();
```

---

### Step 2: Add Process Detection (Before OnFrame() in menu.h)

```cpp
#include <tlhelp32.h>

bool IsCS2Running()
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) return false;
    
    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
    
    if (Process32First(hProcessSnap, &pe32))
    {
        do {
            if (_stricmp(pe32.szExeFile, "cs2.exe") == 0)
            {
                CloseHandle(hProcessSnap);
                return true;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }
    
    CloseHandle(hProcessSnap);
    return false;
}
```

---

### Step 3: Add Waiting Screen Renderer

```cpp
void RenderWaitingScreen(float x, float y, float w, float h)
{
    // Background
    SC_GUI::DrawRect(x, y, w, h, ImColor(20, 20, 20, 200));
    SC_GUI::DrawBorder(x, y, w, h, ImColor(100, 100, 100, 255), 2.0f);
    
    // Center text
    float centerX = x + w / 2;
    float centerY = y + h / 2;
    
    ImVec2 textSize = ImGui::CalcTextSize("Waiting for CS2...");
    SC_GUI::DrawText("Waiting for CS2...", 
        centerX - textSize.x / 2, centerY - 50,
        ImColor(255, 200, 0, 255), 18.0f);
    
    ImVec2 instrSize = ImGui::CalcTextSize("Please launch Counter-Strike 2");
    SC_GUI::DrawText("Please launch Counter-Strike 2",
        centerX - instrSize.x / 2, centerY,
        ImColor(200, 200, 200, 200), 14.0f);
}
```

---

### Step 4: Add Loading Screen Renderer

```cpp
void RenderLoadingScreen(float x, float y, float w, float h)
{
    // Background
    SC_GUI::DrawRect(x, y, w, h, ImColor(25, 25, 25, 220));
    SC_GUI::DrawBorder(x, y, w, h, ImColor(100, 100, 100, 255), 2.0f);
    
    float centerX = x + w / 2;
    float centerY = y + h / 2;
    
    // Title
    ImVec2 titleSize = ImGui::CalcTextSize("Initializing...");
    SC_GUI::DrawText("Initializing...",
        centerX - titleSize.x / 2, centerY - 60,
        ImColor(100, 180, 255, 255), 18.0f);
    
    // Progress bar
    float barW = 300.0f, barH = 20.0f;
    float barX = centerX - barW / 2;
    float barY = centerY;
    
    SC_GUI::DrawRect(barX, barY, barW, barH, ImColor(50, 50, 50, 200));
    SC_GUI::DrawRect(barX + 2, barY + 2, (barW - 4) * (loadingProgress / 100.0f), barH - 4,
        ImColor(100, 180, 255, 255));
    SC_GUI::DrawBorder(barX, barY, barW, barH, ImColor(100, 150, 200, 255), 1.5f);
    
    // Status
    SC_GUI::DrawText("Loading weapon skins...",
        centerX - ImGui::CalcTextSize("Loading weapon skins...").x / 2,
        centerY + 50, ImColor(150, 150, 150, 200), 12.0f);
}
```

---

### Step 5: Add Beta Warning

```cpp
void RenderBetaWarning(float x, float y, float w, float h)
{
    if (!showBetaWarning) return;
    
    // Overlay
    SC_GUI::DrawRect(x, y, w, h, ImColor(0, 0, 0, 150));
    
    // Warning box
    float boxW = 400.0f, boxH = 200.0f;
    float boxX = x + (w - boxW) / 2;
    float boxY = y + (h - boxH) / 2;
    
    SC_GUI::DrawRect(boxX, boxY, boxW, boxH, ImColor(40, 40, 40, 255));
    SC_GUI::DrawBorder(boxX, boxY, boxW, boxH, ImColor(255, 200, 0, 255), 3.0f);
    
    // Title
    ImVec2 titleSize = ImGui::CalcTextSize("⚠ BETA WARNING");
    SC_GUI::DrawText("⚠ BETA WARNING",
        boxX + (boxW - titleSize.x) / 2, boxY + 20,
        ImColor(255, 200, 0, 255), 16.0f);
    
    // Message
    SC_GUI::DrawText("This feature may not work reliably.",
        boxX + 20, boxY + 60, ImColor(200, 200, 200, 255), 13.0f);
    
    // OK Button
    float buttonW = 100.0f, buttonH = 35.0f;
    float buttonX = boxX + (boxW - buttonW) / 2;
    float buttonY = boxY + boxH - 50;
    
    if (SC_GUI::Button("OK", buttonX, buttonY, buttonW, buttonH))
    {
        showBetaWarning = false;
    }
}
```

---

### Step 6: Replace OnFrame() Function

Find the existing `void OnFrame()` in menu.h and replace it with:

```cpp
void OnFrame()
{
    // Check CS2 status
    bool cs2Running = IsCS2Running();
    
    // State transitions
    if (!cs2Running && currentAppState != AppState::WAITING_FOR_CS2)
    {
        currentAppState = AppState::WAITING_FOR_CS2;
    }
    else if (cs2Running && currentAppState == AppState::WAITING_FOR_CS2)
    {
        currentAppState = AppState::LOADING;
        loadingStartTime = std::chrono::high_resolution_clock::now();
        loadingProgress = 0.0f;
    }
    
    // Update loading progress
    if (currentAppState == AppState::LOADING)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - loadingStartTime).count();
        loadingProgress = std::min(100.0f, (float)elapsed / 2000.0f * 100.0f);
        
        if (elapsed > 2000)
        {
            currentAppState = AppState::READY;
        }
    }
    
    float x = 0, y = 0, w = 1920, h = 1080;  // Adjust to your window size
    
    // Render based on state
    if (currentAppState == AppState::WAITING_FOR_CS2)
    {
        RenderWaitingScreen(x, y, w, h);
        return;
    }
    else if (currentAppState == AppState::LOADING)
    {
        RenderLoadingScreen(x, y, w, h);
        return;
    }
    
    // Render beta warning on top
    RenderBetaWarning(x, y, w, h);
    
    // ... rest of your existing OnFrame() code here ...
    // (Keep all the normal menu rendering code that was there before)
}
```

---

### Step 7: Add Beta Warnings to Tabs

#### For Knives Tab (Add at start of knives rendering):

```cpp
static bool warnedKnives = false;
if (!warnedKnives && currentAppState == AppState::READY)
{
    warnedKnives = true;
    showBetaWarning = true;
}
```

#### For Gloves Tab (Add at start of gloves rendering):

```cpp
static bool warnedGloves = false;
if (!warnedGloves && currentAppState == AppState::READY)
{
    warnedGloves = true;
    showBetaWarning = true;
}
```

---

## 🧪 Testing Checklist

- [ ] Build solution (should have no errors)
- [ ] Run without CS2 → See "Waiting for CS2"
- [ ] Start CS2 → See loading bar
- [ ] Wait 2 seconds → See normal menu
- [ ] Click Knives → See beta warning
- [ ] Click OK → Access knives
- [ ] Click Gloves → See beta warning

---

## ✅ What You Get

✓ App runs without CS2  
✓ Shows waiting message  
✓ Shows loading bar after CS2 starts  
✓ Returns to waiting if CS2 closes  
✓ Beta warnings for experimental features  
✓ All consistent with menu design  
✓ Everything on same overlay  

---

**Ready to implement! Just copy and paste the code into menu.h**
