# Contributing to ClearBox

Thanks for offering to help! ClearBox is a mostly solo-maintained project (at least for now :-) ), so any help — even a single config line — is appreciated by the developer.
Please pick a role below based on your strengths. **The first two need no coding.**



## Ways to Contribute

| Direction | What you do | Where it goes |
|---|---|---|
| Config | Write and share your cleanup/organization/automation rules | Built-in config or community rule set |
| Translation | Improve multilingual translation strings | `.docs/README_*.md` or in-app / TUI text |
| Testing | Build on your device/ROM and report bugs | Issues or Telegram group |
| Code | Work on the C core, the Kr-Script app, and build scripts | Pull Requests & Push |



### Config (writing & debugging configs)

Everything below is plain text — if you can edit a `.txt`, you can contribute:

Preset configs:
- `AppSource/clearbox/src/main/assets/usr/Timed_xml/` — automation tasks
- `FileConfigs/` — file cleanup / categorization rules
- `AppConfigs/` — app private storage rules

Custom configs:
- `/data/adb/wipe_cache/CleanConfigs` — messy-directory cleanup rules
- `/data/adb/wipe_cache/BindConfigs` — storage redirection rules

Syntax reference: check the README sections for details. Rules to follow:

- One PR (or one file) corresponds to one rule file, named after what it cleans
- Some rules use relative paths — please **don't** hardcode paths starting with root `/xxx`
- `#` comments should ideally take a full line (inline `#ext` comments at the end of a line are the only exception)
- Please test rules on your own device and include the test device / ROM / Android version in the PR.
- Submit preset configs to the preset directories above and **open a PR**
- For custom configs, we recommend sharing them with the community or in the Telegram group



### Translation (polishing strings)

Found a typo, awkward wording, or an untranslated string? PRs are welcome.
Please try to only change **one language per PR** for faster review.
The project currently includes the following languages — pick the one you're good at:
- **`en-US / zh-CN / zh-TW / ru-RU / ko-KR / ja-JP`**
(What, you found one that's missing :-)? Then we'll have to trouble you)



### Testing (feature improvements & bug reports)

When reporting a problem, please include:

- Device & Android version:
- ROM (MIUI / HyperOS / AOSP...)
- Root solution: Magisk / APatch / KernelSU + version
- ClearBox version (Version Code)
- File system (if known): f2fs / ext4 / sdcardfs
- What actually happened vs expected behavior
- Terminal output or logs (if any)



### Code (feature additions / bug fixes)

- Configure **NDK** and set the environment variable **ANDROID_NDK_HOME** to the **NDK** root directory path
- If you're deeply involved in **App** development and debugging, set up the **Android SDK** environment
- Here are some basic environment setup steps:

Base package tools

``` shell
# Debian, Ubuntu
sudo apt install zip make build-essential

# Arch
sudo pacman -S zip make base-devel

# Alpine
sudo apk add zip make build-base

# Fedora
sudo dnf install zip make
sudo dnf groupinstall "Development Tools"
```

Clone the project (for reference only — fork your own if you're submitting a PR)

``` shell
git clone https://github.com/FLYCOM-E/ClearBox
cd ClearBox/ && chmod +x *

export TARGET_API=23         # adjust the target API version as needed
export TARGET_ABI=aarch64   # currently: aarch64, armv7a, x86_64, riscv64
```

Build the core

``` shell
./build.sh       # add --build-apk to build the APK
```

Commit style: keep the existing style: `Fix: ...` / `Update: ...` / `Add: ...`; try to keep each commit to one related change, and add a newline if there are multiple changes.
PR requirements: one PR = one feature or one fix. Describe what you changed and **why** (if needed. A real failure case you hit is the best "why").



## 🚫 Please don't

- Please don't file a "it doesn't work" issue without the device info above — without it the maintainer may not be able to reproduce it at all
- Feature ideas: before writing lots of code, please open an Issue to discuss first, so you don't waste your time
- Don't mix formatting changes with logic changes in the same commit
- Don't submit untested regex-guess rules



## ✍️ Discussion channels

- Issues: for **Bug** and feature tracking
- Telegram (most active): [**Group**](https://t.me/clearbox_group)



## ❤️ Acknowledgements

This project is licensed under [GPL-3.0](https://github.com/FLYCOM-E/ClearBox/blob/main/LICENSE) ~ by contributing, you agree your work is distributed under the same license.
Contributors will be listed here. Your name could be one of the first on this list.



### Thanks to the following contributors:

[**Amktiao**](https://www.coolapk.com/u/925348): indirectly provided the F2FS-GC principle

[**Drsexo**](https://github.com/Drsexo): added the GitHub Action config

[**Creeper005bili**](https://github.com/Creeper005bili): helped test the module early on
