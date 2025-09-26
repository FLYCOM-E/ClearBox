#!/system/bin/sh
#此脚本来自ClearBox模块，用于根据传入参数调用清理脚本
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
# Off SELinux
[ "$(getenforce)" = "Enforcing" ] && setenforce 0 && OffSelinux=1
######
# 清理第三方软件缓存
clear_cache()
{
sh "$home_dir/wipe_cache/data_cache.sh" &
wait
echo "[ $(date) ]：清理软件缓存" >> "$work_dir/运行日志.log"
}
######
# 清理系统软件缓存
clear_scache()
{
sh "$home_dir/wipe_cache/system_cache.sh" &
wait
echo "[ $(date) ]：清除系统缓存" >> "$work_dir/运行日志.log"
}
######
# 运行处理自定义规则
list_dir()
{
sh "$home_dir/wipe_cache/wipe_list_dir.sh" &
wait
echo "[ $(date) ]：运行规则清理" >> "$work_dir/运行日志.log"
}
######
# 清理储存目录
all_dir()
{
sh "$home_dir/wipe_cache/wipe_all_dir.sh" &
wait
echo "[ $(date) ]：清理储存目录" >> "$work_dir/运行日志.log"
}
######
# 运行规则清理、清理储存目录
dir_file()
{
sh "$home_dir/wipe_cache/wipe_list_dir.sh"
sh "$home_dir/wipe_cache/wipe_all_dir.sh"
echo "[ $(date) ]：运行规则清理、清理储存目录" >> "$work_dir/运行日志.log"
}
######
# 根据prop配置，清理全部文件（仅用于自动清理，该选项打开状态
clear_tar()
{
if [ "$clearall" = 1 ]; then
    sh "$home_dir/wipe_cache/ClearService1.sh" ALL &
    sh "$home_dir/wipe_cache/ClearService2.sh" ALL &
    wait
    echo "[ $(date) ]：清理所有文件" >> "$work_dir/运行日志.log"
fi
}
######
# 自定义格式文件清理
FileClear()
{
sh "$home_dir/wipe_cache/ClearService1.sh" "$1" &
sh "$home_dir/wipe_cache/ClearService2.sh" "$1" &
wait
echo "[ $(date) ]：清理 $1 文件" >> "$work_dir/运行日志.log"
}
######
# 自定义软件 规则清理
ClearApp()
{
sh "$home_dir/wipe_cache/AppClean.sh" "$1" &
wait
echo "[ $(date) ]：清理 $1 软件" >> "$work_dir/运行日志.log"
}
######
# 自定义格式文件归类
file_all()
{
sh "$home_dir/wipe_cache/file_1.sh"
sh "$home_dir/wipe_cache/file_2.sh"
echo "[ $(date) ]：运行文件归类" >> "$work_dir/运行日志.log"
}
######
# 根据prop决定是否运行文件归类
file_all2()
{
if [ "$fileall" = 1 ]; then
    sh "$home_dir/wipe_cache/file_1.sh"
    sh "$home_dir/wipe_cache/file_2.sh"
    echo "[ $(date) ]：运行文件归类" >> "$work_dir/运行日志.log"
fi
}
######
# 磁盘GC
f2fs_GC()
{
sh "$home_dir/wipe_cache/f2fs_GC.sh" F2FS_GC &
wait
echo "[ $(date) ]：GC磁盘优化" >> "$work_dir/运行日志.log"
}
######
# 快速GC
fast_GC()
{
sh "$home_dir/wipe_cache/f2fs_GC.sh" FAST_GC &
wait
}
######
# Dexoat 优化1：触发系统Dexoat
Dexoat_SYSTEM_DEXOAT()
{
sh "$home_dir/wipe_cache/Dexoat.sh" SYSTEM_DEXOAT &
wait 
}
######
# Dexoat 优化2：自定义模式Dexoat
Dexoat_FAST_DEXOAT()
{
sh "$home_dir/wipe_cache/Dexoat.sh" FAST_DEXOAT "$1" &
wait 
}
######
# 其它优化，打开原生墓碑
freezer()
{
sh "$home_dir/wipe_cache/FreeZer.sh" &
wait
}
######
# 根据输入参数执行对应操作
case $1 in
    ClearAll)
      echo "[ $(date) ]：一键优化" >> "$work_dir/运行日志.log"
      clear_cache &
      dir_file &
      clear_tar &
      file_all2 &
      wait
      fast_GC &
      freezer &
      wait
      ;;
    ClearCache)
      clear_cache
      ;;
    Clear_SCache)
      clear_scache
      ;;
    List_Dir)
      list_dir
      ;;
    All_Dir)
      all_dir
      ;;
    File_Clear)
      FileClear "$2"
      ;;
    Clear_App)
      ClearApp "$2"
      ;;
    File_All)
      file_all
      ;;
    F2fs_GC)
      f2fs_GC
      ;;
    Dexoat_1)
      Dexoat_SYSTEM_DEXOAT
      ;;
    Dexoat_2)
      Dexoat_FAST_DEXOAT "$2"
      ;;
    Freezer)
      freezer
      ;;
esac
######
# Reset SELinux
[ "$OffSelinux" = 1 ] && setenforce 1
######
[ -z "$1" ] && echo " » ERROR：需要一个参数，未输入清理项名称！" && exit 1
######
exit 0
