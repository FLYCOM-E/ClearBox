# Version 5.1.5 (98)

# ADD
- NCDU history auto‑cleanup
- Japanese language support
- Korean language support

# UPDATE
- NCDU history now uses SQLite3 for data storage
- Optimized app wallpaper mode
- Improved multi‑language matching and reduced redundant logic

# FIX
- Fixed language fallback on devices where `settings get system system_locales` returns null; now falls back to system default first
- Fixed several memory leak issues
