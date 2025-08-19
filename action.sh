#!/system/bin/sh
#此脚本来自ClearBox模块，用于提供一键优化清理（Root管理器端）按钮
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
sh "$home_dir/all.sh" ClearAll &
wait

exit 0
