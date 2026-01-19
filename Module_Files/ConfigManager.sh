#!/system/bin/sh
#此脚本来自ClearBox模块，用于提供终端菜单UI
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！Please grant root privileges!"
    exit 1
fi
if [ ! -f "/data/adb/wipe_cache/PATH" ]; then
    echo " » Error：PATH读取错误！PATH read error!"
    exit 1
else
    source "/data/adb/wipe_cache/PATH"
fi
######
backupDir="/sdcard/Android/ClearBox"
tempDir="/data/local/ClearBox_temp"
source "$work_dir/settings.prop"
if [ "$DebugPro" = 1 ]; then
    exec 2>>"$work_dir/LOG.log"
else
    exec 2>>/dev/null
fi
source "$home_dir/语言包/Local.conf"
Version=$(grep "version=" "$home_dir/module.prop" | cut -f2 -d "=")
######
case "$1" in
    backup)
      mkdir -p "$tempDir"
      mkdir -p "$backupDir"
      cd "$tempDir"
      cp -r "$work_dir"/* "$tempDir/"
      rm "$tempDir/运行日志.log" "$tempDir/RunStart" "$tempDir/PATH"
      
      if "$bin_dir/busybox" tar -cjf "ClearBox_Config_$Version.tar.bz2" * >/dev/null 2>&1; then
          if mv "ClearBox_Config_$Version.tar.bz2" "$backupDir"; then
              echo " » $BACKUP_SUCCESSFUL"
              echo " » 路径PATH“$backupDir/ClearBox_Config_$Version.tar.bz2”"
          else
              echo " » $BACKUPERR_PATH"
          fi
      else
          echo " » $BACKUPERR_TAR"
      fi
      rm -r "$tempDir"
      ;;
    recovery)
      if ! echo "$2" | grep ".bz2" >/dev/null 2>&1; then
          echo " » $RECOVERYERR_FILECHECK"
          exit 1
      fi
      if "$bin_dir/busybox" tar -xjf "$2" -C "$work_dir/" >/dev/null 2>&1; then
          echo " » $RECOVERY_SUCCESSFUL"
      else
          echo " » $RECOVERY_FAILED"
          if [ ! -f "$2" ]; then
              echo " » $RECOVERY_CHECKERR_1"
          fi
      fi
      ;;
    *)
      echo " » $CONFIG_RUN_ERR"
      ;;
esac
