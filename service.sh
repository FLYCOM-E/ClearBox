#!/system/bin/sh
#此脚本来自ClearBox模块，用于完成模块初始 & 设置 & 加载
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
function StartSettings()
{
if [ ! -d "$work_dir" ]; then
    mkdir -p "$work_dir"
    touch "$work_dir/settings.prop"
    touch "$work_dir/ClearBox模块配置目录"
fi
######
if ! grep ^"stopcache=" "$work_dir/settings.prop" >/dev/null; then
    echo "stopcache=0" >> "$work_dir/settings.prop"
fi
if ! grep ^"stopinstall=" "$work_dir/settings.prop" >/dev/null; then
    echo "stopinstall=0" >> "$work_dir/settings.prop"
fi
if ! grep ^"clearall=" "$work_dir/settings.prop" >/dev/null; then
    echo "clearall=0" >> "$work_dir/settings.prop"
fi
if ! grep ^"fileall=" "$work_dir/settings.prop" >/dev/null; then
    echo "fileall=0" >> "$work_dir/settings.prop"
fi
if ! grep ^"cleardisk=" "$work_dir/settings.prop" >/dev/null; then
    echo "cleardisk=1" >> "$work_dir/settings.prop"
fi
if ! grep ^"Fileall_Disk=" "$work_dir/settings.prop" >/dev/null; then
    echo "Fileall_Disk=1" >> "$work_dir/settings.prop"
fi
if ! grep ^"ClearApk_disk=" "$work_dir/settings.prop" >/dev/null; then
    echo "ClearApk_disk=1" >> "$work_dir/settings.prop"
fi
if ! grep ^"ClearZip_disk=" "$work_dir/settings.prop" >/dev/null; then
    echo "ClearZip_disk=1" >> "$work_dir/settings.prop"
fi
if ! grep ^"ClearFont_disk=" "$work_dir/settings.prop" >/dev/null; then
    echo "ClearFont_disk=0" >> "$work_dir/settings.prop"
fi
if ! grep ^"ClearIso_disk=" "$work_dir/settings.prop" >/dev/null; then
    echo "ClearIso_disk=0" >> "$work_dir/settings.prop"
fi
######
if [ ! -f "$work_dir/whitelist.prop" ]; then
    touch "$work_dir/whitelist.prop"
fi
if [ ! -f "$work_dir/ClearWhitelist.prop" ]; then
    touch "$work_dir/ClearWhitelist.prop"
fi
if [ ! -d "$work_dir/文件格式配置" ]; then
    if [ -d "$home_dir/ProFile" ]; then
        cp -r "$home_dir/ProFile" "$work_dir/文件格式配置"
    else
        echo "[ $(date) ] No such ProFile! Please Reinstall the module." > "$work_dir/运行日志.log"
    fi
fi
if [ ! -d "$work_dir/清理配置" ]; then
    rm -rf "$work_dir/清理配置"
    mkdir -p "$work_dir/清理配置"
fi
######
chmod -R 700 "$work_dir"
######
}
######
while true; do
    if [ "$(getprop sys.boot_completed)" = "1" ]; then
        if [ -d "/storage/emulated/0" ]; then
            break
        fi
    fi
sleep 5
done
######
StartSettings
echo "[ $(date) ] ReStart" > "$work_dir/运行日志.log"
######
if grep "stopcache=1" "$work_dir/settings.prop" >/dev/null; then
    echo -n "* * * * * StopCache Stop" > "$home_dir/CRON/StopCache/root"
    echo "[ $(date) ]：阻止缓存运行" >> "$work_dir/运行日志.log"
else
    echo -n "" > "$home_dir/CRON/StopCache/root"
fi
"$bin_dir/busybox" crond -c "$home_dir"/CRON/StopCache
######
if ! grep "sh" "$home_dir/CRON/ClearCache/root" >/dev/null; then
    if grep "sh" "$work_dir/root_backup" >/dev/null; then
        cp "$work_dir/root_backup" "$home_dir/CRON/ClearCache/root"
    fi
fi
"$bin_dir/busybox" crond -c "$home_dir"/CRON/ClearCache
######
if ! grep "sh" "$home_dir/CRON/FileAll/root" >/dev/null; then
    if grep "sh" "$work_dir/root_backup2" >/dev/null; then
        cp "$work_dir/root_backup2" "$home_dir/CRON/FileAll/root"
    fi
fi
"$bin_dir/busybox" crond -c "$home_dir"/CRON/FileAll
######
if ! grep "sh" "$home_dir/CRON/ClearDir/root" >/dev/null; then
    if grep "sh" "$work_dir/root_backup3" >/dev/null; then
        cp "$work_dir/root_backup3" "$home_dir/CRON/ClearDir/root"
    fi
fi
"$bin_dir/busybox" crond -c "$home_dir"/CRON/ClearDir
######
if grep "stopinstall=1" "$work_dir/settings.prop" >/dev/null; then
    chmod 551 /data/app
elif grep "stopinstall=0" "$work_dir/settings.prop" >/dev/null; then
    chmod 771 /data/app
fi
######
exit 0
