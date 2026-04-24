# Version Code 89

# Add
- App supports predictive back gesture when returning to desktop
- File organization/cleaning supports size range specification：

Configuration Directory： **`/data/adb/wipe-cache/FileConfigs`**

Add **`@MAX=<size/B/K/M/G>`**、**`MIN=<size/B/K/M/G>`** in default/custom configuration files to specify the target file size range. Parsing is overwritable — later declarations will override previous ones, allowing you to leverage this feature to create custom rules

Example：

```
@MAX=1/G @MIN=10/M
zip 7z

@max=-1
iso img

@MIN=10/KB exe
```

All of the above formats are valid. This declaration specifies to only organize **zip**、**7z** files with a maximum size of **1 G** and a minimum size of **10 MB**, while **iso**、**img** files have no maximum size restriction

Note：**-1** means unlimited. **MIN** cannot be greater than **MAX**, otherwise **MIN** will be automatically invalidated. **MAX**、**MIN** and size units are **case-insensitive** (you may have already noticed :⁠-⁠)），**Floating point numbers (decimals) are currently not supported**

## Update
- Migrated Chinese configuration directories to more generic English names
- Built-in essential functions, main program no longer depends on Busybox, no external command invocation overhead
- Timed configuration now supports comments and no longer forcibly overwrites content
- Module has linked all functions into a single binary, improving integration and reducing size to some extent
- Updated icons for some App controls, improved dialog corner rounding
- Fixed issue where App needed to switch or refresh page for dark/light mode to take effect
- Migrated strtok to thread-safe strtok_r, atoi/atol to strtol, NDK 30 Beta now passes compilation
