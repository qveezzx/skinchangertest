@echo off
REM Git Push Guide for CS2 Skinchanger Knife & Glove Fixes
REM This script helps push changes to GitHub

echo.
echo ============================================================
echo CS2 SKINCHANGER - PUSH TO GITHUB
echo ============================================================
echo.

cd /d c:\Users\qveezzx\Documents\Changer

echo [1/5] Checking git status...
git status
echo.

echo [2/5] Adding all changes...
git add -A
echo. Files staged for commit:
git diff --cached --name-only
echo.

echo [3/5] Creating commit...
echo. Commit message will be:
echo "feat: Implement knife and glove model forcing"
echo "- Add CKnifeManager class for subclass ID mapping"
echo "- Implement 6-stage model forcing for knives"
echo "- Prepare 6-stage model forcing for gloves"
echo "- Add comprehensive documentation and automation tools"
echo "- Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
echo.

git commit -m "feat: Implement knife and glove model forcing

- Add CKnifeManager class with subclass ID mapping for 20 knife types
- Implement complete 6-stage knife model forcing in weapon loop
- Add UpdateSubClass and SetModel function signatures
- Enhance glove model forcing with proper staging (ready for patch)
- Add Sleep() synchronization between stages
- Update mesh masks for both first-person and third-person views
- Add comprehensive documentation (5 guides + 2 automation scripts)
- Create automatic patching tool for glove section

Knife Implementation: COMPLETE ✅
Glove Implementation: READY FOR PATCH ⏳

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"

echo.
echo [4/5] Checking remote...
git remote -v
echo.

echo [5/5] Ready to push!
echo.
echo To push to GitHub, run:
echo   git push origin main
echo.
echo Or use this one-liner:
echo   git push -u origin main
echo.
echo ============================================================
pause
