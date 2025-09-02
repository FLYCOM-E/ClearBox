#!/system/bin/sh
# ClearBox
MODDIR=${0%/*}
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！"
    exit 1
fi
######
home_dir="$MODDIR"
bin_dir=$(ClearBox -b)
work_dir=$(ClearBox -w)
exec 2>>/dev/null
######
set=0
while true; do
    if [ "$(getprop sys.boot_completed)" = "1" ]; then
        if [ -d "/storage/emulated/0" ]; then
            break
        fi
    fi
    set=$((set + 1))
    if [ "$set" = 120 ]; then
        touch "$home_dir/disable"
        exit 1
    fi
sleep 5
done
######
function StartSettings()
{
    function PropSetting()
    {
    if [ "$stopcache" = "" ]; then
        echo "stopcache=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$stopinstall" = "" ]; then
        echo "stopinstall=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$clearall" = "" ]; then
        echo "clearall=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$fileall" = "" ]; then
        echo "fileall=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$cleardisk" = "" ]; then
        echo "cleardisk=1" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$Fileall_Disk" = "" ]; then
        echo "Fileall_Disk=1" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$ClearApk_disk" = "" ]; then
        echo "ClearApk_disk=1" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$ClearZip_disk" = "" ]; then
        echo "ClearZip_disk=1" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$ClearFont_disk" = "" ]; then
        echo "ClearFont_disk=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$ClearIso_disk" = "" ]; then
        echo "ClearIso_disk=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    if [ "$DebugPro" = "" ]; then
        echo "DebugPro=0" >> "$work_dir/settings.prop"
        ReSource=1
    fi
    }
    ######
    function prosetting()
    {
    if [ -f "$work_dir/文件格式配置" ]; then
        rm -r "$work_dir/文件格式配置"
    else
        mkdir -p "$work_dir/文件格式配置"
    fi
    if [ "$(ls "$work_dir/文件格式配置/")" = "" ]; then
        if [ -d "$home_dir/ProFile" ]; then
            cp -r "$home_dir/ProFile/"* "$work_dir/文件格式配置/"
        else
            echo "[ $(date) ] No such ProFile! Please Reinstall the module." > "$work_dir/运行日志.log"
        fi
    fi
        ls "$work_dir/文件格式配置/" | while read f; do
        name1=$(echo "$f" | cut -f1 -d '.')
        name2=$(echo "$f" | cut -f2 -d '.')
        if [ ! "$name2" = "conf" ]; then
            mv "$work_dir/文件格式配置/$name1.$name2" "$work_dir/文件格式配置/$name1.conf"
        fi
    done
    if [ ! -f "$work_dir/whitelist.prop" ]; then
        touch "$work_dir/whitelist.prop"
    fi
    if [ ! -f "$work_dir/ClearWhitelist.prop" ]; then
        touch "$work_dir/ClearWhitelist.prop"
    fi
    if [ ! -d "$work_dir/清理配置" ]; then
        rm -rf "$work_dir/清理配置"
        mkdir -p "$work_dir/清理配置"
    fi
    if [ ! -d "$work_dir/清理规则" ]; then
        rm -rf "$work_dir/清理规则"
        mkdir -p "$work_dir/清理规则"
    fi
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
mkdir -p "$work_dir/CRON/StopCache" &
mkdir -p "$work_dir/CRON/ClearCache" &
mkdir -p "$work_dir/CRON/FileAll" &
mkdir -p "$work_dir/CRON/ClearDir" &
wait
"$bin_dir/busybox" crond -c "$work_dir/CRON/StopCache" &
"$bin_dir/busybox" crond -c "$work_dir/CRON/ClearCache" &
"$bin_dir/busybox" crond -c "$work_dir/CRON/FileAll" &
"$bin_dir/busybox" crond -c "$work_dir/CRON/ClearDir" &
wait
######
if [ "$stopcache" = 1 ]; then
    echo -n "* * * * * StopCache Stop" > "$work_dir/CRON/StopCache/root"
    echo "[ $(date) ]：阻止缓存运行" >> "$work_dir/运行日志.log"
else
    echo -n "" > "$work_dir/CRON/StopCache/root"
fi
######
if [ "$stopinstall" = 1 ]; then
    chmod 551 /data/app
elif [ "$stopinstall" = 0 ]; then
    chmod 771 /data/app
fi
######
exit 0