# Runtime Fixes Applied - GUI Launch Failure Resolution

## Problem Identified
Build succeeded but GUI didn't launch:
1. When CS2 not running: Terminal didn't launch
2. When CS2 running: Terminal launched then immediately closed
3. No overlay window appeared in either case

## Root Causes Found

### Issue 1: Game Memory Access Before CS2 Check
**File:** `src/ext-cs2-skin-changer/src/main.cpp` (lines 71-79)
**Problem:** The main loop tried to read game memory immediately without checking if CS2 was running:
```cpp
const uintptr_t localController = GetLocalController();  // Crashes if CS2 not running!
const uintptr_t inventoryServices = mem.Read<uintptr_t>(localController + ...);
```

**Fix Applied:**
```cpp
// Call OnFrame first for overlay rendering
OnFrame();

// Guard game memory access with null checks
const uintptr_t localController = GetLocalController();
if (!localController) continue;  // CS2 not running, skip to next frame

const uintptr_t inventoryServices = mem.Read<uintptr_t>(localController + ...);
const uintptr_t localPlayer = GetLocalPlayer();
if (!localPlayer) continue;  // No local player, skip
```

**Rationale:** Frame rendering must happen regardless of CS2 status. Game memory operations are guarded with null checks.

---

### Issue 2: Null Callback to overlay::Render()
**File:** `src/ext-cs2-skin-changer/src/menu.h` (lines 787, 792)
**Problem:** The waiting state and loading screens called `overlay::Render(nullptr, true)`:
```cpp
void Render(void (*RenderCallback)(), bool visible) {
    // ...expects a valid function pointer, not null!
}

// Calling with null:
overlay::Render(nullptr, true);  // Crashes when trying to call nullptr!
```

**Fix Applied:**
Created wrapper functions that can be passed as valid callbacks:

```cpp
void RenderWaitingScreenCallback()
{
    RenderWaitingScreen(0.0f, 0.0f, 
        static_cast<float>(overlay::G_Width), 
        static_cast<float>(overlay::G_Height));
}

void RenderLoadingScreenCallback()
{
    RenderLoadingScreen(0.0f, 0.0f, 
        static_cast<float>(overlay::G_Width), 
        static_cast<float>(overlay::G_Height));
}

// Now call with valid callbacks:
if (currentAppState == AppState::WAITING_FOR_CS2) {
    overlay::Render(RenderWaitingScreenCallback, true);
    return;
}
else if (currentAppState == AppState::LOADING) {
    overlay::Render(RenderLoadingScreenCallback, true);
    return;
}
```

**Rationale:** The overlay rendering system requires valid function pointers. Wrapper functions act as adapters to call the render functions with correct parameters.

---

## Files Modified

### 1. `src/ext-cs2-skin-changer/src/main.cpp`
- **Lines 67-86:** Restructured main loop to:
  1. Call `OnFrame()` first (always, even if CS2 not running)
  2. Guard all game memory access with null checks
  3. Skip game processing frames when CS2 not ready

### 2. `src/ext-cs2-skin-changer/src/menu.h`
- **Lines 754-763:** Added two wrapper callback functions
- **Lines 792-798:** Updated OnFrame to use callback wrappers instead of nullptr
- **Removed:** Lambda expressions (not supported by this codebase)

---

## Expected Behavior After Fixes

### When CS2 Not Running
1. ✅ App launches successfully
2. ✅ Terminal shows: "SkinChanger Started"
3. ✅ Overlay window appears with "Waiting for CS2..." message centered
4. ✅ App continues running, waiting for CS2 to launch

### When CS2 Launches
1. ✅ App detects CS2 process (via IsCS2Running())
2. ✅ Overlay transitions to LOADING state
3. ✅ 2-second loading progress bar displays
4. ✅ After 2 seconds, transitions to READY state
5. ✅ Normal skinchanger menu appears with INSERT key toggle

### When Clicking Knives/Gloves Tabs
1. ✅ Beta warning modal appears with "BETA WARNING" title
2. ✅ Message: "This feature may not work reliably."
3. ✅ OK button to dismiss
4. ✅ Warning is styled to match overlay theme (dark background, yellow text)

### When CS2 Closes
1. ✅ App detects CS2 process ended
2. ✅ Returns to WAITING_FOR_CS2 state
3. ✅ Shows "Waiting for CS2..." message again
4. ✅ Waits for CS2 to relaunch

---

## Technical Details

### State Machine Flow
```
START
  ↓
OnFrame() checks IsCS2Running()
  ↓
NOT RUNNING → WAITING_FOR_CS2 state
  ├→ RenderWaitingScreenCallback() displays "Waiting for CS2..."
  ├→ overlay::Render() draws overlay
  └→ Loop continues (OnFrame still called)

CS2 DETECTED → LOADING state
  ├→ RenderLoadingScreenCallback() displays loading bar
  ├→ Progress animates from 0-100% over 2 seconds
  ├→ overlay::Render() draws overlay
  └→ After 2 seconds → READY state

READY state
  ├→ Normal menu rendering via RenderMenu()
  ├→ User can toggle menu with INSERT key
  ├→ Game memory operations execute
  └→ Beta warnings show when needed

CS2 CLOSES → Back to WAITING_FOR_CS2 state
```

### Memory Safety
- **Before fix:** Crash when reading from null pointers before CS2 check
- **After fix:** Guard clauses prevent dereferencing invalid pointers:
  ```cpp
  const uintptr_t localController = GetLocalController();
  if (!localController) continue;  // Safe!
  ```

### Overlay Rendering
- **Before fix:** Null function pointer crash when calling overlay::Render(nullptr)
- **After fix:** Valid callback functions provided:
  ```cpp
  overlay::Render(RenderWaitingScreenCallback, true);  // Safe!
  ```

---

## Build Command
```cmd
cd C:\Users\qveezzx\Documents\Changer
REBUILD_NOW.bat
```

## Expected Build Result
```
✅ BUILD SUCCESSFUL
0 errors
1 warning (C4244 - non-critical)
```

## Expected Runtime Result
```
✅ App launches with overlay visible
✅ Shows "Waiting for CS2..." when CS2 not running
✅ Transitions to loading screen when CS2 launches
✅ Shows normal menu when ready
✅ Returns to waiting when CS2 closes
```

---

**Status:** ✅ Ready for testing
**Build:** Should complete with 0 errors
**Runtime:** Should launch GUI properly now
