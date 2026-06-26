# Version 5.1.2 (94)

# ADD
- File organization now supports setting custom organization directory, supports absolute/relative paths
- System cache cleaning added: `/data/anr` `/data/tombstones`
- Added cleaning rules:
```
info.muge.appshare   #AppShare
com.coolapk.market   #Coolapk
```

# UPDATE
- Cleanup size display now supports dynamic unit display, F2FS-GC statistics now show size instead of segment count
- App cache cleaning optimized for multi-user performance, no longer repeatedly fetches app list
- File organization size limit feature now fully supports per-extension configuration
- Timed, StopCached process signal handling optimized
- Rule directory binding now supports single-quote wrapped path parsing
- LOG recording mechanism updated
- App UI layout updated
- Comment support optimized

# FIX
- Timed hot-reload now includes move event monitoring, fixed issue where atomic config modification did not trigger update
- Fixed configuration backup abnormality
