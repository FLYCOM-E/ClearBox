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
#exec 2>>/dev/null
exec 2>>"$work_dir/运行日志.log"
######
# Off SELinux
if [ "$(getenforce)" = "Enforcing" ]; then
    setenforce 0
    OffSelinux=1
fi
######
# 清理第三方软件缓存
function clear_cache()
{
sh "$home_dir/wipe_cache/data_cache.sh" &
wait
echo "[ $(date) ]：清理软件缓存" >> "$work_dir/运行日志.log"
}
######
# 清理系统软件缓存
function clear_scache()
{
sh "$home_dir/wipe_cache/system_cache.sh" &
wait
echo "[ $(date) ]：清除系统缓存" >> "$work_dir/运行日志.log"
}
######
# 运行处理自定义规则
function list_dir()
{
sh "$home_dir/wipe_cache/wipe_list_dir.sh" &
wait
echo "[ $(date) ]：运行规则清理" >> "$work_dir/运行日志.log"
}
######
# 清理储存目录
function all_dir()
{
sh "$home_dir/wipe_cache/wipe_all_dir.sh" &
wait
echo "[ $(date) ]：清理储存目录" >> "$work_dir/运行日志.log"
}
######
# 运行规则清理、清理储存目录
function dir_file()
{
sh "$home_dir/wipe_cache/wipe_list_dir.sh"
sh "$home_dir/wipe_cache/wipe_all_dir.sh"
echo "[ $(date) ]：运行规则清理、清理储存目录" >> "$work_dir/运行日志.log"
}
######
# 根据prop配置，清理全部文件（仅用于自动清理，该选项打开状态
function clear_tar()
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
function FileClear()
{
sh "$home_dir/wipe_cache/ClearService1.sh" "$1" &
sh "$home_dir/wipe_cache/ClearService2.sh" "$1" &
wait
echo "[ $(date) ]：清理 $1 文件" >> "$work_dir/运行日志.log"
}
######
# 自定义软件 规则清理
function ClearApp()
{
sh "$home_dir/wipe_cache/AppClean.sh" "$1" &
wait
echo "[ $(date) ]：清理 $1 软件" >> "$work_dir/运行日志.log"
}
######
# 自定义格式文件归类
function file_all()
{
sh "$home_dir/wipe_cache/file_1.sh"
sh "$home_dir/wipe_cache/file_2.sh"
echo "[ $(date) ]：运行文件归类" >> "$work_dir/运行日志.log"
}
######
# 根据prop决定是否运行文件归类
function file_all2()
{
if [ "$fileall" = 1 ]; then
    sh "$home_dir/wipe_cache/file_1.sh"
    sh "$home_dir/wipe_cache/file_2.sh"
    echo "[ $(date) ]：运行文件归类" >> "$work_dir/运行日志.log"
fi
}
######
# 磁盘GC
function f2fs_GC()
{
sh "$home_dir/wipe_cache/f2fs_GC.sh" F2FS_GC &
wait
echo "[ $(date) ]：GC磁盘优化" >> "$work_dir/运行日志.log"
}
######
# 快速GC
function fast_GC()
{
sh "$home_dir/wipe_cache/f2fs_GC.sh" FAST_GC &
wait
echo "[ $(date) ]：GC磁盘优化" >> "$work_dir/运行日志.log"
}
######
# 其它优化，打开原生墓碑
function freezer()
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
    Freezer)
      freezer
      ;;
esac
######
# Reset SELinux
if [ "$OffSelinux" = 1 ]; then
    setenforce 1
fi
######
if [ "$1" = "" ]; then
    echo " » ERROR：需要一个参数，未输入清理项名称！"
    exit 1
fi
######
exit 0
