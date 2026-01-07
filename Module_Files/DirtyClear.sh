#!/system/bin/sh
#此脚本来自ClearBox模块，用于版本更新过度时清理不需要的配置和垃圾文件
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！Please grant root privileges!"
    exit 1
fi
######
if [ ! -f "/data/adb/wipe_cache/PATH" ]; then
    echo " » Error：PATH读取错误！PATH read error!"
    exit 1
else
    source "/data/adb/wipe_cache/PATH"
fi
######
source "$work_dir/settings.prop"
exec 2>>/dev/null
######
[ -n "$ClearApk_disk" ] && sed -i 's|ClearApk_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearZip_disk" ] && sed -i 's|ClearZip_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearFont_disk" ] && sed -i 's|ClearFont_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearIso_disk" ] && sed -i 's|ClearIso_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$clearall" ] && sed -i 's|clearall=[0-9]*||g' "$work_dir/settings.prop"
[ "$ClearCacheSize" -gt 100 ] && sed -i 's|ClearCacheSize=[0-9]*|ClearCacheSize=5|g' "$work_dir/settings.prop"
sed -i '/^$/d' "$work_dir/settings.prop"

rm -r "$home_dir/CRON"
rm -r "$work_dir/CRON"
rm -f "$work_dir/root_backup"
rm -f "$work_dir/root_backup1"
rm -f "$work_dir/root_backup2"
rm -f "$work_dir/root_backup3"

true
