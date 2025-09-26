#!/system/bin/sh
#此脚本来自ClearBox模块，用于完全卸载模块并清理残留
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
# 还原模块设置并执行卸载
uninstall_md()
{
sed -E -i 's/(stopinstall|stopcache)=1/\1=0/g' "$work_dir/settings.prop"
sh "$home_dir/service.sh"
touch "$home_dir/disable"
touch "$home_dir/remove"
}
######
uninstall_md
echo " » 3秒后卸载软件！"
echo " » 感谢您的使用与支持，欢迎下次光临😋！！"
sleep 3
pm uninstall "wipe.cache.module"
rm -r "$work_dir"
rm -r "$home_dir"
exit 0
