#!/system/bin/sh
# ClearBox
[ ! "$(whoami)" = "root" ] && echo " » 请授予root权限！Please grant root privileges!" && exit 1
######
export home_dir=${0%/*}
export work_dir="/data/adb/wipe_cache"
if [ -d "/data/adb/magisk" ]; then
    export bin_dir="/data/adb/magisk"
elif [ -d "/data/adb/ap/bin" ]; then
    export bin_dir="/data/adb/ap/bin"
elif [ -d "/data/adb/ksu/bin" ]; then
    export bin_dir="/data/adb/ksu/bin"
fi
######
mkdir -p "$work_dir"
echo -en "home_dir=$home_dir\nwork_dir=$work_dir\nbin_dir=$bin_dir" > "$work_dir/PATH"
######
exec 2>/dev/null
######
set=0
while [ "$(getprop sys.boot_completed)" != "1" ]; do
    [ "$set" = 120 ] && touch "$home_dir/disable" && exit 1
    set=$((set + 1))
    sleep 5
done
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
    prosetting()
    {
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
                echo "[ $(date) ] No Find ProFile! Please Reinstall Module." > "$work_dir/运行日志.log"
            fi
        fi
        for f in "$work_dir/文件格式配置"/*; do
            name1=$(echo "$f" | cut -f1 -d '.')
            name2=$(echo "$f" | cut -f2 -d '.')
            [ ! "$name2" = "conf" ] && mv "$work_dir/文件格式配置/$name1.$name2" "$work_dir/文件格式配置/$name1.conf"
        done
    }
    ######
    touch "$work_dir/settings.prop" 2>/dev/null
    source "$work_dir/settings.prop"
    PropSetting &
    prosetting &
    wait
    source "$work_dir/settings.prop"
    touch "$work_dir/ClearBox模块配置目录"
}
######
StartSettings
echo "====== ReStart Time $(date) ======" > "$work_dir/运行日志.log"
######
if [ "$stopcache" = 1 ]; then
    if ! pgrep "StopCached" >/dev/null 2>&1; then
        "$home_dir/bin/StopCached" -b "$bin_dir" -w "$work_dir"
    fi
fi
if pgrep Timed; then
    killall Timed
fi
if "$home_dir/Timed" "$work_dir/TimedConfig"; then
    echo "[ $(date) ]：Timed运行" >> "$work_dir/运行日志.log"
fi
######
chmod -R 700 "$work_dir/"
######
exit 0