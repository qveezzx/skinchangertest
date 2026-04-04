#!/usr/bin/env python3
"""Update menu.h with waiting state for gloves tab"""

import re

filepath = 'src/ext-cs2-skin-changer/src/menu.h'

with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
    content = f.read()

# Add beta warning to RenderGloveTab
glove_pattern = r'(void RenderGloveTab\(float x, float y, float w, float h\)\s*\{)\s*(\/\/ Layout:)'
glove_replacement = r'''\1
     // ===== BETA WARNING =====
     if (currentAppState == AppState::READY && !warnedGloves) {
         warnedGloves = true;
         showBetaWarning = true;
     }
     
     \2'''

content = re.sub(glove_pattern, glove_replacement, content)

with open(filepath, 'w', encoding='utf-8') as f:
    f.write(content)

print("✓ Updated RenderGloveTab with beta warning")
