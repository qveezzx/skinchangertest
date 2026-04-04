#include <openssl/evp.h>
#include <openssl/ssl.h>



#include "menu.h"
#include "SDK/entity/CWeaponServices.h"
#include "SDK/weapon/C_EconEntity.h"
#include "SDK/CEconItemAttributeManager.h"
#include "update_offsets.h"

void Radar()
{
    for (uint8_t i = 1; i < 64; i++)
    {
        const uintptr_t Controller = GetEntityByHandle(i);
        if(!Controller)
			continue;
        const uintptr_t Pawn = GetEntityByHandle(mem.Read<uint16_t>(Controller + 0x6B4));
		if (!Pawn)
            continue;

		const uintptr_t entitySpotted = Pawn + 0x2700;
        mem.Write<bool>(entitySpotted + 0x8, true);
    }
}

class CBaseEntity
{
public:
	char pad0[0x330];//0x000
	uintptr_t sceneNode; //0x330
};

void su()
{
    INPUT input = { 0 };
    ZeroMemory(&input, sizeof(INPUT));
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VK_SPACE;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void sd()
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VK_SPACE;
    SendInput(1, &input, sizeof(INPUT));
}

int main()
{
    // Initialize menu and overlay first so loading screen shows
    InitMenu();
    
    configManager->Setup();
    
    std::cout << "SkinChanger Initializing...\n";

    // === PHASE 1: WAITING FOR CS2 ===
    // Keep showing loading screen until CS2 is found
    bool cs2Found = mem.IsConnected();
    while (!cs2Found && !overlay::ShouldQuit)
    {
        Sleep(500); // Check every 500ms
        
        // Render loading screen with phase
        OnFrame();
        
        // Try to connect to CS2
        if (!mem.IsConnected()) {
            mem.TryReconnect(L"cs2.exe");
        }
        
        if (mem.IsConnected()) {
            cs2Found = true;
            // Give CS2 ~1 second to fully initialize after connection
            Sleep(1000);
        }
    }

    // === PHASE 2: CS2 FOUND - INDEXING SKINS (5+ second loading) ===
    if (!overlay::ShouldQuit) {
        std::cout << "CS2 Found! Indexing skins...\n";
        
        // Move to indexing phase
        currentPhase = PHASE_INDEXING_SKINS;
        indexingStartTime = GetTickCount64();
        
        // Initialize the skin changer data during this phase (while loading screen is showing)
        mem.Write<uint16_t>(Sigs::RegenerateWeaponSkins + 0x52, (uint16_t)(Offsets::m_AttributeManager + Offsets::m_Item + Offsets::m_AttributeList + Offsets::m_Attributes));

        skindb->Dump();
        Updater::UpdateOffsets();
        
        configManager->AutoLoad();

        std::cout << "Skin database loaded, waiting for indexing to complete...\n";
        
        // Wait for indexing phase to complete (5 seconds)
        while (currentPhase == PHASE_INDEXING_SKINS && !overlay::ShouldQuit)
        {
            Sleep(100);
            OnFrame();
        }
        
        // Additional delay after indexing to ensure weapon list is populated
        std::cout << "Indexing complete, finalizing weapon list...\n";
        Sleep(500);
        
        // Force UpdateOffsets and retrieve weapon list at least once before starting main loop
        const uintptr_t localPlayer = GetLocalPlayer();
        if (localPlayer) {
            const std::vector<uintptr_t> weapons = GetWeapons(localPlayer);
            std::cout << "Initial weapon list populated: " << weapons.size() << " weapons found\n";
        }
    }

    // === PHASE 3: MAIN SKINCHANGER LOOP ===
    std::cout << "SkinChanger Started\n";
    while (!overlay::ShouldQuit && mem.IsConnected())
    {
        Sleep(5);

        const uintptr_t localController = GetLocalController();
        const uintptr_t inventoryServices = mem.Read<uintptr_t>(localController + Offsets::m_pInventoryServices);
        const uintptr_t localPlayer = GetLocalPlayer();
        const uintptr_t pWeaponServices = mem.Read<uintptr_t>(localPlayer + Offsets::m_pWeaponServices);

        mem.Write<uint16_t>(inventoryServices + Offsets::m_unMusicID, skinManager->MusicKit.id);

        UpdateActiveMenuDef(localPlayer);
        OnFrame();

        bool ShouldUpdate = false;

        // Process weapons every frame for consistent skin application
        // (removed updateCounter optimization to ensure skin applies immediately)
        {

            const std::vector<uintptr_t> weapons = GetWeapons(localPlayer);
    /*
    //clean up & hud update

    for (const uintptr_t& weapon : weapons)
    {
        const uintptr_t item = weapon + Offsets::m_AttributeManager + Offsets::m_Item;
        
        if (mem.Read<uint32_t>(weapon + Offsets::m_nFallbackPaintKit) == -1)
            continue;

        mem.Write<uint32_t>(weapon + Offsets::m_nFallbackPaintKit, -1);

        //UpdateHud(weapon);

        econItemAttributeManager.Remove(item);
        continue;
    }
    */
        for (const uintptr_t& weapon : weapons)
        {
            const uintptr_t item = weapon + Offsets::m_AttributeManager + Offsets::m_Item;
            const uint16_t weaponDefIndex = mem.Read<uint16_t>(item + Offsets::m_iItemDefinitionIndex);
            
            if(ForceUpdate)
                mem.Write<uint32_t>(item + Offsets::m_iItemIDHigh, NULL);

            if (mem.Read<uint32_t>(item + Offsets::m_iItemIDHigh) == -1)
                continue;

            mem.Write<uint32_t>(item + Offsets::m_iItemIDHigh, -1);

            SkinInfo_t skin = GetSkin(item);
            if (!skin.Paint) 
                continue;

            mem.Write<uint32_t>(weapon + Offsets::m_nFallbackPaintKit, skin.Paint);
            
            // Also update weapon name with skin name
            const uintptr_t itemView = item; // item is C_EconItemView
            if (!skin.name.empty()) {
                // Write skin name to weapon (max 160 chars for name override)
                char nameBuf[161] = {0};
                strncpy_s(nameBuf, sizeof(nameBuf), skin.name.c_str(), _TRUNCATE);
                WriteProcessMemory(mem.hProcess, reinterpret_cast<LPVOID>(itemView + Offsets::m_szCustomNameOverride), nameBuf, sizeof(nameBuf), nullptr);
            }

            const uint64_t mask = skin.bUsesOldModel + 1;

            const uintptr_t hudWeapon = GetHudWeapon(weapon);
            SetMeshMask(weapon, mask);
            SetMeshMask(hudWeapon, mask);

            bool isKnife = (weaponDefIndex == WeaponsEnum::CtKnife || weaponDefIndex == WeaponsEnum::Tknife);
            
            if (isKnife && skinManager->Knife.defIndex != 0)
            {
                const auto it = KnifeModels.find(skinManager->Knife.defIndex);
                if (it != KnifeModels.end())
                {
                    mem.Write<uint16_t>(item + Offsets::m_iItemDefinitionIndex, skinManager->Knife.defIndex);
                }
            }

            econItemAttributeManager.Create(item, skin);

            ShouldUpdate = true;
        }

        // --- Glove Changer Logic ---
        static uint16_t lastGloveDef = 0;
        static int lastGlovePaint = 0;
        
        bool userChangedSettings = (skinManager->Gloves.defIndex != lastGloveDef || 
                                    skinManager->Gloves.Paint != lastGlovePaint);

        if (skinManager->Gloves.defIndex != 0)
        {
             const uintptr_t localPlayerPawn = GetLocalPlayer(); 
             if (localPlayerPawn) {
                 const uintptr_t econGloves = localPlayerPawn + Offsets::m_EconGloves;
                 
                 // Read current (before we overwrite)
                 uint16_t currentDef = mem.Read<uint16_t>(econGloves + Offsets::m_iItemDefinitionIndex);
                 int currentPaint = mem.Read<int>(econGloves + Offsets::m_nFallbackPaintKit);
                 // int currentIDHigh = mem.Read<int>(econGloves + Offsets::m_iItemIDHigh);

                 // Get Valid XUID from Active Weapon (if existing) or fallback to AccountID
                 int xuidLow = -1;
                 int xuidHigh = -1;
                 if (weapons.size() > 0) {
                     uintptr_t firstWep = weapons[0]; 
                     if (firstWep) {
                         xuidLow = mem.Read<int>(firstWep + Offsets::m_OriginalOwnerXuidLow);
                         xuidHigh = mem.Read<int>(firstWep + Offsets::m_OriginalOwnerXuidLow + 4);
                     }
                 }

                 // Debug Output (Throttle to every 60 frames ~ 1 sec)
                 static int frameCount = 0;
                 if (frameCount++ % 60 == 0) {
                     std::cout << "[GloveDebug] CurDef: " << currentDef << " | CurPaint: " << currentPaint 
                               << " | TargetDef: " << skinManager->Gloves.defIndex 
                               << " | TargetPaint: " << skinManager->Gloves.Paint 
                               << " | XUID_Low: " << xuidLow << std::endl;
                 }
                 
                 // Attributes logic
                 bool gameReverted = (currentDef != skinManager->Gloves.defIndex);
                 
                 if (userChangedSettings || ForceUpdate || gameReverted) 
                 {
                     // Disable Initialization during write
                     mem.Write<bool>(econGloves + Offsets::m_bInitialized, false);
                     mem.Write<bool>(econGloves + Offsets::m_bDisallowSOC, false);
                     mem.Write<bool>(econGloves + Offsets::m_bRestoreCustomMaterialAfterPrecache, true);
                     
                     mem.Write<uint16_t>(econGloves + Offsets::m_iItemDefinitionIndex, skinManager->Gloves.defIndex);
                     
                     // Set ItemID to -1 (both Low and High)
                     mem.Write<int>(econGloves + Offsets::m_iItemIDHigh, -1);
                     mem.Write<int>(econGloves + Offsets::m_iItemIDHigh + 4, -1);
                     
                     mem.Write<int>(econGloves + Offsets::m_iEntityQuality, 3);
                     
                     // Apply XUID if found
                     if (xuidLow != -1) {
                         mem.Write<int>(econGloves + Offsets::m_OriginalOwnerXuidLow, xuidLow);
                         mem.Write<int>(econGloves + Offsets::m_OriginalOwnerXuidLow + 4, xuidHigh);
                         mem.Write<int>(econGloves + Offsets::m_iAccountID, xuidLow & 0xFFFFFFFF); 
                     } else {
                         mem.Write<int>(econGloves + Offsets::m_iAccountID, 12345);
                     }

                     // Create Attribute List
                     SkinInfo_t gloveSkin;
                     gloveSkin.Paint = skinManager->Gloves.Paint;
                     gloveSkin.weaponType = WeaponsEnum::none; 
                     econItemAttributeManager.Create(econGloves, gloveSkin);


                     mem.Write<bool>(econGloves + Offsets::m_bInitialized, true);
                     
                     mem.Write<bool>(localPlayerPawn + Offsets::m_bNeedToReApplyGloves, true);
                     
                     lastGloveDef = skinManager->Gloves.defIndex;
                     lastGlovePaint = skinManager->Gloves.Paint;
                 }
             }
        }

        if (ShouldUpdate || ForceUpdate)
            UpdateWeapons(weapons);
        }

        ForceUpdate = false;
        
        if (ShouldUpdate) {
            configManager->AutoSave();
        }
    }

    // === CS2 DISCONNECTED or ShouldQuit ===
    if (!overlay::ShouldQuit) {
        // CS2 process closed, go back to waiting screen
        std::cout << "CS2 disconnected. Waiting for reconnection...\n";
        CS2Connected = false;
        
        // Return to waiting loop
        int maxRetries = 0;
        while (maxRetries < 100000 && !overlay::ShouldQuit)  // Will loop indefinitely if overlay is running
        {
            Sleep(500);
            OnFrame();
            
            if (!mem.IsConnected()) {
                mem.TryReconnect(L"cs2.exe");
            }
            
            if (mem.IsConnected()) {
                // CS2 found again, restart skinchanger
                std::cout << "CS2 reconnected. Restarting...\n";
                return main();  // Restart the entire process
            }
            maxRetries++;
        }
    }
    
    return 0;
}