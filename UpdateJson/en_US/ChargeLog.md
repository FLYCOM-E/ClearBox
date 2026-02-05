# VersionCode 82 To 83

For the full architecture, please visit [**CI Build**](https://github.com/FLYCOM-E/ClearBox/actions)

## Update:

Removed temporary SELinux disabling during cleanup

Opened up Timed custom configuration, parsing optimization

Fixed an error in redirection that prevented error messages from showing during configuration updates

Full multi-extension storage support, including Micro Card, USB storage, etc.

Blocked updates no longer support external storage extensions. Therefore, if you have an extended storage inserted and have enabled the block install function, please temporarily disable it before updating this version.

### This Issue's Thanks List

Thanks to GitHub @Amogus for submitting CI auto-build configuration

### **•  Did you know?**

You can use this module **Timed** to run custom tasks! Please configure as follows:

Configuration directory: **`/data/adb/wipe_cache/TimedConfig`**

In this directory, create a custom-named **.conf** file and enter and modify the following three lines:

```conf
time=Time/Unit (currently supports M/min, H/hour, D/day)  
date=0  
run=Program and arguments to run (Note that parameters such as “"", ''” are not supported and will be parsed incorrectly)  
post=Notification title/notification content (send notification, this is an optional parameter, not recommended for minute-level tasks)
```

Example:

```conf
time=1/H  
date=0  
run=/system/bin/true  
post=Test Run/Executed “/system/bin/true”
```

The above configuration will run the **`/system/bin/true`** command every hour and send a notification.

After filling it out, save the file. You need to manually run **Apply Current Configuration** or **Restart** for the task to take effect!

Note: Daily tasks will only run between 0:00 and 6:00 AM!
