#!/system/bin/sh
#此脚本来自ClearBox模块，用于版本更新过度时清理不需要的配置和垃圾文件
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！"
    exit 1
elif ! ClearBox -v >/dev/null; then
    echo " » 模块加载异常，请排查反馈！"
    exit 1
fi
######
bin_dir=$(ClearBox -b)
home_dir=$(ClearBox -h)
work_dir=$(ClearBox -w)
source "$work_dir/settings.prop"
if [ "$DebugPro" = 1 ]; then
    exec 2>>"$work_dir/运行日志.log"
else
    exec 2>>/dev/null
fi
######
[ -n "$ClearApk_disk" ] && sed -i 's|ClearApk_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearZip_disk" ] && sed -i 's|ClearZip_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearFont_disk" ] && sed -i 's|ClearFont_disk=[0-9]*||g' "$work_dir/settings.prop"
[ -n "$ClearIso_disk" ] && sed -i 's|ClearIso_disk=[0-9]*||g' "$work_dir/settings.prop"
sed -i '/^$/d' "$work_dir/settings.prop"

rm -r "$home_dir/CRON"
rm -f "$work_dir/root_backup"
rm -f "$work_dir/root_backup1"
rm -f "$work_dir/root_backup2"
rm -f "$work_dir/root_backup3"

true
