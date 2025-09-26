#!/system/bin/sh
# ClearBox
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！"
    exit 1
fi
######
home_dir=${0%/*}
bin_dir=$(ClearBox -b)
work_dir=$(ClearBox -w)
exec 2>>/dev/null
######
set=0
while true; do
    if [ "$(getprop sys.boot_completed)" = "1" ] || [ -d "/storage/emulated/0/" ]; then
        break
    fi
    if [ "$set" = 120 ]; then
        touch "$home_dir/disable"
        exit 1
    fi
    set=$((set + 1))
    sleep 5
done
######
function StartSettings()
{
    function PropSetting()
    {
    if [ -z "$stopcache" ]; then
        echo "stopcache=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$stopinstall" ]; then
        echo "stopinstall=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$clearall" ]; then
        echo "clearall=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$fileall" ]; then
        echo "fileall=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$cleardisk" ]; then
        echo "cleardisk=1" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$Fileall_Disk" ]; then
        echo "Fileall_Disk=1" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$ClearApk_disk" ]; then
        echo "ClearApk_disk=1" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$ClearZip_disk" ]; then
        echo "ClearZip_disk=1" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$ClearFont_disk" ]; then
        echo "ClearFont_disk=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$ClearIso_disk" ]; then
        echo "ClearIso_disk=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ -z "$DebugPro" ]; then
        echo "DebugPro=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    }
    ######
    function prosetting()
    {
    mkdir -p "$work_dir/清理规则"
    mkdir -p "$work_dir/清理配置"
    mkdir -p "$work_dir/文件格式配置"
    mkdir -p "$work_dir/CRON/StopCache"
    mkdir -p "$work_dir/CRON/ClearCache"
    mkdir -p "$work_dir/CRON/FileAll"
    mkdir -p "$work_dir/CRON/ClearDir"
    ######
    if [ ! -f "$work_dir/whitelist.prop" ]; then
        touch "$work_dir/whitelist.prop"
    fi
    if [ ! -f "$work_dir/ClearWhitelist.prop" ]; then
        touch "$work_dir/ClearWhitelist.prop"
    fi
    ######
    if [ "$(ls "$work_dir/文件格式配置/")" = "" ]; then
        if [ -d "$home_dir/ProFile" ]; then
            cp -r "$home_dir/ProFile/"* "$work_dir/文件格式配置/"
        else
            echo "[ $(date) ] No Find ProFile! Please Reinstall Module." > "$work_dir/运行日志.log"
        fi
    fi
    ls "$work_dir/文件格式配置/" | while read f; do
        name1=$(echo "$f" | cut -f1 -d '.')
        name2=$(echo "$f" | cut -f2 -d '.')
        if [ ! "$name2" = "conf" ]; then
            mv "$work_dir/文件格式配置/$name1.$name2" "$work_dir/文件格式配置/$name1.conf"
        fi
    done
    }
######
if [ -d "$work_dir" ]; then
    source "$work_dir/settings.prop"
    PropSetting &
    prosetting &
    wait
    if [ "$ReSource" = 1 ]; then
        source "$work_dir/settings.prop"
    fi
else
    mkdir -p "$work_dir"
    touch "$work_dir/settings.prop"
    touch "$work_dir/ClearBox模块配置目录"
    PropSetting &
    prosetting &
    wait
    source "$work_dir/settings.prop"
fi
chmod -R 700 "$work_dir"
}
######
StartSettings
echo "====== ReStart Time $(date) ======" > "$work_dir/运行日志.log"
######
if [ "$stopcache" = 1 ]; then
    killall StopCache 2>/dev/null; nohup setsid StopCache >>/dev/null &
    echo -n "*/10 * * * * killall StopCache 2>/dev/null; nohup setsid StopCache >>/dev/null &" > "$work_dir/CRON/StopCache/root"
else
    echo -n "" > "$work_dir/CRON/StopCache/root"
fi
######
if [ "$stopinstall" = 1 ]; then
    chmod 551 /data/app
else
    chmod 771 /data/app
fi
######
if ! pgrep -f "crond -c $work_dir/CRON/" > /dev/null; then
    pkill -f "crond -c $work_dir/CRON/"
    "$bin_dir/busybox" crond -c "$work_dir/CRON/StopCache" &
    "$bin_dir/busybox" crond -c "$work_dir/CRON/ClearCache" &
    "$bin_dir/busybox" crond -c "$work_dir/CRON/FileAll" &
    "$bin_dir/busybox" crond -c "$work_dir/CRON/ClearDir" &
    echo "[ $(date) ]：CRON运行" >> "$work_dir/运行日志.log"
fi
######
exit 0