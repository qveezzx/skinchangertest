#pragma once
#include "SDK/weapon/C_EconEntity.h"
#include "../ext/skindb.h"
#include "window/window.hpp"
#include "ui_engine.hpp"
#include "config.h"
#include <curl/curl.h>

static WeaponsEnum CurrentWeaponDef;

// === UPDATE STATUS ===
static std::string updateStatus = "CHECKING"; // CHECKING, UPDATED, OUTDATED
static ULONGLONG lastUpdateCheck = 0;
static const ULONGLONG UPDATE_CHECK_INTERVAL = 60000; // Check every 60 seconds

void CheckForUpdates() {
    ULONGLONG now = GetTickCount64();
    if (now - lastUpdateCheck < UPDATE_CHECK_INTERVAL) return;
    lastUpdateCheck = now;

    std::thread([]() {
        CURL* curl = curl_easy_init();
        if (!curl) return;

        std::string response;
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Cache-Control: no-cache");
        headers = curl_slist_append(headers, "Pragma: no-cache");

        curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/qveezzx/skinchangertest/refs/heads/main/isupdated");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
            size_t realsize = size * nmemb;
            ((std::string*)userp)->append((char*)contents, realsize);
            return realsize;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK && !response.empty()) {
            // Get first line only
            size_t newlinePos = response.find('\n');
            std::string firstLine = (newlinePos != std::string::npos)
                ? response.substr(0, newlinePos)
                : response;

            // Trim whitespace
            firstLine.erase(0, firstLine.find_first_not_of(" \t\r\n"));
            firstLine.erase(firstLine.find_last_not_of(" \t\r\n") + 1);

            // Convert to uppercase for comparison
            std::string upperLine = firstLine;
            std::transform(upperLine.begin(), upperLine.end(), upperLine.begin(), ::toupper);

            if (upperLine.find("UPDATED") != std::string::npos) {
                updateStatus = "UPDATED";
            } else if (upperLine.find("OUTDATED") != std::string::npos) {
                updateStatus = "OUTDATED";
            }
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }).detach();
}
static int selectedSkinIndex = 0;
static int selectedKnifeIndex = 0;
static int selectedKnifeSkinIndex = 0;
static int selectedMusicKitIndex = 0;
static char searchBuffer[128] = "";

// === STATE MANAGEMENT ===
enum LoadingPhase {
    PHASE_WAITING_CS2,      // Waiting for CS2 to launch (static)
    PHASE_INDEXING_SKINS,   // CS2 detected, indexing skins (5 second loading)
    PHASE_MAIN_MENU         // Main menu ready
};

static LoadingPhase currentPhase = PHASE_INDEXING_SKINS;  // Start with indexing since CS2 is already detected
static ULONGLONG indexingStartTime = 0;
static const ULONGLONG INDEXING_DURATION = 5000; // 5 seconds

// Skin change loading state
static bool isChangingSkin = false;
static ULONGLONG skinChangeStartTime = 0;

// Menu bounds (updated every frame for loading box positioning)
static float menuX = -1;
static float menuY = -1;
static float menuW = 1000;
static float menuH = 700;

void UpdateSearchInput() {
}

void RenderWeaponTab(float x, float y, float w, float h)
{
    // Search Box
    SC_GUI::TextInput("search_wep", searchBuffer, 128, x + 20, y + 20, 300, 35, "Search Skins...");
    
    // Scroll State
    static float scrollY = 0.0f;
    float contentHeight = 0.0f;

    // Viewport
    float viewX = x;
    float viewY = y + 70;
    float viewW = w;
    float viewH = h - 70;

    // Grid Settings
    float itemW = 140; float itemH = 160; float pad = 12;
    int cols = (int)((viewW - 16) / (itemW + pad));
    if (cols < 1) cols = 1;

    // Handle Input
    if (SC_GUI::Input.mousePos.x >= viewX && SC_GUI::Input.mousePos.x <= viewX + viewW &&
        SC_GUI::Input.mousePos.y >= viewY && SC_GUI::Input.mousePos.y <= viewY + viewH) {
         scrollY += SC_GUI::Input.scrollDelta * 0.5f; // Scale speed
    }

    // Prepare content
    std::vector<SkinInfo_t> availableSkins = skindb->GetWeaponSkins(CurrentWeaponDef);
    
    // SORT BY RARITY (Descending)
    std::sort(availableSkins.begin(), availableSkins.end(), [](const SkinInfo_t& a, const SkinInfo_t& b) {
        return a.rarity > b.rarity;
    });

    std::string query = searchBuffer;
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);

    // Filter First to get total count
    std::vector<int> filteredIndices;
    for (int i = 0; i < (int)availableSkins.size(); i++) {
        if (!query.empty()) {
             std::string name = availableSkins[i].name;
             std::transform(name.begin(), name.end(), name.begin(), ::tolower);
             if (name.find(query) == std::string::npos) continue;
        }
        filteredIndices.push_back(i);
    }
    
    // Calculate Content Height
    int rows = ((int)filteredIndices.size() + cols - 1) / cols;
    contentHeight = rows * (itemH + pad);

    // Clamp Scroll
    float maxScroll = 0.0f;
    float minScroll = -(contentHeight - viewH + pad);
    if (minScroll > 0) minScroll = 0;
    if (scrollY > maxScroll) scrollY = maxScroll;
    if (scrollY < minScroll) scrollY = minScroll;

    // Determine currently selected skin index
    int selectedSkinIndex = 0;
    SkinInfo_t currentSkin = skinManager->GetSkin(CurrentWeaponDef);
    if (currentSkin.weaponType != WeaponsEnum::none) {
        for (int i = 0; i < (int)availableSkins.size(); i++) {
            if (availableSkins[i].Paint == currentSkin.Paint) {
                selectedSkinIndex = i;
                break;
            }
        }
    }

    // Draw With Clip
    SC_GUI::SetClip(viewX, viewY, viewW, viewH);

    int displayIdx = 0;
    for (int idx : filteredIndices) {
        // Calculate Pos
        float cX = viewX + 20 + (displayIdx % cols) * (itemW + pad);
        float cY = viewY + scrollY + (displayIdx / cols) * (itemH + pad);
        
        // Visibility Check (Culling)
        if (cY + itemH < viewY || cY > viewY + viewH) {
             displayIdx++; continue;
        }

        bool selected = (selectedSkinIndex == idx);
        
        if (SC_GUI::SkinCard("wskin_" + availableSkins[idx].name, availableSkins[idx].name, availableSkins[idx].image_url, availableSkins[idx].rarity, cX, cY, itemW, itemH, selected, configManager->diskCacheEnabled)) {
            // Start loading state
            isChangingSkin = true;
            skinChangeStartTime = GetTickCount64();
            
            if (idx != 0) skinManager->AddSkin(availableSkins[idx]);
            else {
                skinManager->AddSkin(availableSkins[idx]); 
            }
        }
        displayIdx++;
    }

    SC_GUI::ResetClip();
    
    // Scrollbar (Visual Only)
    if (contentHeight > viewH) {
        float ratio = viewH / contentHeight;
        float barH = viewH * ratio;
        float barY = viewY + (-scrollY / contentHeight) * viewH;
        SC_GUI::DrawRoundedRect(x + w - 8, barY, 4, barH, 2, Color(255, 100, 100, 100));
    }
}

void RenderKnifeTab(float x, float y, float w, float h)
{
     // Smart Auto-Detect Logic
    bool isDefaultKnife = true;
    int autoDetectedIndex = -1;
    auto it = KnifeNames.find(static_cast<uint16_t>(CurrentWeaponDef));
    if (it != KnifeNames.end()) {
        std::string knifeName = it->second;
        for (int i = 0; i < (int)Knifes.size(); i++) {
            if (Knifes[i].name.find(knifeName) != std::string::npos) {
                autoDetectedIndex = i; break;
            }
        }
    }
    if (autoDetectedIndex != -1) {
        if (selectedKnifeIndex != autoDetectedIndex) {
            selectedKnifeIndex = autoDetectedIndex;
            selectedKnifeSkinIndex = 0;
        }
        isDefaultKnife = false;
    }

    float curY = y + 20;

    // Model Selector (Only if Default)
    if (isDefaultKnife) {
        SC_GUI::DrawStringA("Select Knife Type (Scroll to see more):", x + 20, curY, Color::White, SC_GUI::mainFont, false);
        curY += 25;
        
        // Horizontal Scroll for Knives
        static float kScrollX = 0.0f;
        SC_GUI::SetClip(x, curY, w, 35);
        if (SC_GUI::Input.mousePos.y >= curY && SC_GUI::Input.mousePos.y <= curY + 35) {
             kScrollX += SC_GUI::Input.scrollDelta * 0.5f;
        }
        float kContentW = Knifes.size() * 85.0f;
        if (kScrollX > 0) kScrollX = 0;
        if (kScrollX < -(kContentW - w)) kScrollX = -(kContentW - w);

        for(int k=0; k<(int)Knifes.size(); k++) {
             if (SC_GUI::Button("k_model_" + std::to_string(k), Knifes[k].name, x + 20 + kScrollX + (k*85), curY, 80, 25, selectedKnifeIndex == k)) {
                 selectedKnifeIndex = k;
                 selectedKnifeSkinIndex = 0;
             }
        }
        SC_GUI::ResetClip();
        curY += 40;
    } else {
        SC_GUI::DrawStringA("Detected: " + Knifes[selectedKnifeIndex].name, x + 20, curY, Color(255, 100, 255, 100), SC_GUI::titleFont, false);
        curY += 40;
    }

    // Grid (Scrollable)
    std::string FilterName = Knifes[selectedKnifeIndex].name;
    const std::vector<SkinInfo_t>& allKnifeSkins = skindb->GetKnifeSkins();
    std::vector<SkinInfo_t> filteredSkins;
    filteredSkins.push_back(SkinInfo_t{ 0, false, "Vanilla", WeaponsEnum::none, 1 }); // Default rarity
    
    for (const auto& skin : allKnifeSkins) {
        if (skin.name.find(FilterName) == std::string::npos) continue;
        filteredSkins.push_back(skin);
    }
    
    // Sort Knife Skins
    std::sort(filteredSkins.begin(), filteredSkins.end(), [](const SkinInfo_t& a, const SkinInfo_t& b) {
        return a.rarity > b.rarity;
    });
    
    // Draw Grid with Scroll
    static float kSkinScrollY = 0.0f;
    float viewY = curY;
    float viewH = (y + h) - viewY;
    float itemW = 140; float itemH = 160; float pad = 15;
    int cols = 6;
    
    // Input for Knife Grid
    if (SC_GUI::Input.mousePos.y > viewY) {
         kSkinScrollY += SC_GUI::Input.scrollDelta * 0.5f;
    }
    
    int rows = ((int)filteredSkins.size() + cols - 1) / cols;
    float contentH = rows * (itemH + pad);
    if (kSkinScrollY > 0) kSkinScrollY = 0;
    if (kSkinScrollY < -(contentH - viewH + pad)) kSkinScrollY = -(contentH - viewH + pad);

    SC_GUI::SetClip(x, viewY, w, viewH);
    int displayIdx = 0;
    for (int i = 0; i < (int)filteredSkins.size(); i++) {
         float cX = x + 20 + (displayIdx % cols) * (itemW + pad);
         float cY = viewY + kSkinScrollY + (displayIdx / cols) * (itemH + pad);
         
         // Cull
         if (cY + itemH < viewY || cY > viewY + viewH) { displayIdx++; continue; }

         bool selected = (selectedKnifeSkinIndex == i);
         if (SC_GUI::SkinCard("kskin_" + filteredSkins[i].name, filteredSkins[i].name, filteredSkins[i].image_url, filteredSkins[i].rarity, cX, cY, itemW, itemH, selected, configManager->diskCacheEnabled)) {
             selectedKnifeSkinIndex = i;
             if (i!=0) {
                 // Start loading state
                 isChangingSkin = true;
                 skinChangeStartTime = GetTickCount64();
                 
                 SkinInfo_t s = filteredSkins[i];
                 s.weaponType = WeaponsEnum::CtKnife; skinManager->AddSkin(s);
                 s.weaponType = WeaponsEnum::Tknife; skinManager->AddSkin(s);
             }
         }
         displayIdx++;
    }
    SC_GUI::ResetClip();
}

void RenderMusicTab(float x, float y, float w, float h)
{
    // Search/Filter
    SC_GUI::TextInput("search_music", searchBuffer, 128, x + 20, y + 20, 300, 35, "Search Music Kits...");
    std::string query = searchBuffer;
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);

    std::vector<int> filteredIndices;
    for (int i = 0; i < (int)musicKits.size(); i++) {
        if (!query.empty()) {
             std::string name = musicKits[i].name;
             std::transform(name.begin(), name.end(), name.begin(), ::tolower);
             if (name.find(query) == std::string::npos) continue;
        }
        filteredIndices.push_back(i);
    }

    // Scroll Logic
    static float mScrollY = 0.0f;
    float itemW = 140; float itemH = 100; float pad = 15;
    int cols = (int)((w - 40) / (itemW + pad));
    if (cols < 1) cols = 1;

    // Content Height
    int rows = ((int)filteredIndices.size() + cols - 1) / cols;
    float contentH = rows * (itemH + pad);
    float viewY = y + 70;
    float viewH = h - 70;

    // Input
    if (SC_GUI::Input.mousePos.x >= x && SC_GUI::Input.mousePos.x <= x + w &&
        SC_GUI::Input.mousePos.y >= viewY && SC_GUI::Input.mousePos.y <= viewY + viewH) {
         mScrollY += SC_GUI::Input.scrollDelta * 0.5f;
    }
    
    // Clamp
    if (mScrollY > 0) mScrollY = 0;
    if (mScrollY < -(contentH - viewH + pad)) mScrollY = -(contentH - viewH + pad);

    // Draw
    SC_GUI::SetClip(x, viewY, w, viewH);
    int displayIdx = 0;
    for (int idx : filteredIndices) {
        float cX = x + 20 + (displayIdx % cols) * (itemW + pad);
        float cY = viewY + mScrollY + (displayIdx / cols) * (itemH + pad);

        if (cY + itemH < viewY || cY > viewY + viewH) { displayIdx++; continue; }

        bool selected = (selectedMusicKitIndex == idx);
        // Use Rarity 3 (Rare/Blue) for Music Kits as default
        if (SC_GUI::SkinCard("music_" + std::to_string(idx), musicKits[idx].name, musicKits[idx].image_url, 3, cX, cY, itemW, itemH, selected, configManager->diskCacheEnabled)) {
             selectedMusicKitIndex = idx;
             skinManager->MusicKit = musicKits[idx];
             ForceUpdate = true;
        }
        displayIdx++;
    }
    SC_GUI::ResetClip();
    
    // Scrollbar
    if (contentH > viewH) {
        float ratio = viewH / contentH;
        float barH = viewH * ratio;
        float barY = viewY + (-mScrollY / contentH) * viewH;
        SC_GUI::DrawRoundedRect(x + w - 8, barY, 4, barH, 2, Color(255, 100, 100, 100));
    }
}



void RenderConfigTab(float x, float y, float w, float h)
{
    float curY = y + 20;

    // Config Input
    static char cfgNameBuf[64] = "";
    SC_GUI::TextInput("cfg_name", cfgNameBuf, 64, x + 20, curY, 200, 35, "Config Name");
    
    if (SC_GUI::Button("cfg_create_save", "Create / Save", x + 230, curY, 120, 35)) {
        std::string name = cfgNameBuf;
        if (!name.empty()) {
            configManager->Save(name);
            memset(cfgNameBuf, 0, 64);
        }
    }
    curY += 50;
    
    // Separator
    SC_GUI::DrawRect(x + 20, curY, w - 40, 1, SC_GUI::currentTheme.separator);
    curY += 20;

    // List Configs
    std::vector<std::string> configs = configManager->GetConfigs();
    
    // Scrollable Area
    float listH = h - (curY - y) - 20;
    
    static float scrollY = 0.0f;
    float contentH = configs.size() * 45.0f;
    
    // Input for List
     if (SC_GUI::Input.mousePos.x >= x && SC_GUI::Input.mousePos.x <= x + w &&
        SC_GUI::Input.mousePos.y >= curY && SC_GUI::Input.mousePos.y <= curY + listH) {
         scrollY += SC_GUI::Input.scrollDelta * 0.5f;
    }
    if (scrollY > 0) scrollY = 0;
    if (scrollY < -(contentH - listH)) scrollY = -(contentH - listH);
    if (contentH < listH) scrollY = 0;

    SC_GUI::SetClip(x, curY, w, listH);
    
    for (int i=0; i<(int)configs.size(); i++) {
        float itemY = curY + scrollY + (i * 45);
        if (itemY + 40 < curY || itemY > curY + listH) continue;

        // Background
        SC_GUI::DrawRoundedRect(x + 20, itemY, w - 240, 35, 6, SC_GUI::currentTheme.contentBg);
        SC_GUI::DrawStringA(configs[i], x + 35, itemY + 10, SC_GUI::currentTheme.text, SC_GUI::mainFont, false);

        // Buttons
        if (SC_GUI::Button("cfg_load_" + std::to_string(i), "Load", x + w - 200, itemY, 80, 35)) {
            configManager->Load(configs[i]);
        }
        
        if (SC_GUI::Button("cfg_save_" + std::to_string(i), "Save", x + w - 110, itemY, 80, 35)) {
            configManager->Save(configs[i]);
        }
    }
    
    SC_GUI::ResetClip();
}

void RenderSettingsTab(float x, float y, float w, float h)
{
    float curY = y + 20;
    
    // Configs below

    
    // Transparency
    SC_GUI::DrawStringA("Menu Transparency", x + 20, curY, Color(255, 255, 255), SC_GUI::mainFont, false);
    curY += 25;
    
    static float alphaVal = 255.0f;
    if (SC_GUI::Slider("alpha_slider", &alphaVal, 50.0f, 255.0f, x + 20, curY, 300, 20)) {
        overlay::GlobalAlpha = (BYTE)alphaVal;
    }
    curY += 40;

    // Disk Cache
    if (SC_GUI::Checkbox("disk_cache_check", "Enable Disk Cache", &configManager->diskCacheEnabled, x + 20, curY)) {
        if (!configManager->diskCacheEnabled) {
            SC_GUI::TextureCache.ClearDisk();
        }
    }
    curY += 40;

    // Theme
    SC_GUI::DrawStringA("Theme", x + 20, curY, Color(255, 255, 255), SC_GUI::mainFont, false);
    curY += 25;
    SC_GUI::DrawStringA("BERSERK Dark Red (fixed)", x + 20, curY, SC_GUI::currentTheme.text, SC_GUI::mainFont, false);
    curY += 30;
    SC_GUI::DrawStringA("Theme selection is disabled. The menu uses a single dark red style.", x + 20, curY, SC_GUI::currentTheme.textDim, SC_GUI::smallFont, false);
    curY += 45;

    // Config Section Header
    SC_GUI::DrawStringA("Configurations", x + 20, curY, Color(255, 100, 255, 100), SC_GUI::titleFont, false);
    curY += 40;

    RenderConfigTab(x, curY, w, h - (curY - y));
    RenderConfigTab(x, curY, w, h - (curY - y));
}

// --- Glove Tab ---
void RenderGloveTab(float x, float y, float w, float h)
{
    // Layout: Left column for Glove Types, Right area for Skins
    float typeW = 160.0f;
    float typeX = x + 10;
    float typeY = y + 10;
    
    // Background for Types
    SC_GUI::DrawFilledRoundedRect(typeX, typeY, typeW, h - 20, 8.0f, SC_GUI::currentTheme.sidebarBg);
    
    static int selectedGloveIdx = 0;
    float curY = typeY + 10;
    
    // Render Glove Types
    for (int i=0; i < (int)GloveTypes.size(); i++) {
         bool active = (selectedGloveIdx == i);
         // Mini Tab Button
         if (SC_GUI::TabButton("glove_type_" + std::to_string(i), GloveTypes[i].name, typeX + 5, curY, typeW - 10, 40, active, "", SC_GUI::smallFont)) {
             selectedGloveIdx = i;
             // Reset skin selection offset?
         }
         curY += 45;
    }

    // Right Side: Skins
    float skinX = x + typeW + 20;
    float skinY = y + 10;
    float skinW = w - typeW - 30;
    float skinH = h - 20;
    
    std::string currentGloveName = GloveTypes[selectedGloveIdx].name;
    
    std::vector<SkinInfo_t> gloves = skindb->GetGloveSkins(currentGloveName);
    
    // Grid Layout
    float cardW = 180;
    float cardH = 220;
    float gap = 15;
    int cols = 4;
    
    // Safety
    if (gloves.empty()) {
        SC_GUI::DrawStringA("No Skins Found", skinX + 20, skinY + 20, SC_GUI::currentTheme.text, SC_GUI::mainFont, false);
        return;
    }
    
    float totalH = ((gloves.size() + cols - 1) / cols) * (cardH + gap);
    
    // Scroll
    static float scrollY = 0.0f;
    if (SC_GUI::Input.mousePos.x >= skinX && SC_GUI::Input.mousePos.x <= skinX + skinW &&
        SC_GUI::Input.mousePos.y >= skinY && SC_GUI::Input.mousePos.y <= skinY + skinH) {
         scrollY += SC_GUI::Input.scrollDelta * 0.5f;
    }
    if (scrollY > 0) scrollY = 0;
    float maxScroll = -(totalH - skinH);
    if (maxScroll > 0) maxScroll = 0;
    if (scrollY < maxScroll) scrollY = maxScroll;

    SC_GUI::SetClip(skinX, skinY, skinW, skinH);
    
    int currentPaint = skinManager->Gloves.Paint;
    uint16_t currentDef = skinManager->Gloves.defIndex;
    
    for(int i=0; i<(int)gloves.size(); i++) {
        int col = i % cols;
        int row = i / cols;
        
        float drawX = skinX + col * (cardW + gap);
        float drawY = skinY + scrollY + row * (cardH + gap);
        
        if (drawY + cardH < skinY || drawY > skinY + skinH) continue;
        
        // Selected?
        // We need to check if current Glove Def matches AND paint matches
        bool isSelected = (currentPaint == gloves[i].Paint && currentDef == GloveTypes[selectedGloveIdx].defIndex);
        
        if (SC_GUI::SkinCard("glove_" + std::to_string(i), gloves[i].name, gloves[i].image_url, gloves[i].rarity, drawX, drawY, cardW, cardH, isSelected, configManager->diskCacheEnabled)) {
            // Apply Glove
            skinManager->Gloves.defIndex = GloveTypes[selectedGloveIdx].defIndex;
            skinManager->Gloves.Paint = gloves[i].Paint;
            skinManager->Gloves.Paint = gloves[i].Paint;
            ForceUpdate = true; // Trigger update in main loop
        }
    }
    
    SC_GUI::ResetClip();
}

static int active_tab = 0;
bool MenuOpen = true;
bool CS2Connected = false;

// === LOADING SCREENS ===

// Static waiting for CS2 screen (no animation)
void RenderWaitingForCS2Screen()
{
    float w = 520;
    float h = 320;
    
    float x = (overlay::G_Width - w) / 2;
    float y = (overlay::G_Height - h) / 2;

    // Main Background
    SC_GUI::DrawRoundedRect(x, y, w, h, 12, SC_GUI::currentTheme.mainBg);
    
    // Border
    SC_GUI::DrawStrokeRoundedRect(x, y, w, h, 12, SC_GUI::currentTheme.border, 2.0f);

    // Logo/Title
    SC_GUI::DrawStringA("CS2 Skin Changer", x + w/2, y + 50, SC_GUI::currentTheme.text, SC_GUI::titleFont, true);

    // Main message
    SC_GUI::DrawStringA("Waiting for CS2...", x + w/2, y + 120, SC_GUI::currentTheme.textDim, SC_GUI::mainFont, true);

    // Status indicators (static dots)
    SC_GUI::DrawStringA("●  ○  ○", x + w/2, y + 170, SC_GUI::currentTheme.accent, SC_GUI::mainFont, true);

    // Status text
    SC_GUI::DrawStringA("Detecting Counter-Strike 2 Instance", x + w/2, y + 230, SC_GUI::currentTheme.textDim, SC_GUI::smallFont, true);

    // Tip text
    SC_GUI::DrawStringA("Make sure Counter-Strike 2 is running", x + w/2, y + 280, Color(255, 100, 100, 100), SC_GUI::smallFont, true);
}

// Animated indexing skins screen (5 second loading)
void RenderIndexingSkinsScreen()
{
    float w = 520;
    float h = 320;
    
    float x = (overlay::G_Width - w) / 2;
    float y = (overlay::G_Height - h) / 2;

    // Main Background
    SC_GUI::DrawRoundedRect(x, y, w, h, 12, SC_GUI::currentTheme.mainBg);
    
    // Border with accent
    SC_GUI::DrawStrokeRoundedRect(x, y, w, h, 12, SC_GUI::currentTheme.accent, 2.0f);

    // Logo/Title
    SC_GUI::DrawStringA("CS2 Skin Changer", x + w/2, y + 50, SC_GUI::currentTheme.accent, SC_GUI::titleFont, true);

    // Main message
    SC_GUI::DrawStringA("Indexing Skins...", x + w/2, y + 120, SC_GUI::currentTheme.text, SC_GUI::mainFont, true);

    // Animated Progress Bar
    float barW = 300;
    float barH = 12;
    float barX = x + (w - barW) / 2;
    float barY = y + 170;

    // Background of bar
    SC_GUI::DrawRoundedRect(barX, barY, barW, barH, 6, SC_GUI::currentTheme.contentBg);

    // Animated progress based on elapsed time
    ULONGLONG elapsed = GetTickCount64() - indexingStartTime;
    float progress = fmin(1.0f, (float)elapsed / INDEXING_DURATION);

    // Progress fill
    float fillW = barW * progress;
    SC_GUI::DrawRoundedRect(barX, barY, fillW, barH, 6, SC_GUI::currentTheme.accent);

    // Shimmer effect
    float shimmerPos = fmod((float)elapsed / 500.0f, 1.0f);
    float shimmerW = barW * 0.15f;
    float shimmerX = barX + (barW - shimmerW) * shimmerPos;
    Color shimmerColor = Color(255, 150, 150, 200);
    SC_GUI::DrawRoundedRect(shimmerX, barY, shimmerW, barH, 6, shimmerColor);

    // Animating dots
    static float dotAnim = 0.0f;
    dotAnim += 0.04f;
    if (dotAnim > 3.0f) dotAnim = 0.0f;

    std::string dots = "";
    if (dotAnim < 1.0f) dots = ".";
    else if (dotAnim < 2.0f) dots = "..";
    else dots = "...";

    SC_GUI::DrawStringA(dots, x + w/2 + 130, y + 120, SC_GUI::currentTheme.accent, SC_GUI::mainFont, true);

    // Status text with countdown
    int remainingSecs = (int)((INDEXING_DURATION - elapsed) / 1000);
    if (remainingSecs < 0) remainingSecs = 0;
    
    std::string statusText = "Preparing weapon database (" + std::to_string(remainingSecs) + "s)";
    SC_GUI::DrawStringA(statusText, x + w/2, y + 230, SC_GUI::currentTheme.textDim, SC_GUI::smallFont, true);

    // Check if indexing is complete
    if (elapsed >= INDEXING_DURATION) {
        currentPhase = PHASE_MAIN_MENU;
    }
}

// Loading messagebox for skin changes (overlay on main menu - not fullscreen)
void RenderSkinChangeLoadingBox()
{
    if (!isChangingSkin) return;

    float w = 280;
    float h = 160;
    
    // Center within the menu instead of screen
    float x = menuX + (menuW - w) / 2;
    float y = menuY + (menuH - h) / 2;

    // Semi-transparent dark backdrop ONLY over the menu area
    SC_GUI::DrawRect(menuX, menuY, menuW, menuH, Color(128, 12, 12, 12));

    // Message box Background
    SC_GUI::DrawRoundedRect(x, y, w, h, 12, SC_GUI::currentTheme.mainBg);
    
    // Border with accent
    SC_GUI::DrawStrokeRoundedRect(x, y, w, h, 12, SC_GUI::currentTheme.accent, 2.5f);

    // Message text
    SC_GUI::DrawStringA("Applying Skin", x + w/2, y + 35, SC_GUI::currentTheme.text, SC_GUI::mainFont, true);

    // Animated dots
    static float dotAnim = 0.0f;
    dotAnim += 0.05f;
    if (dotAnim > 3.0f) dotAnim = 0.0f;

    std::string dots = "";
    if (dotAnim < 1.0f) dots = ".";
    else if (dotAnim < 2.0f) dots = "..";
    else dots = "...";

    SC_GUI::DrawStringA("Changing" + dots, x + w/2, y + 80, SC_GUI::currentTheme.accent, SC_GUI::mainFont, true);

    // Animated pulsing dots indicator
    std::string pulsingDots = "";
    float phase = fmod(dotAnim, 3.0f);
    if (phase < 1.0f) pulsingDots = "●  ○  ○";
    else if (phase < 2.0f) pulsingDots = "○  ●  ○";
    else pulsingDots = "○  ○  ●";
    
    SC_GUI::DrawStringA(pulsingDots, x + w/2, y + 120, SC_GUI::currentTheme.accent, SC_GUI::mainFont, true);
}

// Main loading screen handler
void RenderLoadingScreen()
{
    if (currentPhase == PHASE_WAITING_CS2) {
        RenderWaitingForCS2Screen();
    } else if (currentPhase == PHASE_INDEXING_SKINS) {
        RenderIndexingSkinsScreen();
    }
}

void RenderMenu()
{
    float w = 1000;
    float h = 700;
    
    static float x = -1;
    static float y = -1;
    // Init Center
    if (x == -1) {
        x = (overlay::G_Width - w) / 2;
        y = (overlay::G_Height - h) / 2;
    }
    
    // Update global menu bounds for loading box positioning
    menuX = x;
    menuY = y;
    menuW = w;
    menuH = h;

    // Dragging Logic (Header Area)
    static bool draggingMenu = false;
    static POINT dragStart = {0, 0};
    static float dragX = 0.0f;
    static float dragY = 0.0f;

    bool headerHovered = SC_GUI::Input.mousePos.x >= x && SC_GUI::Input.mousePos.x <= x + w &&
                         SC_GUI::Input.mousePos.y >= y && SC_GUI::Input.mousePos.y <= y + 80;

    if (SC_GUI::Input.leftClicked && headerHovered) {
        draggingMenu = true;
        dragStart = SC_GUI::Input.mousePos;
        dragX = x;
        dragY = y;
    }

    if (draggingMenu) {
        if (SC_GUI::Input.leftClick) {
            x = dragX + (SC_GUI::Input.mousePos.x - dragStart.x);
            y = dragY + (SC_GUI::Input.mousePos.y - dragStart.y);
        } else {
            draggingMenu = false;
        }
    }

    // Constrain to screen bounds so it doesn't go totally offscreen
    float margin = 40.0f;
    if (x < -w + margin) x = -w + margin;
    if (y < -h + margin) y = -h + margin;
    if (x > overlay::G_Width - margin) x = overlay::G_Width - margin;
    if (y > overlay::G_Height - margin) y = overlay::G_Height - margin;

    // Main Window Background
    SC_GUI::DrawRoundedRect(x, y, w, h, 10, SC_GUI::currentTheme.mainBg);
    SC_GUI::DrawStrokeRoundedRect(x, y, w, h, 10, SC_GUI::currentTheme.border, 1.0f);

    // Header Background Gradient
    float headerH = 160.0f;
    SC_GUI::DrawGradientRoundedRect(x + 10, y + 10, w - 20, headerH, 10.0f,
        Color(255, 35, 25, 25), Color(255, 18, 10, 12));
    SC_GUI::DrawStrokeRoundedRect(x + 10, y + 10, w - 20, headerH, 10.0f, Color(255, 80, 80, 80), 1.0f);

    // Logo area
    float logoSize = 42.0f;
    SC_GUI::DrawRoundedRect(x + 24, y + 24, logoSize, logoSize, 10.0f, SC_GUI::currentTheme.accent);
    SC_GUI::DrawStringA("B", x + 24 + logoSize/2, y + 24 + logoSize/2, Color(255,255,255,255), SC_GUI::titleFont, true);
    SC_GUI::DrawStringA("ERSERK", x + 24 + logoSize + 2, y + 26, SC_GUI::currentTheme.text, SC_GUI::titleFont, false);

    // Top icon tabs row
    float navY = y + 105;
    float buttonSize = 38;
    float buttonGap = 10;
    float buttonsTotal = buttonSize * 5 + buttonGap * 4;
    float startX = x + 20 + (w - 40 - buttonsTotal) / 2;

    if (SC_GUI::TabButton("tab_wep", "", startX + (buttonSize + buttonGap) * 0, navY, buttonSize, buttonSize, active_tab == 0, "weapons")) active_tab = 0;
    if (SC_GUI::TabButton("tab_gloves", "", startX + (buttonSize + buttonGap) * 1, navY, buttonSize, buttonSize, active_tab == 4, "gloves")) active_tab = 4;
    if (SC_GUI::TabButton("tab_music", "", startX + (buttonSize + buttonGap) * 2, navY, buttonSize, buttonSize, active_tab == 1, "musickit")) active_tab = 1;
    if (SC_GUI::TabButton("tab_knives", "", startX + (buttonSize + buttonGap) * 3, navY, buttonSize, buttonSize, active_tab == 2, "knifes")) active_tab = 2;
    if (SC_GUI::TabButton("tab_settings", "", startX + (buttonSize + buttonGap) * 4, navY, buttonSize, buttonSize, active_tab == 3, "settings")) active_tab = 3;

    // Single separator line under header
    SC_GUI::DrawRect(x + 10, y + 155, w - 20, 1, Color(255, 70, 70, 70));

    // Content Area
    float cX = x + 20;
    float cY = y + 10 + headerH + 20;
    float cW = w - 40;
    float cH = h - (cY - y) - 20;

    SC_GUI::SetClip(cX, cY, cW, cH);
    switch (active_tab) {
        case 0: RenderWeaponTab(cX, cY, cW, cH); break;
        case 1: RenderMusicTab(cX, cY, cW, cH); break;
        case 2: RenderKnifeTab(cX, cY, cW, cH); break;
        case 3: RenderSettingsTab(cX, cY, cW, cH); break;
        case 4: RenderGloveTab(cX, cY, cW, cH); break;
    }
    SC_GUI::ResetClip();
}

void RenderUpdateStatusLabel(float menuX, float menuY, float menuW, float menuH)
{
    // Small label in bottom-right corner
    float labelW = 80;
    float labelH = 22;
    float padding = 10;
    float labelX = menuX + menuW - labelW - padding;
    float labelY = menuY + menuH - labelH - padding;

    Color labelColor = (updateStatus == "UPDATED")
        ? Color(255, 70, 200, 70)   // Green for UPDATED
        : (updateStatus == "OUTDATED"
            ? Color(255, 200, 70, 70)  // Red for OUTDATED
            : Color(255, 150, 150, 150)); // Grey for CHECKING

    // Background
    SC_GUI::DrawFilledRoundedRect(labelX, labelY, labelW, labelH, 4.0f, Color(180, 20, 20, 20));

    // Border
    SC_GUI::DrawStrokeRoundedRect(labelX, labelY, labelW, labelH, 4.0f, labelColor, 1.0f);

    // Text
    SC_GUI::DrawStringA(updateStatus, labelX + labelW/2, labelY + labelH/2, labelColor, SC_GUI::smallFont, true);
}

void OnFrame()
{
    // Check for updates periodically
    CheckForUpdates();

    // Handle skin change loading timeout (2 seconds)
    if (isChangingSkin) {
        ULONGLONG elapsed = GetTickCount64() - skinChangeStartTime;
        if (elapsed > 2000) {  // 2 second timeout
            isChangingSkin = false;
        }
    }

    // Disable input when changing skin
    bool inputEnabled = !isChangingSkin;

    // Toggle Logic (only if not changing skin)
    if (inputEnabled) {
        static bool prevInsert = false;
        bool insert = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
        if (insert && !prevInsert) {
            MenuOpen = !MenuOpen;
            overlay::SetInput(MenuOpen);
        }
        prevInsert = insert;
    }

    // Check current phase
    if (currentPhase == PHASE_WAITING_CS2 || currentPhase == PHASE_INDEXING_SKINS) {
        // Show loading screens
        overlay::Render(RenderLoadingScreen, true);
    } else {
        // Phase is PHASE_MAIN_MENU - show main menu
        overlay::Render(RenderMenu, MenuOpen);

        // Render update status label (visible in all tabs)
        RenderUpdateStatusLabel(menuX, menuY, menuW, menuH);

        // Render skin change loading box on top if needed
        if (isChangingSkin) {
            overlay::Render(RenderSkinChangeLoadingBox, true);
        }
    }
}

void MenuThread()
{
	while (!overlay::ShouldQuit)
	{
		OnFrame(); 
	}
}

void UpdateActiveMenuDef(const uintptr_t localPlayer)
{
	const uintptr_t activeWeapon = mem.Read<uintptr_t>(localPlayer + Offsets::m_pClippingWeapon);
	const uintptr_t activeItem = activeWeapon + Offsets::m_AttributeManager + Offsets::m_Item;
	CurrentWeaponDef = mem.Read<WeaponsEnum>(activeItem + Offsets::m_iItemDefinitionIndex);
}

void InitMenu(const bool autoThread = false)
{
	overlay::Setup();
    if (autoThread) {
        std::thread menuthread(MenuThread);
        menuthread.detach();
    }
	std::cout << "GDI+ Menu Setup\n";
}