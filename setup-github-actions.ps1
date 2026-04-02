# GitHub Actions Setup Script for CS2 Skin Changer
# Run this script in PowerShell from the project root directory

Write-Host "Setting up GitHub Actions CI/CD pipeline..." -ForegroundColor Green

# Create directory structure
$workflowDir = ".github/workflows"
if (!(Test-Path $workflowDir)) {
    New-Item -ItemType Directory -Path $workflowDir -Force | Out-Null
    Write-Host "✓ Created $workflowDir directory" -ForegroundColor Green
} else {
    Write-Host "✓ $workflowDir directory already exists" -ForegroundColor Green
}

# Create build.yml workflow file
$buildWorkflow = @'
name: Build CS2 Skin Changer

on:
  push:
    branches: [ main, master, develop ]
  pull_request:
    branches: [ main, master, develop ]
  release:
    types: [created]

env:
  SOLUTION_FILE_PATH: src/ext-cs2-skin-changer.sln
  BUILD_PLATFORM: x64
  BUILD_CONFIGURATION: Release

jobs:
  build:
    runs-on: windows-latest
    
    steps:
    - name: Checkout repository
      uses: actions/checkout@v3
      with:
        submodules: recursive

    - name: Setup MSBuild
      uses: microsoft/setup-msbuild@v1.1
      with:
        msbuild-version: latest

    - name: Setup NuGet
      uses: NuGet/setup-nuget@v1.1
      with:
        nuget-version: latest

    - name: Restore NuGet packages
      run: nuget restore "${{ env.SOLUTION_FILE_PATH }}"

    - name: Build solution
      run: msbuild "${{ env.SOLUTION_FILE_PATH }}" /m /p:Configuration=${{ env.BUILD_CONFIGURATION }} /p:Platform=${{ env.BUILD_PLATFORM }}

    - name: Verify build output
      run: |
        if (!(Test-Path "x64\${{ env.BUILD_CONFIGURATION }}\ext-cs2-skin-changer.exe")) {
          Write-Host "Build failed: executable not found"
          exit 1
        }
        Write-Host "Build successful: executable found"
        Get-Item "x64\${{ env.BUILD_CONFIGURATION }}\ext-cs2-skin-changer.exe"

    - name: Create artifacts directory
      run: mkdir build_artifacts

    - name: Copy executable to artifacts
      run: copy "x64\${{ env.BUILD_CONFIGURATION }}\ext-cs2-skin-changer.exe" "build_artifacts\"

    - name: Copy dependencies to artifacts
      run: |
        $dllPath = "x64\${{ env.BUILD_CONFIGURATION }}"
        if (Test-Path $dllPath) {
          copy "$dllPath\*.dll" "build_artifacts\" -ErrorAction SilentlyContinue
        }

    - name: Upload build artifacts
      uses: actions/upload-artifact@v3
      with:
        name: CS2-SkinChanger-${{ env.BUILD_PLATFORM }}-${{ env.BUILD_CONFIGURATION }}
        path: build_artifacts/
        retention-days: 30

    - name: Upload release asset (on release)
      if: github.event_name == 'release'
      uses: softprops/action-gh-release@v1
      with:
        files: |
          build_artifacts/ext-cs2-skin-changer.exe
          build_artifacts/*.dll
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

    - name: Build Info
      run: |
        Write-Host "=== Build Information ==="
        Write-Host "Repository: ${{ github.repository }}"
        Write-Host "Commit: ${{ github.sha }}"
        Write-Host "Branch: ${{ github.ref }}"
        Write-Host "Event: ${{ github.event_name }}"
        Write-Host "Platform: ${{ env.BUILD_PLATFORM }}"
        Write-Host "Configuration: ${{ env.BUILD_CONFIGURATION }}"
        Write-Host "========================"
'@

Set-Content -Path "$workflowDir/build.yml" -Value $buildWorkflow -Encoding UTF8
Write-Host "✓ Created build.yml workflow file" -ForegroundColor Green

# Create .gitignore for build artifacts
$gitignore = @'
# Build outputs
x64/
Debug/
Release/
*.obj
*.exe
*.dll
*.lib
*.pdb

# NuGet
packages/
*.nupkg
*.nuspec

# Visual Studio
.vs/
*.vcxproj.user
*.VC.db
*.VC.VC.opendb
.vscode/
.idea/

# IDE files
*.sln.iml
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db

# Build artifacts from GitHub Actions
build_artifacts/
'@

Set-Content -Path ".gitignore" -Value $gitignore -Encoding UTF8
Write-Host "✓ Created/updated .gitignore" -ForegroundColor Green

# Create GitHub CI/CD documentation
$ciDoc = @'
# GitHub Actions CI/CD Setup

## Overview

This project is configured with GitHub Actions to automatically build the CS2 Skin Changer executable when code is pushed to the repository.

## Features

✅ **Automatic Builds**
- Builds on every push to main, master, or develop branches
- Builds on pull requests
- Builds on release creation

✅ **Artifact Management**
- Automatically uploads compiled executable
- Includes any required DLLs
- 30-day retention for artifacts

✅ **Release Automation**
- Automatically attaches executables to GitHub releases
- Includes all compiled files

## How It Works

### Build Trigger Events
1. **Push to Main Branch**: Automatically builds and uploads artifacts
2. **Pull Requests**: Verifies pull requests build successfully
3. **Releases**: Builds and attaches executable to release

### Build Process
1. Checks out code
2. Sets up MSBuild and NuGet
3. Restores NuGet packages (openssl, curl, nlohmann/json, DirectX)
4. Compiles in Release x64 configuration
5. Verifies executable was created
6. Uploads artifacts to GitHub

## Setup Instructions

### Initial Setup (One Time)

1. **Clone Repository**
   ```bash
   git clone https://github.com/YOUR_USERNAME/cs2-skin-changer.git
   cd cs2-skin-changer
   ```

2. **Initialize Git (if not already done)**
   ```bash
   git init
   git add .
   git commit -m "Initial commit with GitHub Actions setup"
   ```

3. **Add Remote**
   ```bash
   git remote add origin https://github.com/YOUR_USERNAME/cs2-skin-changer.git
   ```

4. **Push to GitHub**
   ```bash
   git push -u origin main
   ```

### After Initial Setup

The workflow runs automatically on:
- Every push to main, master, or develop branches
- Every pull request
- Release creation

## Downloading Build Artifacts

### From GitHub Actions

1. Go to your repository on GitHub
2. Click "Actions" tab
3. Select the latest successful workflow
4. Scroll to "Artifacts" section
5. Download "CS2-SkinChanger-x64-Release"

### From Releases

1. Go to your repository on GitHub
2. Click "Releases" 
3. Download the latest release executable

## Workflow File

The workflow file is located at: `.github/workflows/build.yml`

Key configuration:
- **Platform**: x64
- **Configuration**: Release
- **Runner**: Windows Latest
- **Solution**: `src/ext-cs2-skin-changer.sln`

## Troubleshooting

### Build Fails with "NuGet Restore Failed"
- Check internet connectivity
- Verify packages.config is present
- Check NuGet sources are accessible

### Build Fails with "MSBuild not found"
- The runner automatically sets up VS 2022 with MSBuild
- No manual setup needed

### Artifacts Not Uploading
- Check workflow file paths are correct
- Verify executable was created
- Check GitHub Actions logs for errors

### Release Upload Fails
- Ensure GITHUB_TOKEN is available (automatic)
- Check file permissions
- Verify artifacts exist before upload

## Manual Build Commands

If you need to build locally without GitHub Actions:

```bash
# Restore NuGet packages
nuget restore src/ext-cs2-skin-changer.sln

# Build with MSBuild
msbuild src/ext-cs2-skin-changer.sln /p:Configuration=Release /p:Platform=x64

# Output will be at: x64\Release\ext-cs2-skin-changer.exe
```

## GitHub Secrets (If Needed)

For advanced workflows, you can set repository secrets:

1. Go to Settings → Secrets and variables → Actions
2. Add custom secrets as needed

Current workflow uses only `GITHUB_TOKEN` which is automatically available.

## Environment Variables

Configured in workflow:
- `SOLUTION_FILE_PATH`: Path to Visual Studio solution
- `BUILD_PLATFORM`: x64 architecture
- `BUILD_CONFIGURATION`: Release mode

## Performance Notes

- Build time: ~5-10 minutes depending on cache
- Artifacts retained for 30 days
- Concurrent builds limited by GitHub (typically 1-2 per account)

## Next Steps

1. Push this code to your GitHub repository
2. Watch Actions tab for build status
3. Download artifacts once build completes
4. Test the compiled executable

Enjoy automated builds! 🚀
'@

Set-Content -Path "GITHUB_ACTIONS_SETUP.md" -Value $ciDoc -Encoding UTF8
Write-Host "✓ Created GITHUB_ACTIONS_SETUP.md documentation" -ForegroundColor Green

Write-Host ""
Write-Host "✅ GitHub Actions setup complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Initialize git repository (if not done): git init"
Write-Host "2. Add all files: git add ."
Write-Host "3. Commit: git commit -m 'Initial commit with GitHub Actions'"
Write-Host "4. Add remote: git remote add origin https://github.com/YOUR_USERNAME/cs2-skin-changer.git"
Write-Host "5. Push: git push -u origin main"
Write-Host ""
Write-Host "Read GITHUB_ACTIONS_SETUP.md for detailed information!" -ForegroundColor Green
