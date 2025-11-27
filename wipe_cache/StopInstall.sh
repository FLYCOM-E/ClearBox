#!/system/bin/sh
# 此脚本来自ClearBox模块，用于阻止安装功能
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
data_dir="/data/app"
micro_dir="/mnt/expand/$(ls /mnt/expand)/app"
######
case "$1" in
    STOP)
      chattr +i "$data_dir"
      [ -d "$micro_dir" ] && chattr +i "$micro_dir"
      echo " » 已开启阻止更新！"
      [ "$stopinstall" = 0 ] && sed -i 's/stopinstall=0/stopinstall=1/g' "$work_dir/settings.prop"
      ;;
    RESET)
      chattr -i "$data_dir"
      [ -d "$micro_dir" ] && chattr -i "$micro_dir"
      echo " » 已关闭阻止更新！"
      sed -i 's/stopinstall=1/stopinstall=0/g' "$work_dir/settings.prop"
      ;;
esac
######
[ -z "$1" ] && echo " » ERROR：未传入参数！" && exit 1
######
exit 0
