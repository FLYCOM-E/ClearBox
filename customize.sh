#!/system/bin/sh
# 此脚本来自ClearBox模块，用于模块安装
exec 2>>/dev/null
SKIPUNZIP=1
home_dir=$(ClearBox -h)
work_dir=$(ClearBox -w)
uninstall()
{
rm "$ZIPFILE"
rm -r "$MODPATH"
}
######
echo "====================================================="
if [ -d "/data/adb/magisk" ]; then
    echo "                              "
    echo " » 您正在使用 Magisk ROOT 🔥"
elif [ -d "/data/adb/ap" ]; then
    echo "                              "
    echo " » 您正在使用 APatch ROOT 🔥"
elif [ -d "/data/adb/ksu" ]; then
    echo "                              "
    echo " » 您正在使用 KernelSU ROOT 🔥"
else
    echo "                              "
    echo " » 未检测到适配的 ROOT 方案或环境异常 ❄"
    sleep 0.1
    echo " » 安装错误，请联系模块作者适配或排查问题❗"
    echo "                              "
    uninstall && exit 1
fi
######
if [ -d "$home_dir" ]; then
    Oldsha256="$(sha256sum "$home_dir/system/bin/StopCache" | cut -f1 -d ' ')"
    MODPATH="$home_dir"
fi
if unzip -oq "$ZIPFILE" -d "$MODPATH"; then
    chmod 700 "$MODPATH/system/bin"/*
    chown root:root "$MODPATH/system/bin"/*
else
    uninstall && echo " » 模块解压发生错误！" && exit 1
fi
Newsha256="$(sha256sum "$home_dir/system/bin/StopCache" | cut -f1 -d ' ')"
######
if $(pm list package -3 | grep "wipe.cache.module" >/dev/null); then
    echo "                              "
    sleep 0.1
    echo " » 是否更新模块 ClearBox 软件？"
    echo "                              "
    sleep 1
    echo " » 触摸屏幕或其它按键更新，任意音量键取消 &&"
    echo "                              "
else
    echo "                              "
    sleep 0.1
    echo " » 是否安装模块 ClearBox 软件？"
    echo "                              "
    sleep 1
    echo " » 触摸屏幕或其它按键安装，任意音量键取消 &&"
    echo "                              "
fi
######
getevent -qlc 1 2>> /dev/null | while read -r A; do
  case "$A" in
    *KEY_VOLUMEUP* | *KEY_VOLUMEDOWN*)
      sleep 0.1
      echo " » 已为您取消安装💔"
      ;;
    *)
      if [ -d "$work_dir" ]; then
          if grep "stopinstall=1" "$work_dir/settings.prop" >/dev/null; then
              chattr -i /data/app
              sa=1
          else
              sa=0
          fi
      fi
      sleep 0.1
      echo " » 正在为您安装 ClearBox 软件❤"
      echo "                              "
      cp "$MODPATH/META-INF/TEMP_RES/"ClearBox*.apk "$TMPDIR"
      chmod 755 "$TMPDIR/"ClearBox*.apk
      if pm install -r "$TMPDIR/"ClearBox*.apk >/dev/null; then
          sleep 0.1
          echo " » 安装成功！✅"
      else
          sleep 0.1
          echo " » 安装失败！❌"
          DE=1
          if [ "$DE" = 1 ]; then
              echo "                              "
              sleep 0.1
              echo " » 检测到方法一安装失败，是否尝试以第二方式安装？"
              echo "                              "
              sleep 0.1
              echo " » 触摸屏幕或其它按键确认，任意音量键取消 &&"
              echo "                              "
              getevent -qlc 1 2>> /dev/null | while read -r A; do
                case "$A" in
                  *KEY_VOLUMEUP* | *KEY_VOLUMEDOWN*)
                    sleep 0.1
                    echo " » 已为您取消安装💔"
                    ;;
                  *)
                    sleep 0.1
                    echo " » 正在尝试安装！"
                    if [ "$(ls "$MODPATH/META-INF/TEMP_RES/"ClearBox*.apk)" = "" ]; then
                        echo " » 模块安装包文件缺失！请重新在官方渠道下载模块"
                        uninstall
                        exit 1
                    elif [ "$(ls "$TMPDIR/"ClearBox*.apk)" = "" ]; then
                        echo " » 模块安装包提取失败！正在尝试重新提取"
                        rm -r "$TMPDIR/*"
                        mkdir -p "$TMPDIR"
                        cp "$(ls "$MODPATH/META-INF/TEMP_RES/"ClearBox*.apk)" "$TMPDIR"
                        [ "$(ls "$TMPDIR/ClearBox*.apk")" = "" ] && echo " » 提取失败！" && uninstall && exit 1
                    fi
                    chmod 755 "$TMPDIR/"ClearBox*.apk
                    pm uninstall "wipe.cache.module" >/dev/null
                    if pm install -r "$TMPDIR/"ClearBox*.apk >/dev/null; then
                        echo "                              "
                        sleep 0.1
                        echo " » 安装成功！✅"
                    else
                        echo "                              "
                        sleep 0.1
                        echo " » 安装失败！❌"
                        echo "                              "
                        sleep 0.1
                        echo " » 请反馈开发或尝试手动安装！"
                        D=1
                    fi
                    ;;
                esac
              done
          fi
      fi
      rm "$TMPDIR"/*.apk
      [ "$sa" = 1 ] && chattr +i /data/app
      ;;
  esac
done
######
rm -r "$MODPATH"/META-INF
######
echo "                              "
sleep 0.1
echo " » 感谢使用！您的支持是模块开发最大的动力！！🎉🎉🎉"
echo "                              "
sleep 0.1
echo " » 模块安装完成 ✨"
echo "                              "
echo "====================================================="
######
if [ "$Newsha256" = "$Oldsha256" ]; then
    echo -e "\n * 无需重启设备\n"
    sh "$home_dir/service.sh" >>/dev/null
    nohup sleep 5 && rm -f "$home_dir/update" &
else
    echo -e "\n * 需要重启设备\n"
fi




