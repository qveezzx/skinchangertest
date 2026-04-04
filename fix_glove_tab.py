#!/usr/bin/env python3
"""Add beta warning to RenderGloveTab in menu.h"""

filepath = r'C:\Users\qveezzx\Documents\Changer\src\ext-cs2-skin-changer\src\menu.h'

with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
    lines = f.readlines()

# Find RenderGloveTab and add beta warning
for i, line in enumerate(lines):
    if 'void RenderGloveTab(float x, float y, float w, float h)' in line and i < len(lines) - 3:
        # Check if it's the actual function definition
        if '{' in lines[i + 1]:
            # Insert beta warning after the opening brace
            insert_code = '''     // ===== BETA WARNING =====
     if (currentAppState == AppState::READY && !warnedGloves) {
         warnedGloves = true;
         showBetaWarning = true;
     }
     
'''
            lines.insert(i + 2, insert_code)
            print(f"✓ Added beta warning to RenderGloveTab at line {i+1}")
            break

# Write back
with open(filepath, 'w', encoding='utf-8') as f:
    f.writelines(lines)

print("✓ Successfully updated menu.h")
