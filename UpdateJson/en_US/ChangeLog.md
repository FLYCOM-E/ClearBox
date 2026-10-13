# Version 5.2.0 (100) Release

## ADD

- Storage analysis (NCDU) now supports displaying symlink types, performance optimization
- Added cleanup of SystemServer crash logs
- Added storage cleanup rule preset:
```
com.tencent.mobileqq   #QQ
```

## UPDATE

- File organization/cleanup concurrency support, allocated based on device CPU core multiplier, measured file organization speed up to x2
- Unified module packaging, automatically extracts the corresponding architecture during installation, no extra storage space used
- App UI theme, layout, animation optimization, upgraded library versions
- Whitelist now also applies to system cache cleanup

## FIX

- Fixed a bug where file operations could freeze in some scenarios
- Fixed abnormal animation in App transparent mode
