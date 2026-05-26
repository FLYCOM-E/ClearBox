# Version 5.1.0 (92)

## Add

- Timed supports automatic hot-reload configuration, with extensive logic optimizations
- A simple storage binding feature (to be expanded later):

Configuration directory: **/data/adb/wipe_cache/BindConfigs**

Create a custom-named **.conf** file in this directory and enter your custom mount rules:

```
<Bound Directory> <Target Directory>
```

Note: Please do not fill in **absolute paths**, but rather **paths relative to internal storage**, otherwise it will cause the mount to fail. Paths with spaces are not supported yet

When writing to/creating directories/files inside **<Bound Directory>**, they will be redirected to **<Target Directory>**

This feature currently cannot run on **sdcardfs** devices, but you don't need to worry, **Android 12+** basically no longer has devices using **sdcardfs**

## Update

- App UI optimized, target TARGET upgraded to Android 17, no longer compatible with devices below Android 6.0, fixed abnormal stretching of dialogs when running on Pad
- Internal storage cleanup and file categorization use Android standard storage mount paths, tested and fixed Android 12+ ghost file/folder issues
- System package manager call reverted to using PM instead of CMD to ensure compatibility with more devices
- Cache prevention whitelist no longer displays system APP by default
- Removed the apply changes immediately button (already useless
- F2FS GC changed to background execution + real-time notification method
