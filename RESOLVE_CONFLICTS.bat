@echo off
REM Resolve merge conflicts and push to GitHub

cd /d c:\Users\qveezzx\Documents\Changer

echo [Step 1] Adding resolved files
git add src/ext-cs2-skin-changer/ext/sigs.h
git add src/ext-cs2-skin-changer/ext/skindb.h
git add src/ext-cs2-skin-changer/src/main.cpp

echo.
echo [Step 2] Committing merge resolution
git commit -m "fix: Resolve merge conflicts in knife and glove implementation"

echo.
echo [Step 3] Pushing to GitHub
git push

echo.
echo [DONE] All conflicts resolved and pushed!
