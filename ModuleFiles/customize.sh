#!/system/bin/sh
# 此脚本来自ClearBox模块，用于模块安装
exec 2>>/dev/null
SKIPUNZIP=1
file_config_dir="FileConfigs"
SHOUT_S="0.3"
LONG_S="1"
TIMEOUT_S="10"
######
update=0
if [ -f "/data/adb/wipe_cache/PATH" ]; then
    source "/data/adb/wipe_cache/PATH"
    source "$work_dir/settings.prop"
    update=1
fi
######
uninstall()
{
    rm "$ZIPFILE"
    rm -r "$MODPATH"
}
###
reinstall()
{
    sleep "$SHOUT_S"
    echo -e " » $INSTALL_APP_FAILED❌\n"
    sleep "$SHOUT_S"
    echo -e " » $INSTALL_APP_TRY\n"
    sleep "$SHOUT_S"
    if [ ! -f "$MODPATH/ClearBox.apk" ]; then
        echo -e " » $APKNOTFIND\n"
        uninstall; exit 1
    elif [ ! -f "$TMPDIR/ClearBox.apk" ]; then
        echo -e " » $APKUNUP\n"
        rm -r "$TMPDIR"
        mkdir -p "$TMPDIR"
        cp "$MODPATH/ClearBox.apk" "$TMPDIR"
        chmod +x "$TMPDIR/ClearBox.apk"
        [ ! -f "$TMPDIR/ClearBox.apk" ] && echo -e " » $APKGETERR\n"; uninstall; exit 1
    fi
    pm uninstall "wipe.cache.module" >/dev/null
    if pm install -r "$TMPDIR/ClearBox.apk" >/dev/null; then
        sleep "$SHOUT_S"
        echo -e " » $SUCCESSFUL✅\n"
    else
        sleep "$SHOUT_S"
        echo -e " » $INSTALL_APP_FAILED❌\n"
        sleep "$SHOUT_S"
        echo -e " » $INSTALLERROR\n"
    fi
}
######
if ! unzip -oq "$ZIPFILE" -d "$MODPATH"; then
    uninstall
    echo -e " » 模块解压发生错误！An error occurred while extracting the module!\n"
    exit 1
fi
######
lang=""
lang_dir="LANG"
local_lang="$(settings get system system_locales | cut -f1 -d ',')"
if echo "$local_lang" | grep -E "zh-CN|Hans" 2>/dev/null; then
    source "$home_dir/$lang_dir/zh_CN.conf"
    lang="zh_CN"
elif echo "$local_lang" | grep -E "zh-TW|Hant" 2>/dev/null; then
    source "$home_dir/$lang_dir/zh_TW.conf"
    lang="zh_TW"
else
    source "$home_dir/$lang_dir/en_US.conf"
    lang="en_US"
fi
######
sed -i "s|@LANG|$lang|g" "$MODPATH/module.prop"
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
    sleep "$SHOUT_S"
    echo -e " » $INSTALLERROR❗\n"
    uninstall
    exit 1
fi
######
sleep "$SHOUT_S"
echo -e "=====================================================\n"
if ! pm list package -3 | grep "wipe.cache.module" >/dev/null 2>&1; then
    echo -e " » $TICKUPDATEAPP\n"
    sleep "$LONG_S"
    echo -e " » $TICKUPDATEAPP_1\n"
    ######
    case "$(timeout "$TIMEOUT_S" getevent -qlc 1 2>/dev/null)" in
        *KEY_VOLUMEUP*)
          sleep "$SHOUT_S"
          echo -e " » $INSTALLAPP❤\n"
          cp "$MODPATH/ClearBox.apk" "$TMPDIR/"
          chmod +x "$TMPDIR/ClearBox.apk"
          if pm install -r "$TMPDIR/ClearBox.apk" >/dev/null; then
              sleep "$SHOUT_S"
              echo -e " » $SUCCESSFUL✅\n"
          else
              reinstall
          fi
          rm "$TMPDIR/ClearBox.apk" >/dev/null 2>&1
          rm -rf "$MODPATH/ClearBox.apk" >/dev/null 2>&1
          ;;
        *)
          sleep "$SHOUT_S"
          echo -e " » $NOTINSTALLAPP💔\n"
          ;;
    esac
else
    echo -e " » $TICKUPDATEAPP_2\n"
    
    if [ "$update" = 1 ]; then
        if [ "$clearbox_stop_install" = 1 ]; then
            "$home_dir/BashCore" StopInstall RESET >/dev/null
            RESET=1
        fi
    fi
    
    cp "$MODPATH/ClearBox.apk" "$TMPDIR/"
    chmod +x "$TMPDIR/ClearBox.apk"
    if pm install -r "$TMPDIR/ClearBox.apk" >/dev/null; then
        sleep "$SHOUT_S"
        echo -e " » $SUCCESSFUL✅\n"
    else
        reinstall
    fi
    rm "$TMPDIR/ClearBox.apk" >/dev/null 2>&1
    rm -rf "$MODPATH/ClearBox.apk" >/dev/null 2>&1
    
    [ "$RESET" = 1 ] && "$home_dir/BashCore" StopInstall STOP >/dev/null
fi
######
sleep "$SHOUT_S"
echo -e "=====================================================\n"
sleep "$SHOUT_S"
echo -e " » $TICKUPDATETUI\n"
sleep "$LONG_S"
echo -e " » $TICKUPDATETUI_1\n"
sleep "$LONG_S"
echo -e " » $TICKTITLE\n"
case "$(timeout "$TIMEOUT_S" getevent -qlc 1 2>/dev/null)" in
    *KEY_VOLUMEUP*)
      rm "$MODPATH/skip_mount" 2>/dev/null
      echo -e " » $SUCCESSFUL✅\n"
      ;;
    *)
      touch "$MODPATH/skip_mount" 2>/dev/null
      echo -e " » $NOTSUCCESS💔\n"
      ;;
esac
######
rm -r "$MODPATH/META-INF" >/dev/null
######
if [ "$update" = 1 ]; then
    sleep "$SHOUT_S"
    echo -e "=====================================================\n"
    sleep "$SHOUT_S"
    echo -e " » $CONFIGTITLE_1\n"
    sleep "$LONG_S"
    echo -e " » $CONFIGTITLE_2\n"
    sleep "$LONG_S"
    echo -e " » $TICKTITLE\n"
    case "$(timeout "$TIMEOUT_S" getevent -qlc 1 2>/dev/null)" in
        *KEY_VOLUMEUP*)
          cp -r "$MODPATH/FileConfigs/"* "$work_dir/$file_config_dir/"
          echo -e " » $SUCCESSFUL✅\n"
          ;;
        *)
          echo -e " » $NOTSUCCESS💔\n"
          ;;
    esac
fi
######
sleep "$SHOUT_S"
echo -e "=====================================================\n"
sleep "$SHOUT_S"
echo -e " » $INSTALLTHANKS🎉🎉🎉\n"
sleep "$SHOUT_S"
echo -e " » $INSTALLDONE ✨\n"
sleep "$SHOUT_S"
echo -e "=====================================================\n"
