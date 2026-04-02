#!/usr/bin/env python3
# Script to edit main.cpp for glove initialization

file_path = r"c:\Users\qveezz\Documents\Changer\src\ext-cs2-skin-changer\src\main.cpp"

with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# First edit: Add two lines after m_bInitialized line
found_first = False
for i, line in enumerate(lines):
    if 'mem.Write<bool>(econGloves + Offsets::m_bInitialized, false);' in line:
        print(f"Found m_bInitialized at line {i+1}")
        new_line1 = '                      mem.Write<bool>(econGloves + Offsets::m_bDisallowSOC, false);\n'
        new_line2 = '                      mem.Write<bool>(econGloves + Offsets::m_bRestoreCustomMaterialAfterPrecache, true);\n'
        lines.insert(i+1, new_line1)
        lines.insert(i+2, new_line2)
        found_first = True
        print("Added m_bDisallowSOC and m_bRestoreCustomMaterialAfterPrecache lines")
        break

# Second edit: Change xuidLow to xuidLow & 0xFFFFFFFF
found_second = False
for i, line in enumerate(lines):
    if 'mem.Write<int>(econGloves + Offsets::m_iAccountID, xuidLow' in line and '0xFFFFFFFF' not in line:
        print(f"Found m_iAccountID xuidLow at line {i+1}")
        lines[i] = line.replace('xuidLow);', 'xuidLow & 0xFFFFFFFF);')
        found_second = True
        print("Updated m_iAccountID to use bitwise AND with 0xFFFFFFFF")
        break

if found_first and found_second:
    with open(file_path, 'w', encoding='utf-8') as f:
        f.writelines(lines)
    print("\nBoth edits completed successfully!")
else:
    print(f"\nWarning: found_first={found_first}, found_second={found_second}")
