#!/system/bin/sh
# 此脚本来自ClearBox模块，用于运行DexOat
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
SystemDexOatServer()
{
echo " » 系统默认Dexoat..."
cmd package bg-dexopt-job
}

AllDexOat()
{
echo " » 自定义模式编译：$option_mode..."
cmd package compile -m "$option_mode" -f -a
}
######
option_mode="$2"
AppPackage="$3"
case "$1" in
    SYSTEM_DEXOAT)
        SystemDexOatServer
        ;;
    FAST_DEXOAT)
        [ -z "$option_mode" ] && echo " » ERROR：需要一个参数，未传入模式！" && exit 1
        AllDexOat
        ;;
    *)
        [ -z "$1" ] && echo " » ERROR：需要一个参数，未传入选项名称！" && exit 1
        ;;
esac
######
exit 0
