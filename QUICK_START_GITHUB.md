# 🚀 GitHub Actions - Quick Start (5 Minutes)

## TL;DR - Copy/Paste Commands

Open PowerShell in project directory and run:

```powershell
# 1. Setup GitHub Actions
.\setup-github-actions.ps1

# 2. Initialize Git
git init
git config user.name "Your Name"
git config user.email "your@email.com"

# 3. Add and commit everything
git add .
git commit -m "Initial commit with GitHub Actions CI/CD"

# 4. Create repo on https://github.com/new - name it "cs2-skin-changer"

# 5. Copy these commands from GitHub and run them:
# (Replace USERNAME with your GitHub username)
git remote add origin https://github.com/USERNAME/cs2-skin-changer.git
git branch -M main
git push -u origin main

# 6. Done! Watch the Actions tab build your project
```

---

## What Happens Next

1. ✅ GitHub Actions automatically builds your code
2. ✅ Executable is ready to download in 5-10 minutes
3. ✅ Future pushes auto-build
4. ✅ Releases auto-attach executables

---

## Download Your Exe

**Option A: From Actions**
1. GitHub → Actions tab
2. Click latest successful build
3. Download artifact under "Artifacts"

**Option B: Create Release**
1. GitHub → Releases → Create new release
2. Executable auto-attached

---

## That's It! 🎉

Every time you push, it builds automatically!

For detailed info, read **GITHUB_ACTIONS_SETUP.md**
