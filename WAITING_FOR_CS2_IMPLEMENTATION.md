# 🎮 Skinchanger App - Waiting for CS2 + Loading Screen Implementation

## 📋 What's Needed

Your request:
- ✅ App loads even if CS2 is not running
- ✅ Shows "Waiting for CS2" message in menu center
- ✅ Plain window with no inventory UI
- ✅ When CS2 loads, show loading screen
- ✅ Add "may not work, beta" warnings for knives/gloves
- ✅ Everything in same menu design/overlay
- ✅ All displays on same overlay, not separate windows

---

## 🔧 Implementation Strategy

### State Management

Add to top of `menu.h`:

```cpp
// Waiting state for CS2
enum class AppState {
    WAITING_FOR_CS2,
    LOADING,
    READY
};

static AppState currentAppState = AppState::WAITING_FOR_CS2;
static bool showBetaWarning = false;
static std::string betaWarningMessage = "";
static float loadingProgress = 0.0f;
static std::chrono::high_resolution_clock::time_point loadingStartTime;
```

### Process Detection

Add to `menu.h` before `OnFrame()`:

```cpp
// Check if CS2 is running
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

// Initialize when CS2 is detected
void InitializeOnCS2Start()
{
    if (currentAppState == AppState::LOADING)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - loadingStartTime).count();
        
        loadingProgress = std::min(100.0f, (float)elapsed / 2000.0f * 100.0f);
        
        if (elapsed > 2000)
        {
            currentAppState = AppState::READY;
            loadingProgress = 100.0f;
        }
    }
}
```

---

## 🎨 UI Rendering Functions

### Waiting Screen

Add to `menu.h`:

```cpp
void RenderWaitingScreen(float x, float y, float w, float h)
{
    // Semi-transparent dark background
    SC_GUI::DrawRect(x, y, w, h, ImColor(20, 20, 20, 200));
    
    // Border
    SC_GUI::DrawBorder(x, y, w, h, ImColor(100, 100, 100, 255), 2.0f);
    
    // Center text
    float centerX = x + w / 2;
    float centerY = y + h / 2;
    
    // Main message
    ImVec2 textSize = ImGui::CalcTextSize("Waiting for CS2...");
    SC_GUI::DrawText("Waiting for CS2...", 
        centerX - textSize.x / 2, 
        centerY - 50,
        ImColor(255, 200, 0, 255),  // Yellow/Gold
        18.0f);
    
    // Instructions
    ImVec2 instrSize = ImGui::CalcTextSize("Please launch Counter-Strike 2");
    SC_GUI::DrawText("Please launch Counter-Strike 2",
        centerX - instrSize.x / 2,
        centerY,
        ImColor(200, 200, 200, 200),
        14.0f);
    
    // Animated dots
    static float dotAnimation = 0.0f;
    dotAnimation += 0.05f;
    if (dotAnimation > 3.0f) dotAnimation = 0.0f;
    
    std::string dots = "";
    for (int i = 0; i < (int)dotAnimation; i++) dots += ".";
    
    ImVec2 dotsSize = ImGui::CalcTextSize(dots.c_str());
    SC_GUI::DrawText(dots,
        centerX - dotsSize.x / 2,
        centerY + 40,
        ImColor(150, 150, 150, 200),
        14.0f);
}
```

### Loading Screen

Add to `menu.h`:

```cpp
void RenderLoadingScreen(float x, float y, float w, float h)
{
    // Dark background
    SC_GUI::DrawRect(x, y, w, h, ImColor(25, 25, 25, 220));
    
    // Border
    SC_GUI::DrawBorder(x, y, w, h, ImColor(100, 100, 100, 255), 2.0f);
    
    float centerX = x + w / 2;
    float centerY = y + h / 2;
    
    // Loading title
    ImVec2 titleSize = ImGui::CalcTextSize("Initializing...");
    SC_GUI::DrawText("Initializing...",
        centerX - titleSize.x / 2,
        centerY - 60,
        ImColor(100, 180, 255, 255),  // Blue
        18.0f);
    
    // Progress bar
    float progressBarW = 300.0f;
    float progressBarH = 20.0f;
    float progressBarX = centerX - progressBarW / 2;
    float progressBarY = centerY;
    
    // Background
    SC_GUI::DrawRect(progressBarX, progressBarY, progressBarW, progressBarH, 
        ImColor(50, 50, 50, 200));
    
    // Progress fill
    float fillW = (progressBarW - 4) * (loadingProgress / 100.0f);
    SC_GUI::DrawRect(progressBarX + 2, progressBarY + 2, fillW, progressBarH - 4,
        ImColor(100, 180, 255, 255));
    
    // Border
    SC_GUI::DrawBorder(progressBarX, progressBarY, progressBarW, progressBarH,
        ImColor(100, 150, 200, 255), 1.5f);
    
    // Percentage text
    std::string percentText = std::to_string((int)loadingProgress) + "%";
    ImVec2 percentSize = ImGui::CalcTextSize(percentText.c_str());
    SC_GUI::DrawText(percentText,
        centerX - percentSize.x / 2,
        progressBarY + progressBarH / 2 - percentSize.y / 2,
        ImColor(255, 255, 255, 255),
        12.0f);
    
    // Status text
    SC_GUI::DrawText("Loading weapon skins and configurations...",
        centerX - ImGui::CalcTextSize("Loading weapon skins and configurations...").x / 2,
        centerY + 50,
        ImColor(150, 150, 150, 200),
        12.0f);
}
```

### Beta Warning Dialog

Add to `menu.h`:

```cpp
void RenderBetaWarning(float x, float y, float w, float h)
{
    if (!showBetaWarning) return;
    
    // Semi-transparent overlay (blocks interaction with menu)
    SC_GUI::DrawRect(x, y, w, h, ImColor(0, 0, 0, 150));
    
    // Warning box
    float boxW = 400.0f;
    float boxH = 200.0f;
    float boxX = x + (w - boxW) / 2;
    float boxY = y + (h - boxH) / 2;
    
    // Background
    SC_GUI::DrawRect(boxX, boxY, boxW, boxH, ImColor(40, 40, 40, 255));
    
    // Border (yellow for warning)
    SC_GUI::DrawBorder(boxX, boxY, boxW, boxH, ImColor(255, 200, 0, 255), 3.0f);
    
    // Title
    ImVec2 titleSize = ImGui::CalcTextSize("⚠ BETA WARNING");
    SC_GUI::DrawText("⚠ BETA WARNING",
        boxX + (boxW - titleSize.x) / 2,
        boxY + 20,
        ImColor(255, 200, 0, 255),
        16.0f);
    
    // Message
    SC_GUI::DrawText("This feature may not work reliably.",
        boxX + 20,
        boxY + 60,
        ImColor(200, 200, 200, 255),
        13.0f);
    
    SC_GUI::DrawText("Use at your own risk.",
        boxX + 20,
        boxY + 85,
        ImColor(200, 200, 200, 255),
        13.0f);
    
    // OK Button
    float buttonW = 100.0f;
    float buttonH = 35.0f;
    float buttonX = boxX + (boxW - buttonW) / 2;
    float buttonY = boxY + boxH - 50;
    
    bool buttonClicked = SC_GUI::Button("OK", buttonX, buttonY, buttonW, buttonH);
    
    if (buttonClicked)
    {
        showBetaWarning = false;
    }
}
```

---

## 📝 Modified OnFrame() Function

Replace the existing `OnFrame()` in menu.h with:

```cpp
void OnFrame()
{
    // Check CS2 status
    bool cs2Running = IsCS2Running();
    
    // State machine
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
        InitializeOnCS2Start();
    }
    
    // Get window dimensions
    float x = 0, y = 0, w = 1920, h = 1080;  // Adjust as needed
    
    // Render based on state
    if (currentAppState == AppState::WAITING_FOR_CS2)
    {
        RenderWaitingScreen(x, y, w, h);
        return;  // Don't render menu yet
    }
    else if (currentAppState == AppState::LOADING)
    {
        RenderLoadingScreen(x, y, w, h);
        return;  // Don't render menu yet
    }
    
    // Normal menu rendering (when READY)
    // ... existing menu code ...
}
```

---

## 🚨 Beta Warnings for Knives/Gloves

### In Knives Tab Handler

Add before rendering knives tab:

```cpp
void RenderKnivesTab(float x, float y, float w, float h)
{
    // Show beta warning when this tab is accessed
    static bool showedKnifeWarning = false;
    if (!showedKnifeWarning)
    {
        showBetaWarning = true;
        betaWarningMessage = "Knife changing may not work reliably.";
        showedKnifeWarning = true;
    }
    
    // ... rest of knives rendering code ...
}
```

### In Gloves Tab Handler

Add before rendering gloves tab:

```cpp
void RenderGlovesTab(float x, float y, float w, float h)
{
    // Show beta warning when this tab is accessed
    static bool showedGloveWarning = false;
    if (!showedGloveWarning)
    {
        showBetaWarning = true;
        betaWarningMessage = "Glove changing may not work reliably.";
        showedGloveWarning = true;
    }
    
    // ... rest of gloves rendering code ...
}
```

---

## ✅ Implementation Checklist

### Step 1: Add State Management
- [ ] Add enum and static variables to menu.h (after line 12)

### Step 2: Add Helper Functions
- [ ] Add `IsCS2Running()` function
- [ ] Add `InitializeOnCS2Start()` function

### Step 3: Add UI Rendering
- [ ] Add `RenderWaitingScreen()` function
- [ ] Add `RenderLoadingScreen()` function
- [ ] Add `RenderBetaWarning()` function

### Step 4: Modify OnFrame()
- [ ] Replace existing OnFrame() with new state-machine version
- [ ] Ensure it calls correct rendering based on state

### Step 5: Add Beta Warnings
- [ ] Add warning checks to knives tab
- [ ] Add warning checks to gloves tab

### Step 6: Test
- [ ] Build solution
- [ ] Run without CS2 → should show "Waiting"
- [ ] Start CS2 → should show loading
- [ ] Wait for loading → should show menu
- [ ] Click knives tab → should show warning

---

## 🎨 Design Consistency

All new UI elements use:
- ✅ Same color scheme as existing menu
- ✅ Same fonts/sizes
- ✅ Same styling (borders, transparency)
- ✅ Same overlay positioning
- ✅ No separate windows

---

## 💻 Key Code Sections

### Process Detection
```cpp
bool IsCS2Running()
```
Checks if CS2.exe is in process list

### State Management
```cpp
enum class AppState { WAITING_FOR_CS2, LOADING, READY }
```
Controls what UI is displayed

### Waiting Screen
```cpp
void RenderWaitingScreen()
```
Displayed until CS2 launches

### Loading Screen
```cpp
void RenderLoadingScreen()
```
Displayed after CS2 launches, while initializing

### Beta Warnings
```cpp
void RenderBetaWarning()
```
Modal dialog for knives/gloves tabs

---

## 🔍 Files to Modify

**Main file**: `src/ext-cs2-skin-changer/src/menu.h`

**Changes**:
1. Add state enum and variables
2. Add process detection function
3. Add rendering functions
4. Modify OnFrame()
5. Add beta warning checks

---

## ⏱️ Implementation Time

- State setup: 5 min
- Process detection: 5 min
- Waiting screen: 10 min
- Loading screen: 15 min
- Beta warnings: 5 min
- Testing: 10 min

**Total: ~50 minutes**

---

## 🎯 Expected Result

### Workflow 1: App Before CS2
1. User runs skinchanger.exe
2. App opens with "Waiting for CS2" message
3. User launches CS2
4. App shows loading screen (2 seconds)
5. App shows normal menu

### Workflow 2: Accessing Beta Features
1. User clicks on Knives tab
2. Beta warning appears
3. User clicks OK
4. Knives menu appears

### Workflow 3: After CS2 Closes
1. If user closes CS2
2. App returns to "Waiting for CS2" state
3. Waits for next CS2 launch

---

## ✨ Summary

This implementation provides:
- ✅ App runs without CS2
- ✅ Clear "Waiting" state
- ✅ Loading screen while initializing
- ✅ Beta warnings for experimental features
- ✅ All in consistent menu design
- ✅ Everything on same overlay

Ready to implement!
