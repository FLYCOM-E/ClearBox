#!/system/bin/sh
#此脚本来自ClearBox模块，用于版本更新过度时清理不需要的配置和垃圾文件
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！"
    exit 1
fi
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
source "$work_dir/settings.prop"
exec 2>>/dev/null
######
grep '1=' "$work_dir/RunStart" >>/dev/null 2>&1 || echo "1=" >>"$work_dir/RunStart"
grep '2=' "$work_dir/RunStart" >>/dev/null 2>&1 || echo "2=" >>"$work_dir/RunStart"
grep '3=' "$work_dir/RunStart" >>/dev/null 2>&1 || echo "3=" >>"$work_dir/RunStart"
grep '4=' "$work_dir/RunStart" >>/dev/null 2>&1 || echo "4=" >>"$work_dir/RunStart"
grep '5=' "$work_dir/RunStart" >>/dev/null 2>&1 || echo "5=" >>"$work_dir/RunStart"
grep 'reset=' "$work_dir/RunStart" >>/dev/null 2>&1 || echo "reset=" >>"$work_dir/RunStart"

[ -n "$ClearApk_disk" ] && sed -i 's|ClearApk_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearZip_disk" ] && sed -i 's|ClearZip_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearFont_disk" ] && sed -i 's|ClearFont_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearIso_disk" ] && sed -i 's|ClearIso_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$clearall" ] && sed -i 's|clearall=[0-9]*||g' "$work_dir/settings.prop"
[ "$ClearCacheSize" -gt 100 ] && sed -i 's|ClearCacheSize=[0-9]*|ClearCacheSize=5|g' "$work_dir/settings.prop"
sed -i '/^$/d' "$work_dir/settings.prop"

rm -r "$home_dir/CRON"
rm -r "$work_dir/CRON/StopCache"
rm -f "$work_dir/root_backup"
rm -f "$work_dir/root_backup1"
rm -f "$work_dir/root_backup2"
rm -f "$work_dir/root_backup3"

true
