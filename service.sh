#!/system/bin/sh
# ClearBox
[ ! "$(whoami)" = "root" ] && echo " » 请授予root权限！" && exit 1
######
home_dir=${0%/*}
bin_dir=$(ClearBox -b)
work_dir=$(ClearBox -w)
exec 2>>/dev/null
######
set=0
while true; do
    [ "$(getprop sys.boot_completed)" = "1" ] || [ -d "/storage/emulated/0/" ] && break
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
    [ -z "$stopcache" ] && echo "stopcache=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$stopinstall" ] && echo "stopinstall=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$stopstorage" ] && echo "stopstorage=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$clearall" ] && echo "clearall=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$fileall" ] && echo "fileall=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$cleardisk" ] && echo "cleardisk=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$Fileall_Disk" ] && echo "Fileall_Disk=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$FileClear_Disk" ] && echo "FileClear_Disk=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$DebugPro" ] && echo "DebugPro=0" >> "$work_dir/settings.prop" && ReSource=1
    [ -z "$ClearCacheSize" ] && echo "ClearCacheSize=5" >> "$work_dir/settings.prop" && ReSource=1
    }
    ######
    prosetting()
    {
    mkdir -p "$work_dir/清理规则"
    mkdir -p "$work_dir/清理配置"
    mkdir -p "$work_dir/文件格式配置"
    mkdir -p "$work_dir/CRON/ClearCache"
    mkdir -p "$work_dir/CRON/FileAll"
    mkdir -p "$work_dir/CRON/ClearDir"
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
if [ -d "$work_dir" ]; then
    source "$work_dir/settings.prop"
    PropSetting &
    prosetting &
    wait
    [ "$ReSource" = 1 ] && source "$work_dir/settings.prop"
else
    mkdir -p "$work_dir"
    touch "$work_dir/settings.prop"
    touch "$work_dir/ClearBox模块配置目录"
    PropSetting &
    prosetting &
    wait
    source "$work_dir/settings.prop"
fi
}
######
StartSettings
echo "====== ReStart Time $(date) ======" > "$work_dir/运行日志.log"
######
if [ "$stopcache" = 1 ]; then
    if ! pgrep -x "StopCache"; then
        nohup setsid StopCache >>/dev/null &
    fi
fi
######
if ! pgrep -f "crond -c $work_dir/CRON/" > /dev/null; then
    pkill -f "crond -c $work_dir/CRON/"
    "$bin_dir/busybox" crond -c "$work_dir/CRON/ClearCache" &
    "$bin_dir/busybox" crond -c "$work_dir/CRON/FileAll" &
    "$bin_dir/busybox" crond -c "$work_dir/CRON/ClearDir" &
    echo "[ $(date) ]：CRON运行" >> "$work_dir/运行日志.log"
fi
######
chmod -R 700 "$work_dir/"
exit 0
