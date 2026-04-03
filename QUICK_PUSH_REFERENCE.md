# 🚀 QUICK REFERENCE - Push to GitHub in 60 Seconds

## Command Cheat Sheet

```bash
# Go to project
cd c:\Users\qveezzx\Documents\Changer

# Add all changes
git add -A

# Check what's being committed
git status

# Commit
git commit -m "feat: Implement knife and glove model forcing

- Complete knife model forcing implementation
- Glove model forcing ready for patch
- Comprehensive documentation added

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"

# Push to GitHub
git push -u origin main
```

## One-Liner (Copy & Paste)

```bash
cd c:\Users\qveezzx\Documents\Changer && git add -A && git commit -m "feat: Implement knife and glove model forcing - Complete knife implementation + glove ready for patch + comprehensive docs - Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>" && git push -u origin main
```

## Before You Push

- [ ] GitHub repo exists: https://github.com/qveezzx/skinchangertest
- [ ] Git is installed
- [ ] You have a Personal Access Token (https://github.com/settings/tokens)
- [ ] Or you have SSH keys set up

## After You Push

✅ Visit: https://github.com/qveezzx/skinchangertest  
✅ Changes should be visible  
✅ Documentation shows up  
✅ Build may trigger (if GitHub Actions configured)

## If It Fails

**Error: "Not a git repository"**
```bash
git init
git config user.name "qveezzx"
git remote add origin https://github.com/qveezzx/skinchangertest.git
```

**Error: "Authentication failed"**
- Use Personal Access Token (not password)
- Get from: https://github.com/settings/tokens

**Error: "fatal: The current branch main has no upstream branch"**
```bash
git push -u origin main
```

## Full Guide

See: `GITHUB_PUSH_GUIDE.md`

---

Ready? Just copy a command from above and run it! 🎯
