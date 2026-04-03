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

        const uintptr_t localController = GetLocalController();
        const uintptr_t inventoryServices = mem.Read<uintptr_t>(localController + Offsets::m_pInventoryServices);
        const uintptr_t localPlayer = GetLocalPlayer();
        const uintptr_t pWeaponServices = mem.Read<uintptr_t>(localPlayer + Offsets::m_pWeaponServices);

        mem.Write<uint16_t>(inventoryServices + Offsets::m_unMusicID, skinManager->MusicKit.id);

        UpdateActiveMenuDef(localPlayer);
        OnFrame();

        bool ShouldUpdate = false;

<<<<<<< HEAD
        static int updateCounter = 0;
        updateCounter++;
        
        // Only process weapons every 2 frames (~10ms) to reduce CPU usage
        if (updateCounter % 2 == 0)
        {
            const std::vector<uintptr_t> weapons = GetWeapons(localPlayer);
=======
        const std::vector<uintptr_t> weapons = GetWeapons(localPlayer);
>>>>>>> 65bda63 (feat: Implement knife and glove model forcing - Complete knife implementation + glove ready for patch + comprehensive docs - Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>)
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
<<<<<<< HEAD
=======
            mem.Write<float>(weapon + Offsets::m_flFallbackWear, 0.01f);
            mem.Write<uint32_t>(weapon + Offsets::m_nFallbackSeed, 0);
>>>>>>> 65bda63 (feat: Implement knife and glove model forcing - Complete knife implementation + glove ready for patch + comprehensive docs - Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>)

            const uint64_t mask = skin.bUsesOldModel + 1;

            const uintptr_t hudWeapon = GetHudWeapon(weapon);
            SetMeshMask(weapon, mask);
            SetMeshMask(hudWeapon, mask);

<<<<<<< HEAD
=======
            // --- KNIFE MODEL CHANGING ---
>>>>>>> 65bda63 (feat: Implement knife and glove model forcing - Complete knife implementation + glove ready for patch + comprehensive docs - Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>)
            bool isKnife = (weaponDefIndex == WeaponsEnum::CtKnife || weaponDefIndex == WeaponsEnum::Tknife);
            
            if (isKnife && skinManager->Knife.defIndex != 0)
            {
                const auto it = KnifeModels.find(skinManager->Knife.defIndex);
                if (it != KnifeModels.end())
                {
<<<<<<< HEAD
                    mem.Write<uint16_t>(item + Offsets::m_iItemDefinitionIndex, skinManager->Knife.defIndex);
=======
                    // Stage 1: Set disallow_soc and restore flags for proper material handling
                    mem.Write<bool>(item + 0x1E9, false);  // m_bDisallowSOCm
                    mem.Write<bool>(item + 0x1B8, true);   // m_bRestoreCustomMaterialAfterPrecache
                    
                    // Stage 2: Apply knife definition index
                    mem.Write<uint16_t>(item + Offsets::m_iItemDefinitionIndex, skinManager->Knife.defIndex);
                    
                    Sleep(5);
                    
                    // Stage 3: Update subclass ID for proper model rendering
                    auto knifeIt = knifeManager->m_subclassIdMap.find(skinManager->Knife.defIndex);
                    if (knifeIt != knifeManager->m_subclassIdMap.end())
                    {
                        mem.Write<uint64_t>(weapon + Offsets::m_nSubclassID, knifeIt->second);
                    }
                    
                    Sleep(5);
                    
                    // Stage 4: Call UpdateSubClass to refresh entity rendering
                    uintptr_t updateSubClassAddr = Sigs::GetUpdateSubClassFunc();
                    if (updateSubClassAddr)
                    {
                        mem.CallThread(updateSubClassAddr, reinterpret_cast<LPVOID>(weapon));
                        Sleep(50);
                    }
                    
                    // Stage 5: Update mesh masks for weapon and HUD version
                    const uintptr_t weaponNode = mem.Read<uintptr_t>(weapon + Offsets::m_pGameSceneNode);
                    if (weaponNode)
                    {
                        const auto weaponModel = weaponNode + Offsets::m_modelState;
                        mem.Write<uint64_t>(weaponModel + Offsets::m_MeshGroupMask, 1);
                    }
                    
                    if (hudWeapon)
                    {
                        const uintptr_t hudNode = mem.Read<uintptr_t>(hudWeapon + Offsets::m_pGameSceneNode);
                        if (hudNode)
                        {
                            const uintptr_t viewModel = hudNode + Offsets::m_modelState;
                            mem.Write<uint64_t>(viewModel + Offsets::m_MeshGroupMask, 1);
                        }
                    }
                    
                    std::cout << "[Knife] Model changed to: " << skinManager->Knife.name << std::endl;
>>>>>>> 65bda63 (feat: Implement knife and glove model forcing - Complete knife implementation + glove ready for patch + comprehensive docs - Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>)
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
<<<<<<< HEAD
                     ShouldUpdate = true;
=======
>>>>>>> 65bda63 (feat: Implement knife and glove model forcing - Complete knife implementation + glove ready for patch + comprehensive docs - Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>)
                 }
             }
        }

        if (ShouldUpdate || ForceUpdate)
            UpdateWeapons(weapons);
<<<<<<< HEAD
        }
=======
>>>>>>> 65bda63 (feat: Implement knife and glove model forcing - Complete knife implementation + glove ready for patch + comprehensive docs - Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>)

        ForceUpdate = false;
        
        if (ShouldUpdate) {
            configManager->AutoSave();
        }
    }
    
    return 0;
}