# GitHub Actions CI/CD Setup for CS2 Skin Changer

## 🚀 Quick Start

This guide walks you through setting up your CS2 Skin Changer to automatically build and compile on GitHub using GitHub Actions.

---

## ⚙️ What This Does

GitHub Actions will automatically:

✅ **Build the project** on every push to main/master/develop branches  
✅ **Compile the executable** with all dependencies  
✅ **Upload artifacts** for easy download  
✅ **Attach releases** to GitHub releases automatically  
✅ **Verify builds** on pull requests  

**Result**: Push code → Automatic compilation → Download exe ready to use!

---

## 📋 Prerequisites

Before you start, make sure you have:

1. ✅ A **GitHub account** (free tier works fine)
2. ✅ **Git installed** on your computer
3. ✅ The **CS2 Skin Changer code** ready to push
4. ✅ A **clean repository** (new or empty)

---

## 🎯 Setup Steps

### Step 1: Run Setup Script

Open PowerShell in your project directory and run:

```powershell
.\setup-github-actions.ps1
```

**What it does:**
- ✓ Creates `.github/workflows/` directory
- ✓ Creates `build.yml` workflow file
- ✓ Updates `.gitignore` with build artifacts
- ✓ Creates documentation

### Step 2: Initialize Git Repository

If you haven't already, initialize git:

```bash
cd c:\Users\qveezz\Documents\Changer
git init
git config user.name "Your Name"
git config user.email "your.email@example.com"
```

### Step 3: Add All Files

```bash
git add .
```

### Step 4: Create Initial Commit

```bash
git commit -m "Initial commit: CS2 Skin Changer with GitHub Actions"
```

### Step 5: Create GitHub Repository

1. Go to https://github.com/new
2. Name your repository: `cs2-skin-changer`
3. Add description: "Automated CS2 Skin Changer with GitHub Actions CI/CD"
4. Choose **Private** or **Public** (your choice)
5. Do **NOT** initialize with README, .gitignore, or license (you already have them)
6. Click **Create repository**

### Step 6: Add Remote and Push

Copy the commands from GitHub and run them. They'll look like:

```bash
git remote add origin https://github.com/YOUR_USERNAME/cs2-skin-changer.git
git branch -M main
git push -u origin main
```

### Step 7: Verify Workflow

1. Go to your repository on GitHub
2. Click the **Actions** tab
3. You should see a workflow running titled "Build CS2 Skin Changer"
4. Wait for it to complete (usually 5-10 minutes)

---

## 📥 Downloading Your Compiled Executable

### From GitHub Actions Artifacts

1. Go to **Actions** tab on GitHub
2. Click the latest successful workflow run
3. Scroll to **Artifacts** section
4. Download `CS2-SkinChanger-x64-Release`
5. Extract the zip file
6. Find `ext-cs2-skin-changer.exe`

### From GitHub Releases

1. Go to **Releases** on your repository
2. Download the executable from the latest release
3. Ready to use!

---

## 🔄 Workflow Usage

### Make Changes and Push

After setup, whenever you make changes:

```bash
# Make your code changes
# Then commit and push:

git add .
git commit -m "Description of changes"
git push
```

GitHub Actions will automatically:
- Build your project
- Test compilation
- Upload artifacts
- Notify you of success/failure

### Create a Release

To create a release with executable:

1. Go to GitHub repository
2. Click **Releases** → **Create a new release**
3. Tag version: `v1.0.0`
4. Release title: `CS2 Skin Changer v1.0.0`
5. Add description
6. Click **Publish release**

Executable will automatically attach to the release!

---

## 🏗️ What Gets Built

### Build Configuration
- **Platform**: x64 (64-bit)
- **Configuration**: Release (optimized)
- **Solution**: `src/ext-cs2-skin-changer.sln`

### Output Files
- `ext-cs2-skin-changer.exe` - Main executable
- `*.dll` - Any required runtime libraries

### Dependencies Included (Auto-downloaded)
- nlohmann/json 3.11.2
- openssl-native 1.0.1
- curl 7.53.0
- DirectX SDK

---

## 📁 File Structure

Your repository will have:

```
cs2-skin-changer/
├── .github/
│   └── workflows/
│       └── build.yml          ← GitHub Actions workflow
├── .gitignore
├── GITHUB_ACTIONS_SETUP.md
├── setup-github-actions.ps1
├── src/
│   ├── ext-cs2-skin-changer/
│   ├── ext-cs2-skin-changer.sln
│   └── ...
├── BUILDING.md
├── CHANGELOG_UPDATES.md
└── ... (other files)
```

---

## 🐛 Troubleshooting

### Issue: Workflow doesn't run after push

**Solution:**
- Verify `.github/workflows/build.yml` exists
- Check branch name is main/master/develop
- Trigger workflow manually: Actions → Build CS2 Skin Changer → Run workflow

### Issue: Build fails with NuGet error

**Solution:**
- This is usually temporary (network issue)
- Re-run the workflow: click **Re-run jobs**
- Verify `packages.config` exists in `src/ext-cs2-skin-changer/`

### Issue: Executable not found in artifacts

**Solution:**
- Check build logs for compilation errors
- Verify solution file path is correct
- Try building locally: `msbuild src/ext-cs2-skin-changer.sln /p:Configuration=Release /p:Platform=x64`

### Issue: "Permission denied" on push

**Solution:**
- Generate SSH key: `ssh-keygen -t ed25519`
- Add to GitHub: Settings → SSH Keys
- Or use HTTPS with personal access token

---

## 🔐 Security Notes

✅ **What's Safe:**
- NuGet packages download from official sources
- Workflow runs in Microsoft-hosted runners
- No credentials stored in workflow
- GitHub Token automatically managed

✅ **Best Practices:**
- Keep repository up to date
- Review workflow logs regularly
- Don't commit sensitive information
- Use branch protection rules

---

## 🚀 Advanced Usage

### Trigger Manual Builds

1. Go to Actions tab
2. Select "Build CS2 Skin Changer"
3. Click "Run workflow"
4. Select branch
5. Click "Run workflow"

### Schedule Builds

To build on a schedule, add to `build.yml`:

```yaml
on:
  schedule:
    - cron: '0 0 * * 0'  # Every Sunday at midnight
```

### Build on Tags

To build only on git tags:

```yaml
on:
  push:
    tags:
      - 'v*'
```

---

## 📚 Reference

### GitHub Actions Documentation
- [GitHub Actions Docs](https://docs.github.com/en/actions)
- [Workflow Syntax](https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions)
- [Artifacts](https://docs.github.com/en/actions/using-workflows/storing-workflow-data-as-artifacts)

### Windows Runners
- [Hosted Runners Overview](https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners)
- [Windows Runner Specs](https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners/about-github-hosted-runners#supported-runners-and-hardware-resources)

### MSBuild and NuGet
- [MSBuild Documentation](https://docs.microsoft.com/en-us/visualstudio/msbuild)
- [NuGet Docs](https://docs.microsoft.com/en-us/nuget/)

---

## ✅ Verification Checklist

Before you push, verify:

- [ ] `.github/workflows/build.yml` exists
- [ ] `.gitignore` includes build artifacts
- [ ] `src/ext-cs2-skin-changer.sln` exists
- [ ] `src/ext-cs2-skin-changer/packages.config` exists
- [ ] All source files are committed
- [ ] Repository is created on GitHub
- [ ] Remote URL is correct

---

## 🎓 Example Workflow

### Day 1: Initial Setup
```bash
1. Run setup-github-actions.ps1
2. git add .
3. git commit -m "Initial commit"
4. git push
5. Wait for build to complete
6. Download executable
```

### Day 2: Make Changes
```bash
1. Edit code in src/
2. git add .
3. git commit -m "Fixed knife changing"
4. git push
5. Workflow runs automatically
6. Download updated executable
```

### Day 3: Release
```bash
1. Create release on GitHub
2. Executable automatically attached
3. Users download from releases page
```

---

## 🎉 Done!

You now have fully automated CI/CD with GitHub Actions!

**Benefits:**
- ✅ No manual building needed
- ✅ Always have latest executable
- ✅ Easy sharing with others
- ✅ Professional release management
- ✅ Automatic artifact storage

---

## 📞 Support

If you encounter issues:

1. Check this documentation
2. Review workflow logs: Actions → Click workflow → View logs
3. Check [GitHub Actions docs](https://docs.github.com/en/actions)
4. Verify local build works first

---

## 📝 Notes

- Free GitHub tier includes 2,000 Action minutes per month
- Windows builds use more minutes (multi-minute builds)
- You'll likely have plenty for personal use
- Open source repositories get more free minutes

---

**Status**: ✅ Ready for deployment!  
**Last Updated**: April 2, 2026  
**CS2 Skin Changer Version**: Updated with Latest Offsets
