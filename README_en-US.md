![image](https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/ClearBox.png)



   # What is **ClearBox**?

**ClearBox** is a deep & fast cleanup module that can one-click delete all software caches, installation packages, compressed archives, junk files, and empty folders for your device, block cache, file classification, prevent software updates/installations, and perform periodic optimization/cleanup & file classification (no background). It supports adding custom cleanup rules and file cleanup/organization.

Execution time is as fast as ✈️ compared with ordinary cleanup software on the market.

__Compatible with [**Magisk**](https://github.com/topjohnwu/Magisk)、[**APatch**](https://github.com/bmax121/APatch)、[**KernelSU**](https://github.com/tiann/KernelSU) and their branch managers__



   # Installation?

   ## [**Releases**](https://github.com/FLYCOM-E/ClearBox/releases) or Build from source:
``` shell
#(Please configure the NDK and set the variable $NDK to the NDK root directory path)

sudo apt install zip make

git clone https://github.com/FLYCOM-E/ClearBox
cd ClearBox && chmod +x *

export M_API=21 #(Can be adjusted according to the required target API version)
export M_LANG=en_US
export M_TARGET=aarch64 #(Options: aarch64, armv7a, x86_64, i686)

bash build.sh 
```
Flash **ClearBox_en_US.zip**



>[!CAUTION]
> The main branch currently differs significantly from Releases. General users are advised to download the Release version directly, while adventurous users can compile and experience it themselves (not recommended). The latest Release version does not yet support internationalization!



   # How to use:

> ：Install and use the module **ClearBox** app (asked during module installation

> ：Enable the terminal **UI** feature by typing **`ClearBox`** in any terminal



   # Brief functions:
   
## **1**：One-click optimization & cleanup

Perform fast one-click optimization/cleanup, the effect is equivalent to periodic optimization. Including cache cleanup, junk cleanup, fast disk **GC**, etc. According to module settings, file cleanup/organization can also be run



## **2**：Clean/Kill custom directories

This function is rule-based cleanup. You can add cleanup rules yourself in the **`/data/adb/wipe_cache/清理配置/`** directory.

Configuration method:

Rules use text format. Optionally fill in **`@/xxx`** (starting from the root directory) on the first line of the rule file to specify the initial directory. Different rule files are independent.

One file/directory path per line. If you have specified the initial path, you can fill in only the relative (initial directory) path instead of the full path.



## **3**：Clear junk files & empty folders

Clean internal/external storage empty folders, multimedia cache, software cache, etc.



## **4**：Clear all software cache

As the name suggests, supports **SD** expansion card software cache cleanup



## **5**：Deep file cleanup

File cleanup function, comes with some cleanup configurations, supports custom file format configuration.

Configuration method:

Configuration directory： **`/data/adb/wipe-cache/文件格式配置`**

Configuration way：Create a **conf** document file in the configuration directory. The file name will be displayed in the cleanup page as the corresponding cleanup item name. Fill in the custom file suffixes in this file, separated by spaces.

The software/terminal cleanup page will be dynamically built according to the configuration file

Scope：file cleanup, and the file classification mentioned later



## **6**：Software rule cleanup

One of the rule cleanup functions, used to clean the internal private directory of software according to rules

Configuration method：

Configuration directory： **`/data/adb/wipe-cache/清理规则`**

Configuration way：Create a **conf** file in the configuration directory. The file name is arbitrary but must not contain special symbols, spaces, etc.
Fill in **`@<package_name>/<app_name>`** at the beginning of the first line to specify the software corresponding to this rule;
After that, one folder path or file path per line. The initial directory is the **App** data directory.

The module will automatically find the data directory corresponding to the package name. The software name will be displayed in the cleanup interface

The software/terminal cleanup page will be dynamically built according to the configuration file

Please do not hard-code cleanup rules with root directory **`/xxx`** in the rules, otherwise cleanup will report an error and skip this rule file.

The module will automatically check some configuration errors



## **7**：Clear system cache

Clear system software cache, system cache, and also clear **MTP** host data (can solve **MTP** file transfer file list display abnormality)



## **8**：Automatic Cleaning

Feature automation, driven by the **Timed** component for execution (ultra-low overhead).

** Did You Know? **

You can use this module's **Timed** feature to run custom tasks! Please configure as follows:

Configuration directory：**`/data/adb/wipe_cache/TimedConfig`**

Create a custom name **.conf** file in this directory, fill in and modify the following three lines：

```conf
time=time/unit (currently supports M/minutes, H/hours, D/days)
date=0
run=program to run and parameters(note: parameters like 「"", ''」are currently not supported and will be incorrectly parsed)
```

Example：

```conf
time=1/H
date=0
run=/system/bin/true
```

The above configuration will run the **`/system/bin/true`** command once every hour.

After filling and saving, you need to manually execute **Make Current Configuration Effective Immediately** or **Restart** for **the task to take effect!**

Note：Not suitable for time-precise tasks. Daily tasks will only run between 0-6 AM！



## **9**：Block All Software Update Installations
As the name implies, file system-level forced blocking of software installations



## **10**：Internal Storage Fix Function
As the name implies, locks the internal storage root directory layout to prevent various applications from randomly creating files/folders



## **11**：Block cache function
This is an important function of the module, used to dynamically block foreground software from generating cache.

After turning on this function and restarting to activate, it will automatically cache the latest 5 foreground software and block cache. You can customize and add a whitelist.

Whitelist location： **`/data/adb/wipe_cache/whitelist.prop`** or add it in the module **ClearBox** app.

**Other**：The module adopts a dynamic wait update method (10s & 30s), on the one hand to avoid continuous resource occupation, on the other hand, some software will be abnormal when launched under the cache block state. Delayed activation of cache blocking after entering the software is the optimal solution.

Blocking cache will not clear the software cache, so the software cache will not be **0**, but will grow slowly. This avoids continuous small file erasure and only limits large cache generation

Extremely low overhead, automatically delays execution when the screen is off

If some software is abnormal, please add it to the whitelist yourself!



## **12**：Magnet (one-click file classification

One-click classify internal/multi-external storage files, rules shared with file cleanup



## **13**：Disk & software optimization

This is an optimization function set, including disk optimization ( **f2fs-GC** ) and **Dexoat** (software pre-compilation) functions


**F2fs** disk **GC**, this function belongs to emergency **GC**, so it does not need to be used frequently, just run it regularly.
The solution comes from the big guy **Coolapk@Amktiao**, and has been agreed, thanks to the big guy!


**Dexoat** software pre-compilation, that is, pre-compile software to accelerate runtime speed, the disadvantage is that it takes up storage space, which belongs to the space-electricity-for-smooth operation ()
The module provides three modes in advance: system default **Dexoat**, custom compilation mode, **RESET**:

System default **Dexoat**： **Android** system (especially newer systems) will also automatically compile some hotspot functions when idle to improve overall running speed, so the module provides system **Dexoat** mode, this mode is equivalent to manually triggering system **Dexoat**, the mode and compilation items are determined by the system.

Custom compilation mode：This mode supports manually specifying the compilation mode, currently supports **Speed**, **Speed-profile**, **Everything** three modes

**RESET**：Restore compilation



> The module **ClearBox App** is made using the [**Kr-Script**](https://github.com/helloklf/kr-scripts) project, thanks to the project

> For module beta, please go to
>[**ClearBox**](https://yhfx.jwznb.com/share?key=yigOTedUjh62&ts=1747355950)
>[More play-machine communication](https://yhfx.jwznb.com/share?key=TihFLlAj6ZJ9&ts=1740281856)
