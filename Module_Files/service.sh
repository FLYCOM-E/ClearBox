#!/system/bin/sh
# ClearBox
[ ! "$(whoami)" = "root" ] && echo " » 请授予root权限！Please grant root privileges!" && exit 1
######
export home_dir=${0%/*}
###### The first stage. wait for boot = 1, timeout auto disable module
first_stage=0
set=0
while [ "$(getprop sys.boot_completed)" != 1 ]; do
    if [ "$set" = 120 ]; then
        touch "$home_dir/disable"
        exit 1
    fi
    first_stage=1
    set=$((set + 1))
    sleep 5
done
######
export work_dir="/data/adb/wipe_cache"
if [ -d "/data/adb/magisk" ]; then
    export bin_dir="/data/adb/magisk"
elif [ -d "/data/adb/ap/bin" ]; then
    export bin_dir="/data/adb/ap/bin"
elif [ -d "/data/adb/ksu/bin" ]; then
    export bin_dir="/data/adb/ksu/bin"
else
    # 这是一个回退方案，很明显一般系统不会带 busybox
    # 以后可能会内置 chattr 指令集
    ln -s /system/bin/toybox "$home_dir/bin/busybox"
    export bin_dir="$home_dir/bin"
fi
mkdir -p "$work_dir"
echo -en "home_dir=$home_dir\nwork_dir=$work_dir\nbin_dir=$bin_dir" > "$work_dir/PATH"
######
sh "$home_dir/DirtyClear.sh" &
######
StartSettings()
{
    PropSetting()
    {
        [ -z "$stopcache" ] && echo "stopcache=0" >> "$work_dir/settings.prop"
        [ -z "$stopinstall" ] && echo "stopinstall=0" >> "$work_dir/settings.prop"
        [ -z "$stopstorage" ] && echo "stopstorage=0" >> "$work_dir/settings.prop"
        [ -z "$clearall" ] && echo "clearall=0" >> "$work_dir/settings.prop"
        [ -z "$fileall" ] && echo "fileall=0" >> "$work_dir/settings.prop"
        [ -z "$cleardisk" ] && echo "cleardisk=0" >> "$work_dir/settings.prop"
        [ -z "$Fileall_Disk" ] && echo "Fileall_Disk=0" >> "$work_dir/settings.prop"
        [ -z "$FileClear_Disk" ] && echo "FileClear_Disk=0" >> "$work_dir/settings.prop"
        [ -z "$DebugPro" ] && echo "DebugPro=0" >> "$work_dir/settings.prop"
        [ -z "$ClearCacheSize" ] && echo "ClearCacheSize=5" >> "$work_dir/settings.prop"
    }
    ######
    touch "$work_dir/settings.prop" 2>/dev/null
    source "$work_dir/settings.prop"
    PropSetting
    ######
    mkdir -p "$work_dir/清理规则"
    mkdir -p "$work_dir/清理配置"
    mkdir -p "$work_dir/文件格式配置"
    mkdir -p "$work_dir/TimedConfig"
    ######
    [ ! -f "$work_dir/whitelist.prop" ] && touch "$work_dir/whitelist.prop"
    [ ! -f "$work_dir/ClearWhitelist.prop" ] && touch "$work_dir/ClearWhitelist.prop"
    ######
    if [ "$(ls "$work_dir/文件格式配置/")" = "" ]; then
        if [ -d "$home_dir/ProFile" ]; then
            cp -r "$home_dir/ProFile/"* "$work_dir/文件格式配置/"
        else
            echo "[ $(date) ] No Find ProFile! Please Reinstall Module." > "$work_dir/LOG.log"
        fi
    fi
    for f in "$work_dir/文件格式配置"/*; do
        name1=$(echo "$f" | cut -f1 -d '.')
        name2=$(echo "$f" | cut -f2 -d '.')
        [ ! "$name2" = "conf" ] && mv "$work_dir/文件格式配置/$name1.$name2" "$work_dir/文件格式配置/$name1.conf"
    done
    ######
    source "$work_dir/settings.prop"
}
######
# Settings
StartSettings
# Remove backup.log and Backup old log
if [ -f "$work_dir/LOG.log" ]; then
    rm -f "$work_dir/LOG.log.bak"
    mv "$work_dir/LOG.log" "$work_dir/LOG.log.bak"
fi
# Start Log
echo "====== Boot Time $(date) ======" > "$work_dir/LOG.log"
if [ "$first_stage" = 1 ]; then
    exec 2>>"$work_dir/LOG.log"
fi
# Chmod 700
chmod -R 700 "$home_dir/"
chmod -R 700 "$work_dir/"
###### The second stage. wait for storaged up
set=0
while [ ! -d "/storage/emulated/0/" ]; do
    [ "$set" = 60 ] && break
    set=$((set + 1))
    sleep 5
done
if [ "$first_stage" = 1 ]; then
    sleep 30
fi
######
if [ "$stopcache" = 1 ]; then
    if ! pgrep "StopCached" >/dev/null 2>&1; then
        "$home_dir/Daemon/StopCached" -b "$bin_dir" -w "$work_dir"
        echo "[ $(date) ]：StopCached Start" >> "$work_dir/LOG.log"
    fi
fi
if pgrep Timed >/dev/null 2>&1; then
    killall Timed
fi
if "$home_dir/Daemon/Timed" "$work_dir/TimedConfig"; then
    echo "[ $(date) ]：Timed Start" >> "$work_dir/LOG.log"
fi
######
exit 0
