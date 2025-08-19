#!/system/bin/sh
# 此脚本来自ClearBox模块，用于打开原生安卓墓碑模式（方便调用就写这了 &&
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
#exec 2>>/dev/null
exec 2>>"$work_dir/运行日志.log"
######
if [ "$(device_config put activity_manager_native_boot use_freezer)" = "false" ]; then
    device_config put activity_manager_native_boot use_freezer true
    echo " » 已打开安卓原生墓碑 (^^)"
fi
