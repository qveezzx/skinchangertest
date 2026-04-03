#!/usr/bin/env python3
"""
Fix Glove Model Changing in CS2 Skinchanger
Automatically applies the enhanced glove model changing code
"""

import os
import sys

def fix_glove_model():
    filepath = r'src\ext-cs2-skin-changer\src\main.cpp'
    
    if not os.path.exists(filepath):
        print(f"❌ File not found: {filepath}")
        print("Please run this script from the project root directory")
        return False
    
    # Read file with BOM awareness
    with open(filepath, 'r', encoding='utf-8-sig') as f:
        content = f.read()
    
    # Check if already patched
    if "Stage 1: Disable initialization during writes" in content:
        print("✓ Glove model changing already patched!")
        return True
    
    # Old pattern to find
    old_pattern = '''                      // Disable Initialization during write
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
    
    new_pattern = '''                      // Stage 1: Disable initialization during writes
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
    
    if old_pattern not in content:
        print("❌ Could not find glove section to patch")
        print("The code may have already been modified or the format differs")
        return False
    
    # Apply the replacement
    new_content = content.replace(old_pattern, new_pattern)
    
    # Write back without BOM
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    print("✓ Glove model changing successfully patched!")
    print("\nNext steps:")
    print("1. Rebuild the project:")
    print("   nuget restore ext-cs2-skin-changer.sln")
    print("   msbuild ext-cs2-skin-changer.sln /p:Configuration=Release /p:Platform=x64")
    print("\n2. Test with bot server")
    print("3. Try changing knife and glove models")
    
    return True

if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.abspath(__file__)) or '.')
    success = fix_glove_model()
    sys.exit(0 if success else 1)
