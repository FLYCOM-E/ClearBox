#!/system/bin/sh
# 此脚本来自ClearBox模块，用于内部储存固定功能
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
data_dir="/data/media/0"
######
case "$1" in
    STOP)
      chattr +i "$data_dir"
      echo " » 已开启内部储存固定！"
      [ "$stopstorage" = 0 ] && sed -i 's/stopstorage=0/stopstorage=1/g' "$work_dir/settings.prop"
      ;;
    RESET)
      chattr -i "$data_dir"
      echo " » 已关闭内部储存固定！"
      sed -i 's/stopstorage=1/stopstorage=0/g' "$work_dir/settings.prop"
      ;;
esac
######
[ -z "$1" ] && echo " » ERROR：未传入参数！" && exit 1
######
exit 0
