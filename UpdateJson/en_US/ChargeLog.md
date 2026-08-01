# Version 5.1.3-Beta (95)

# ADD
- F2FS GC now includes disk protection, won't trigger when dirty segments are too low. Configurable minimum DIRTY limit
- Added cleaning rules:
```
com.lemon.lv   #Jianying (CapCut)
```

# UPDATE
- Updated App cleaning rule copy logic, automatically removes configurations for uninstalled apps
- App cache cleaning optimized, improved cleaning speed
- Updated command-line call parameters

# FIX
- StopCached, Timed now support hot-reload for current language, fixed bug where Daemon still used old language for notifications after system language update
- Fixed bug where directory binding feature could still be toggled on devices that don't support it
- Fixed bug where file cleaning would error out and not clean if the target organization directory does not exist
