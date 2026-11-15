# Version 5.2.2 (102) Release

## ADD 

- The automatic cleaning settings page now shows the last run time; if never run/configured, it displays as empty
- Added app cleaning rule preset: WeChat (com.tencent.mm)

## UPDATE

- App splash screen reworked to follow relevant guidelines; no longer displays a custom splash on startup
- File organization no longer shows items with a count of 0
- Timed language auto-refresh interval changed to 5 minutes, fixing delayed language updates
- App code optimization: removed dead code, updated some legacy code
- Removed the useless Tab bar at the bottom of the App to improve the full-screen experience
- App storage cache cleaning no longer reports errors on failure, since most failures are simply due to the cache directory not existing
- Directory reorganization: whitelist files are no longer scattered in the config directory but placed in a dedicated directory
- The whitelist now applies to app private directories in internal storage, preventing junk and empty folders from being cleaned within them
- Custom rule cleaning now displays the total cleaned size

## FIX

- [Test] Tested fix for disappearing downloaded themes in `Xiaomi Themes` and abnormal image loading in `TIM`
- Fixed a bug where the search box in the App chooser interface merged into the status bar
