#!/system/bin/sh
# 此脚本来自ClearBox模块，用于模块安装
exec 2>>/dev/null
SKIPUNZIP=1
######
update=0
if ClearBox -v >/dev/null; then
    work_dir=$(ClearBox -w)
    home_dir=$(ClearBox -h)
    if [ -d "$work_dir" ] && [ -d "$home_dir" ]; then
        source "$work_dir/settings.prop"
        update=1
    fi
fi
uninstall()
{
    rm "$ZIPFILE"
    rm -r "$MODPATH"
}
######
if unzip -oq "$ZIPFILE" -d "$MODPATH"; then
    chmod 700 "$MODPATH/system/bin"/*
    chown root:root "$MODPATH/system/bin"/*
else
    uninstall
    echo -e " » 模块解压发生错误！An error occurred while extracting the module!\n"
    exit 1
fi
######
echo -e "=====================================================\n"
echo -e " » 请选择语言 Please select a language：\n"
echo -e " » 音量上键 Volume Up：    中文 Chinese"
echo -e " » 音量下键 Volume Down：  英语 English\n"
case "$(getevent -qlc 1 2>/dev/null)" in
    *KEY_VOLUMEUP*)
      Lang="zh-CN"
      ;;
    *KEY_VOLUMEDOWN*)
      Lang="en-US"
      ;;
    *)
      uninstall
      echo " » 选择错误 Tick Error！\n"
      exit 1
      ;;
esac
mv "$MODPATH/语言包/$Lang.conf" "$MODPATH/语言包/Local.conf"
source "$MODPATH/语言包/Local.conf"
######
echo -e "=====================================================\n"
if [ -d "/data/adb/magisk" ]; then
    echo -e " » $USEMAGISKROOT 🔥\n"
elif [ -d "/data/adb/ap" ]; then
    echo -e " » $USEAPATCHROOT 🔥\n"
elif [ -d "/data/adb/ksu" ]; then
    echo -e " » $USEKERNELSUROOT 🔥\n"
else
    echo -e " » $CHECKROOTERROR! \n"
    sleep 0.1
    echo -e " » $INSTALLERROR❗\n"
    uninstall
    exit 1
fi
######
echo -e "=====================================================\n"
if pm list package -3 | grep "wipe.cache.module" >/dev/null 2>&1; then
    sleep 0.1
    echo -e " » $TICKUPDATEAPP\n"
    sleep 1
    echo -e " » $TICKUPDATEAPP_1\n"
else
    sleep 0.1
    echo -e " » $TICKUPDATEAPP_2\n"
    sleep 1
    echo -e " » $TICKUPDATEAPP_3\n"
fi
######
case "$(getevent -qlc 1 2>/dev/null)" in
    *KEY_VOLUMEUP* | *KEY_VOLUMEDOWN*)
      sleep 0.1
      echo -e " » $NOTINSTALLAPP💔\n"
      ;;
    *)
      if [ "$update" = 1 ]; then
          if [ "$stopinstall" = 1 ]; then
              "$home_dir/BashCore" StopInstall RESET >/dev/null
              RESET=1
          fi
      fi
      sleep 0.1
      echo -e " » $INSTALLAPP❤\n"
      cp "$MODPATH/APKS/ClearBox_$Lang.apk" "$TMPDIR/"
      chmod +x "$TMPDIR/ClearBox_$Lang.apk"
      if pm install -r "$TMPDIR/ClearBox_$Lang.apk" >/dev/null; then
          sleep 0.1
          echo -e " » $INSTALL_APP_SUCCESSFUL✅\n"
      else
          sleep 0.1
          echo -e " » $INSTALL_APP_FAILED❌\n"
          sleep 0.1
          echo -e " » $INSTALL_APP_TRY\n"
          sleep 0.1
          if [ ! -f "$MODPATH/APKS/ClearBox_$Lang.apk" ]; then
              echo -e " » $APKNOTFIND\n"
              uninstall; exit 1
          elif [ ! -f "$TMPDIR/ClearBox_$Lang.apk" ]; then
                echo -e " » $APKUNUP\n"
                rm -r "$TMPDIR"
                mkdir -p "$TMPDIR"
                cp "$MODPATH/APKS/ClearBox_$Lang.apk" "$TMPDIR"
                [ ! -f "$TMPDIR/ClearBox_$Lang.apk" ] && echo -e " » 提取失败！\n"; uninstall; exit 1
          fi
          chmod +x "$TMPDIR/ClearBox_$Lang.apk"
          pm uninstall "wipe.cache.module" >/dev/null
          if pm install -r "$TMPDIR/ClearBox_$Lang.apk" >/dev/null; then
              sleep 0.1
              echo -e " » $INSTALL_APP_SUCCESSFUL✅\n"
          else
              sleep 0.1
              echo -e " » $INSTALL_APP_FAILED❌\n"
              sleep 0.1
              echo -e " » $INSTALLERROR\n"
          fi
      fi
      rm "$TMPDIR/ClearBox_$Lang.apk" >/dev/null 2>&1
      rm -rf "$MODPATH/APKS" >/dev/null 2>&1
      [ "$RESET" = 1 ] && "$home_dir/BashCore" StopInstall STOP >/dev/null
      ;;
esac
######
rm -r "$MODPATH/META-INF" >/dev/null
######
echo -e "=====================================================\n"
sleep 0.1
echo -e " » $INSTALLTHANKS🎉🎉🎉\n"
sleep 0.1
echo -e " » $INSTALLDONE ✨\n"
echo -e "=====================================================\n"
