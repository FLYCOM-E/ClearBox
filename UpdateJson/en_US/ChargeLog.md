# Version Code 87

## Add
- Timed: Added time range functionality. A new "in" field has been added to the configuration, to be filled as follows:

```
in=Start Hour/End Hour (Runs between Hour X and Hour X. This is an optional parameter; please do not exceed 23. Note: non-daily tasks may run multiple times within this window)
```

Example:

```conf
time=1/D
date=0
run=/system/bin/sync
in=0/5
post=Test Run/Executed "/system/bin/sync"
```

The above configuration will run the **`/system/bin/sync`** command once between 0:00 and 5:00 AM daily and send a notification.

- One-Tap Clean: Added a new cleaning item: "App Rule Cleanup," which cleans all apps in one go.
- Beta release of cleaning rules for certain apps; the module will automatically handle the following:

```
Google Play  : com.android.vending
Google Photos: com.google.android.apps.photos
GMS          : com.google.android.gms
Google Input : com.google.android.inputmethod.latin
YouTube      : com.google.android.youtube
Edge         : com.microsoft.emmx
Outlook      : com.microsoft.office.outlook
Yunhu        : com.yhchat.app
Pinduoduo Merchant: com.xunmeng.merchant
```

## Update
- Timed: Configuration execution now supports Shell syntax; added minute alignment.
- Fixed bugs in App Rule Cleanup.
- Fixed a bug where the configuration backup script used the wrong log filename for deletion.
- Fixed an issue where cleanup statistics were invisible in Terminal TUI.
- Optimized Shell scripts.
