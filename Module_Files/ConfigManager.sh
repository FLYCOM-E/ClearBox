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
backupDir="/sdcard/Documents/ClearBox"
tempDir="/data/local/ClearBox_temp"
source "$work_dir/settings.prop"
lang_dir="LANG"
if getprop persist.sys.locale | grep "Hans" 2>/dev/null; then
    source "$home_dir/$lang_dir/zh_CN.conf"
elif getprop persist.sys.locale | grep "Hant" 2>/dev/null; then
    source "$home_dir/$lang_dir/zh_TW.conf"
else
    source "$home_dir/$lang_dir/en_US.conf"
fi
Version=$(grep "version=" "$home_dir/module.prop" | cut -f2 -d "=")
######
case "$1" in
    backup)
      mkdir -p "$tempDir"
      mkdir -p "$backupDir"
      cd "$tempDir"
      cp -r "$work_dir"/* "$tempDir/"
      rm -f "$tempDir/LOG.log" "$tempDir/LOG.log.bak" "$tempDir/RunStart" "$tempDir/PATH"
      
      if tar -cjf "ClearBox_Config_$Version.tar.bz2" * >/dev/null 2>&1; then
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
      if tar -xjf "$2" -C "$work_dir/" >/dev/null 2>&1; then
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
