# Version Code 86

## Add
- Added setting option: whether to run fast disk optimization during one-click/automatic cleaning
- Added button wait timeout for module installation (default 10s)
- Added fast disk optimization to Disk & Software Optimization
- Added FSTRIM to fast disk optimization options

## Update
- Optimized error handling when Root environment is abnormal without BusyBox
- Fixed a potential buffer overflow bug in FileManager
- Fixed a small bug in the uninstall process
- Removed Post component (useless)
- Optimized settings information retrieval
- More code optimizations
- Optimized terminal UI

## Break
- Removed old update transition code, direct upgrade from V4.9.0 and earlier versions may retain old configurations
- PROP attribute naming changed
