#!/system/bin/sh
# 此脚本来自ClearBox模块，用于模块安装
exec 2>>/dev/null
SKIPUNZIP=1
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
echo -e "=====================================================\n"
if [ -d "/data/adb/magisk" ]; then
    echo -e " » 您正在使用 Magisk ROOT 🔥\n"
elif [ -d "/data/adb/ap" ]; then
    echo -e " » 您正在使用 APatch ROOT 🔥\n"
elif [ -d "/data/adb/ksu" ]; then
    echo -e " » 您正在使用 KernelSU ROOT 🔥\n"
else
    echo -e " » 未检测到适配的 ROOT 方案或环境异常！\n"
    sleep 0.1
    echo -e " » 安装错误，请联系模块作者适配或排查问题❗\n"
    uninstall && exit 1
fi
######
if unzip -oq "$ZIPFILE" -d "$MODPATH"; then
    chmod 700 "$MODPATH/system/bin"/*
    chown root:root "$MODPATH/system/bin"/*
else
    uninstall && echo -e " » 模块解压发生错误！\n" && exit 1
fi
######
if pm list package -3 | grep "wipe.cache.module" >/dev/null 2>&1; then
    sleep 0.1
    echo -e " » 是否更新模块 ClearBox 软件？\n"
    sleep 1
    echo -e " » 触摸屏幕或其它按键更新，任意音量键取消 &&\n"
else
    sleep 0.1
    echo -e " » 是否安装模块 ClearBox 软件？\n"
    sleep 1
    echo -e " » 触摸屏幕或其它按键安装，任意音量键取消 &&\n"
fi
######
getevent -qlc 1 2>> /dev/null | while read -r A; do
  case "$A" in
    *KEY_VOLUMEUP* | *KEY_VOLUMEDOWN*)
      sleep 0.1
      echo -e " » 已为您取消安装💔\n"
      ;;
    *)
      if [ "$update" = 1 ]; then
          if [ "$stopinstall" = 1 ]; then
              "$home_dir/BashCore" StopInstall RESET >/dev/null
              RESET=1
          fi
      fi
      sleep 0.1
      echo -e " » 正在为您安装 ClearBox 软件❤\n"
      cp "$MODPATH/ClearBox.apk" "$TMPDIR/"
      chmod +x "$TMPDIR/ClearBox.apk"
      if pm install -r "$TMPDIR/ClearBox.apk" >/dev/null; then
          sleep 0.1
          echo -e " » 安装成功！✅\n"
      else
          sleep 0.1
          echo -e " » 安装失败！❌\n"
          sleep 0.1
          echo -e " » 重新尝试安装......\n"
          sleep 0.1
          if [ ! -f "$MODPATH/ClearBox.apk" ]; then
              echo -e " » 安装包文件缺失！请重新在官方渠道下载模块! \n"
              uninstall; exit 1
          elif [ ! -f "$TMPDIR/ClearBox.apk" ]; then
                echo -e " » 模块安装包提取失败！正在尝试重新提取......\n"
                rm -r "$TMPDIR"
                mkdir -p "$TMPDIR"
                cp "$MODPATH/ClearBox.apk" "$TMPDIR"
                [ ! -f "$TMPDIR/ClearBox.apk" ] && echo -e " » 提取失败！\n"; uninstall; exit 1
          fi
          chmod +x "$TMPDIR/ClearBox.apk"
          pm uninstall "wipe.cache.module" >/dev/null
          if pm install -r "$TMPDIR/ClearBox.apk" >/dev/null; then
              sleep 0.1
              echo -e " » 安装成功！✅\n"
          else
              sleep 0.1
              echo -e " » 安装失败！❌\n"
              sleep 0.1
              echo -e " » 请反馈开发或尝试手动安装！\n"
              D=1
          fi
      fi
      rm "$TMPDIR/ClearBox.apk" >/dev/null 2>&1
      rm "$MODPATH/ClearBox.apk" >/dev/null 2>&1
      [ "$RESET" = 1 ] && "$home_dir/BashCore" StopInstall STOP >/dev/null
      ;;
  esac
done
######
rm -r "$MODPATH/META-INF" >/dev/null
######
sleep 0.1
echo -e " » 感谢使用！您的支持是模块开发最大的动力！！🎉🎉🎉\n"
sleep 0.1
echo -e " » 模块安装完成 ✨\n"
echo -e "=====================================================\n"
