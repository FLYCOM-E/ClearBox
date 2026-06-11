# Version 5.1.1 (93)

# ADD
- StopCached supports immediate unlocking when adding to whitelist
- App whitelist now supports multi-select

# UPDATE
- Replaced language detection API, fixed language detection failures on some devices
- Optimized module installation flow: if App is already installed, directly install instead of asking for update
- Storage redirect now supports quoted path parsing; turning off the bind switch immediately unmounts

# FIX
- Fixed bug where Timed config directory was not created on first installation, thanks to TG@'Ezekiel King'
- App fixed Splash Background and Dialog Background colors not following Monet theme
