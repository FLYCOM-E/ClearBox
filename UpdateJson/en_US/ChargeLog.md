# Version 5.1.4-Beta (97)

# ADD
- [BETA] Added storage analysis feature with custom storage analysis directory support
- Added Russian language adaptation

# UPDATE
- Timed no longer exits when no config is present at startup; it now sleeps and waits. The date KEY is no longer required in configs – it is automatically appended by the program. All fields are now case‑insensitive
- App multilingual improvements: using global variables to dynamically replace UI text, reducing duplicate files
- Improved F2FS GC node detection for better compatibility with more devices
- Optimized file classification/cleaning with parallel processing for internal/external storage

# FIX
- Fixed excessive empty lines in LOG files
- Fixed directory emptiness check errors in some scripts
