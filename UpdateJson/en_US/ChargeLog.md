Update Log:

After 5 months of development, ClearBox 5.0 is finally here 🎉!  
This is not just a simple update, but a complete rewrite from Shell to C.

### Architecture Updates

Shell → Completely rewritten in C  
All 11 core modules rewritten in pure C  
Significant reduction in system command calls  
Major speed and performance improvements!

App rule cleaning, cache cleaning, Dexoat optimization  
F2FS garbage collection, file categorization, cleaning, cache suppression  
Internal/external storage cleaning, rule-based cleaning  
Installation blocking, storage locking

## Some Optimizations

### 1. Timed Scheduling System

Completely replaces crond, implemented independently  
Fixes various bugs caused by crond, such as tasks disappearing after a reboot  
Lower resource usage, supports custom tasks

### 2. StopCached

Prevents cache from the source

Adaptive sleep mode:  
Frequent switches: 10-second interval for quick response  
Long periods of inactivity: 60-second interval for power saving  
Automatically skips redundant operations

### 3. Storage Management

Fixed storage: Solves internal storage clutter problems  
Storage cleaning: Deep cleaning of empty folders, junk, empty files, media caches, etc.  
File categorization: Deep-first file cleaning and categorization

### 4. Custom Function Support

Custom storage rule cleaning  
Custom app rules

### 5. More Features

F2FS garbage collection optimization  
Dexoat optimization

## 🌍 Multi-language Support

English (en_US)  
Simplified Chinese (zh_CN)  
Traditional Chinese (zh_TW)

Note: Terminal UI currently only supports Simplified Chinese

## Multi-architecture Support

Automated builds via GitHub Actions (Thanks to @Drsexo for the contribution!)

ARM64 - Most modern devices  
ARM32 (armv7a) - Older devices  
x86_64 - Intel/AMD tablets  
i686 - Older Intel devices

## Breaking Changes

Configuration format changed from XML to CONF  
The cron-based scheduler is replaced by the Timed daemon  
Binary structure has been reorganized (automatically migrated during installation)

Please agree to configuration overwriting when installing!

## PRs Welcome!

5 months of waiting, it's worth the anticipation.