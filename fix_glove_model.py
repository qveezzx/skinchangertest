#!/usr/bin/env python3
"""
Script to fix glove model changing in main.cpp
Adds proper staging with Sleep calls for glove model changes
"""

import sys

# Read the file
with open(r'c:\Users\qveezzx\Documents\Changer\src\ext-cs2-skin-changer\src\main.cpp', 'r', encoding='utf-8-sig') as f:
    content = f.read()

# Replace the glove initialization block
old_text = '''                      // Disable Initialization during write
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
                      lastGlovePaint = skinManager->Gloves.Paint;'''

new_text = '''                      // Stage 1: Disable initialization during writes
                      mem.Write<bool>(econGloves + Offsets::m_bInitialized, false);
                      mem.Write<bool>(econGloves + Offsets::m_bDisallowSOC, false);
                      mem.Write<bool>(econGloves + Offsets::m_bRestoreCustomMaterialAfterPrecache, true);
                      
                      // Stage 2: Apply glove definition
                      mem.Write<uint16_t>(econGloves + Offsets::m_iItemDefinitionIndex, skinManager->Gloves.defIndex);
                      
                      Sleep(5);
                      
                      // Stage 3: Reset ItemID to force regeneration
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

                      Sleep(5);
                      
                      // Stage 4: Create paint kit attributes
                      SkinInfo_t gloveSkin;
                      gloveSkin.Paint = skinManager->Gloves.Paint;
                      gloveSkin.weaponType = WeaponsEnum::none; 
                      econItemAttributeManager.Create(econGloves, gloveSkin);

                      Sleep(5);
                      
                      // Stage 5: Finalize initialization and trigger re-apply
                      mem.Write<bool>(econGloves + Offsets::m_bInitialized, true);
                      
                      mem.Write<bool>(localPlayerPawn + Offsets::m_bNeedToReApplyGloves, true);
                      
                      // Stage 6: Update mesh masks for glove rendering
                      const uintptr_t gloveNode = mem.Read<uintptr_t>(econGloves + Offsets::m_pGameSceneNode);
                      if (gloveNode)
                      {
                          const uintptr_t gloveModel = gloveNode + Offsets::m_modelState;
                          mem.Write<uint64_t>(gloveModel + Offsets::m_MeshGroupMask, 1);
                      }
                      
                      lastGloveDef = skinManager->Gloves.defIndex;
                      lastGlovePaint = skinManager->Gloves.Paint;
                      
                      std::cout << "[Gloves] Model changed to: " << skinManager->Gloves.name << std::endl;'''

if old_text in content:
    content = content.replace(old_text, new_text)
    # Write back without BOM
    with open(r'c:\Users\qveezzx\Documents\Changer\src\ext-cs2-skin-changer\src\main.cpp', 'w', encoding='utf-8') as f:
        f.write(content)
    print("✓ Glove model changing updated successfully!")
else:
    print("✗ Could not find glove section to replace")
    sys.exit(1)
