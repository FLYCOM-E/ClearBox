#!/system/bin/sh
#此脚本来自ClearBox模块，用于提供终端菜单UI
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！"
    exit 1
fi
######
backupDir="/sdcard/Android/ClearBox"
tempDir="/data/local/ClearBox_temp"
work_dir="$1"
bin_dir="$2"
source "$work_dir/settings.prop"
if [ "$DebugPro" = 1 ]; then
    exec 2>>"$work_dir/运行日志.log"
else
    exec 2>>/dev/null
fi
Version=$(ClearBox -v | cut -f3 -d " ")
######
case "$3" in
    backup)
      mkdir -p "$tempDir"
      mkdir -p "$backupDir"
      cd "$tempDir"
      cp -r "$work_dir"/* "$tempDir/"
      
      rm "$tempDir/运行日志.log" "$tempDir/RunStart" 
      if "$bin_dir/busybox" tar -cjf "ClearBox_Config_$Version.tar.bz2" * >/dev/null 2>&1; then
          if mv "ClearBox_Config_$Version.tar.bz2" "$backupDir"; then
              echo " » 配置备份成功！"
              echo " » 路径“$backupDir/ClearBox_Config_$Version.tar.bz2”"
          else
              echo " » 备份失败！$backupDir 路径无法使用！"
          fi
      else
          echo " » 备份失败！压缩包创建失败！"
          if ! "$bin_dir/busybox" tar --help >/dev/null 2>&1; then
              echo " » Tar 命令不存在！！"
          fi
      fi
      rm -r "$tempDir"
      ;;
    recovery)
      if ! echo "$4" | grep ".bz2" >/dev/null 2>&1; then
          echo " » 文件后缀应为 .bz2！请确认是否正确选择文件"
          echo " » 否则可能导入垃圾文件！"
          exit 1
      fi
      if "$bin_dir/busybox" tar -xjf "$2" -C "$work_dir/" >/dev/null 2>&1; then
          echo " » 配置恢复成功！"
      else
          echo " » 配置恢复失败！"
          if [ ! -f "$2" ]; then
              echo " » 传入配置文件不存在！"
          fi
      fi
      ;;
    *)
      echo " » 参数传入错误！"
      ;;
esac
