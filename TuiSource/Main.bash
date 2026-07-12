#!/system/bin/env sh
#
# @ClearBoxModule：TUI
#
if [ ! "$(whoami)" = "root" ]; then
    echo " » 请授予root权限！Please grant root privileges!"
    exit 1
fi
######
if [ ! -f "/data/adb/wipe_cache/PATH" ]; then
    echo " » Error：PATH读取错误！Error：PATH read error!"
    exit 1
else
    source "/data/adb/wipe_cache/PATH"
fi
######
Version=$(grep "version=" "$home_dir/module.prop" | cut -f2 -d "=")
Str_1=$(grep "str=" "$home_dir/module.prop" | cut -f2 -d '=' | cut -f1 -d "|")
Str_2=$(grep "str=" "$home_dir/module.prop" | cut -f2 -d "|")
app_config_dir="AppCleanRules"
file_config_dir="FileConfigs"
timed_config_dir="TimedConfigs"
core="clearbox"
######
lang_dir="LANG"
local_lang="$(settings get system system_locales | cut -f1 -d ',')"
if echo "$local_lang" | grep -E "zh-CN|Hans" 2>/dev/null; then
    source "$home_dir/$lang_dir/zh_CN.conf"
elif echo "$local_lang" | grep -E "zh-TW|Hant" 2>/dev/null; then
    source "$home_dir/$lang_dir/zh_TW.conf"
else
    source "$home_dir/$lang_dir/en_US.conf"
fi
######
# 菜单函数
md_menu()
{
clear
echo -e "\033[104m [$L_HOME_HELLO_TITLE]\033[0m"
echo -e "\033[96m ==============================================\033[0m\n"
echo -e "\033[93m  1:\t$L_CLEARALL\033[0m"
echo -e "\033[93m  2:\t$L_CUST_CLEAR\033[0m"
echo -e "\033[93m  3:\t$L_STORAGE_CLEAR\033[0m"
echo -e "\033[93m  4:\t$L_CLEAR_ALL_APP_CACHE\033[0m"
echo -e "\033[93m  5:\t$L_FILE_ALL\033[0m"
echo -e "\033[93m  6:\t$L_FILE_CLEAR\033[0m"
echo -e "\033[93m  7:\t$L_CUST_APP_CLEAR\033[0m"
echo -e "\033[93m  8:\t$L_CLEAR_SYS_CACHE\033[0m"
echo -e "\033[93m  9:\t$L_AUTO_CLEAR\033[0m"
echo -e "\033[93m  10:\t$L_STOP_INSTALL\033[0m"
echo -e "\033[93m  11:\t$L_PATH_BIND\033[0m"
echo -e "\033[93m  12:\t$L_STOP_STORAGE\033[0m"
echo -e "\033[93m  13:\t$L_STOP_APP_CACHE\033[0m"
echo -e "\033[93m  14:\t$L_DISK_APP_PLUS\033[0m"
echo -e "\033[93m  00:\t$L_SETTINGS\033[0m\n"
echo -e "\033[96m ==============================================\033[0m"
echo -e "\t\t   --- $L_HOME_EXIT_TITLE ---"
echo -n " $L_PLEASE_INPUT: "
read input
case "$input" in
    1)
      clear
      "$home_dir/$core" ClearAll
      ;;
    2)
      clear
      "$home_dir/$core" List_Dir
      ;;
    3)
      clear
      "$home_dir/$core" All_Dir
      ;;
    4)
      clear
      "$home_dir/$core" ClearCache
      ;;
    5)
      echo -en " » $L_CONFIRM？(y): "
      read input_fa
      case "$input_fa" in
          y | Y)
            clear
            "$home_dir/$core" File_All
            ;;
          *)
            echo -e "\033[92m » $L_CANCEL\033[0m"
            ;;
      esac
      ;;
    6)
      clear
      if [ -z "$(ls "$work_dir/$file_config_dir/")" ]; then
          echo -e "\033[91m » $L_FILE_CONFIG_NOT_FOUND\033[0m"
          return 1
      fi
      count=0
      echo -e "\n\033[104m [$L_FC_WARN]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      for file in "$work_dir/$file_config_dir"/*; do
         filename=$(basename "$file" | cut -f1 -d ".")
         count=$((count + 1))
         Num[$count]="$count,$filename"
         echo -e "\033[93m $count：$L_CLEAN $filename\033[0m\n"
      done
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT: "
      read fc_input
      count_2=0
      while true; do
         count_2=$((count_2 + 1))
         if [ "$(echo "${Num[$count_2]}" | cut -f1 -d ',')" = "$fc_input" ]; then
             filename=$(echo "${Num[$count_2]}" | cut -f2 -d ',')
             echo -en " » $L_CONFIRM？(y): "
             read fc_input_2
             case "$fc_input_2" in
                 y | Y)
                   clear
                   "$home_dir/$core" File_Clear "$filename"
                   break
                   ;;
                 *)
                   echo -e "\033[92m » $L_CANCEL\033[0m"
                   break
                   ;;
             esac
         fi
         [ "$count_2" = "$count" ] && echo -e "\033[91m » $L_INPUT_ERROR\033[0m" && break
      done
      ;;
    7)
      clear
      if [ -z "$(ls "$work_dir/$app_config_dir/")" ]; then
          echo -e "\033[91m » $L_APP_CONFIG_NOT_FOUND\033[0m"
          return 1
      fi
      count=0
      echo -e "\n\033[104m [$L_AC_WARN]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      for file in "$work_dir/$app_config_dir"/*; do
         appname=$(grep '@' "$file" | head -n1 | cut -f2 -d '/')
         app_package=$(grep '@' "$file" | head -n1 | cut -f1 -d '/' | cut -f2 -d '@')
         count=$((count + 1))
         Num[$count]="$count,$app_package"
         echo -e "\033[93m $count:\t$L_CLEAN $appname\033[0m\n"
      done
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT: "
      read ac_input
      count_2=0
      while true; do
         count_2=$((count_2 + 1))
         if [ "$(echo "${Num[$count_2]}" | cut -f1 -d ',')" = "$ac_input" ]; then
             app_package=$(echo "${Num[$count_2]}" | cut -f2 -d ',')
             clear
             "$home_dir/$core" Clear_App "$app_package"
             break
         fi
         [ "$count_2" = "$count" ] && echo -e "\033[91m » $L_INPUT_ERROR\033[0m" && break
      done
      ;;
    8)
      echo -en " » $L_CONFIRM？(y): "
      read sc_input
      case "$sc_input" in
          y | Y)
            clear
            "$home_dir/$core" Clear_SCache
            ;;
          *)
            echo -e "\033[92m » $L_CANCEL\033[0m"
            ;;
      esac
      ;;
    9)
      clear
      echo -e "\033[104m [$L_AUTO_CLEAR]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -e "\033[93m 1:\t$L_AUTO_RUN $L_CLEARALL\033[0m\n"
      echo -e "\033[93m 2:\t$L_AUTO_RUN $L_CUST_CLEAR\033[0m\n"
      echo -e "\033[93m 3:\t$L_AUTO_RUN $L_STORAGE_CLEAR\033[0m\n"
      echo -e "\033[93m 4:\t$L_AUTO_RUN $L_CLEAR_ALL_APP_CACHE\033[0m\n"
      echo -e "\033[93m 5:\t$L_AUTO_RUN $L_FILE_ALL\033[0m\n"
      echo -e "\033[93m 6:\t$L_AUTO_RUN $L_DISK_GC\033[0m\n"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT:"
      read at_input
      case "$at_input" in
          1)
            clear
            NowTime=$(grep "time=" "$work_dir/$timed_config_dir/ClearAll.conf" | cut -f2 -d "=")
            echo -e " \033[104m[$L_AR_TITLE：$NowTime]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m 1:\t$L_AR_CUST_TIME ($L_UNIT：$L_TIME_D)\033[0m\n"
            echo -e "\033[93m 0:\t$L_CLOSE_FUNCTION\033[0m\n"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -n " $L_PLEASE_INPUT:"
            read at_fast_input
            case "$at_fast_input" in
                1)
                  clear
                  echo -en " » $L_AR_PLEASE_INPUT 1～30 $L_TIME_D："
                  read day_input
                  if ! echo "$day_input" | grep [0-9] >>/dev/null; then
                      echo -e "\033[1;92m » $L_INPUT_NOTIS_NUM\033[0m"
                  elif [ "$day_input" -lt 1 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MIN_ERR 1 $L_TIME_D!\033[0m"
                  elif [ "$day_input" -gt 30 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MAX_ERR 30 $L_TIME_D!\033[0m"
                  else
                      echo -e "time=${day_input}/D\ndate=0\nrun=$home_dir/$core ClearAll\nin=0/5" > "$work_dir/$timed_config_dir/ClearAll.conf"
                      echo " » $L_OPEND!"
                  fi
                  ;;
                0)
                  clear
                  rm "$work_dir/$timed_config_dir/ClearAll.conf"
                  echo " » $L_CLOSED!"
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          2)
            clear
            NowTime=$(grep "time=" "$work_dir/$timed_config_dir/ListDir.conf" | cut -f2 -d "=")
            echo -e " \033[104m[$L_AR_TITLE：$NowTime]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m 1:\t$L_AR_CUST_TIME ($L_UNIT：$L_TIME_H)\033[0m\n"
            echo -e "\033[93m 0:\t$L_CLOSE_FUNCTION\033[0m\n"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -n " $L_PLEASE_INPUT:"
            read at_cust_input
            case "$at_cust_input" in
                1)
                  clear
                  echo -en " » $L_AR_PLEASE_INPUT 1～24 $L_TIME_H："
                  read hour_input
                  if ! echo "$hour_input" | grep [0-9] >>/dev/null; then
                      echo -e "\033[1;92m » $L_INPUT_NOTIS_NUM\033[0m"
                  elif [ "$hour_input" -lt 1 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MIN_ERR 1 $L_TIME_H!\033[0m"
                  elif [ "$hour_input" -ge 24 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MAX_ERR 24 $L_TIME_H!\033[0m"
                  else
                      echo -e "time=${hour_input}/H\ndate=0\nrun=$home_dir/$core List_Dir" > "$work_dir/$timed_config_dir/ListDir.conf"
                      echo " » $L_OPEND!"
                  fi
                  ;;
                0)
                  clear
                  rm "$work_dir/$timed_config_dir/ListDir.conf"
                  echo " » $L_CLOSED!"
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          3)
            clear
            NowTime=$(grep "time=" "$work_dir/$timed_config_dir/ClearDir.conf" | cut -f2 -d "=")
            echo -e " \033[104m[$L_AR_TITLE：$NowTime]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m 1:\t$L_AR_CUST_TIME ($L_UNIT：$L_TIME_M)\033[0m\n"
            echo -e "\033[93m 0:\t$L_CLOSE_FUNCTION\033[0m\n"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -n " $L_PLEASE_INPUT:"
            read at_storage_input
            case "$at_storage_input" in
                1)
                  clear
                  echo -en " » $L_AR_PLEASE_INPUT 60 $L_TIME_M："
                  read minute_input
                  if ! echo "$minute_input" | grep [0-9] >>/dev/null; then
                      echo -e "\033[1;92m » $L_INPUT_NOTIS_NUM\033[0m"
                  elif [ "$minute_input" -lt 1 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MIN_ERR 1 $L_TIME_M!\033[0m"
                  elif [ "$minute_input" -ge 60 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MAX_ERR 1 $L_TIME_M!\033[0m"
                  else
                      echo -e "time=${minute_input}/M\ndate=0\nrun=$home_dir/$core All_Dir" > "$work_dir/$timed_config_dir/ClearDir.conf"
                      echo " » $L_OPEND!"
                  fi
                  ;;
                0)
                  clear
                  rm "$work_dir/$timed_config_dir/ClearDir.conf"
                  echo " » $L_CLOSED!"
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          4)
            clear
            NowTime=$(grep "time=" "$work_dir/$timed_config_dir/ClearCache.conf" | cut -f2 -d "=")
            echo -e " \033[104m[$L_AR_TITLE：$NowTime]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m 1:\t$L_AR_CUST_TIME ($L_UNIT：$L_TIME_D)\033[0m\n"
            echo -e "\033[93m 0:\t$L_CLOSE_FUNCTION\033[0m\n"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -n " $L_PLEASE_INPUT:"
            read at_appclean_input
            case "$at_appclean_input" in
                1)
                  clear
                  echo -en " » $L_AR_PLEASE_INPUT 1～30 $L_TIME_D："
                  read day_input
                  if ! echo "$day_input" | grep [0-9] >>/dev/null; then
                      echo -e "\033[1;92m » $L_INPUT_NOTIS_NUM\033[0m"
                  elif [ "$day_input" -lt 1 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MIN_ERR 1 $L_TIME_D!\033[0m"
                  elif [ "$day_input" -gt 30 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MAX_ERR 30 $L_TIME_D!\033[0m"
                  else
                      echo -e "time=${day_input}/D\ndate=0\nrun=$home_dir/$core ClearCache\nin=0/5" > "$work_dir/$timed_config_dir/ClearCache.conf"
                      echo " » $L_OPEND!"
                  fi
                  ;;
                0)
                  clear
                  rm "$work_dir/$timed_config_dir/ClearCache.conf"
                  echo " » $L_CLOSED!"
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          5)
            clear
            NowTime=$(grep "time=" "$work_dir/$timed_config_dir/FileAll.conf" | cut -f2 -d "=")
            echo -e " \033[104m[$L_AR_TITLE：$NowTime]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m 1:\t$L_AR_CUST_TIME ($L_UNIT：$L_TIME_H)\033[0m\n"
            echo -e "\033[93m 0:\t$L_CLOSE_FUNCTION\033[0m\n"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -n " $L_PLEASE_INPUT:"
            read at_fileall_input
            case "$at_fileall_input" in
                1)
                  clear
                  echo -en " » $L_AR_PLEASE_INPUT 1～24 $L_TIME_H："
                  read hour_input
                  if ! echo "$hour_input" | grep [0-9] >>/dev/null; then
                      echo -e "\033[1;92m » $L_INPUT_NOTIS_NUM\033[0m"
                  elif [ "$hour_input" -lt 1 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MIN_ERR 1 $L_TIME_H!\033[0m"
                  elif [ "$hour_input" -ge 24 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MAX_ERR 24 $L_TIME_H!\033[0m"
                  else
                      echo -e "time=${hour_input}/H\ndate=0\nrun=$home_dir/$core File_All" > "$work_dir/$timed_config_dir/FileAll.conf"
                      echo " » $L_OPEND!"
                  fi
                  ;;
                0)
                  clear
                  rm "$work_dir/$timed_config_dir/FileAll.conf"
                  echo " » $L_CLOSED!"
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          6)
            clear
            NowTime=$(grep "time=" "$work_dir/$timed_config_dir/F2fsGc.conf" | cut -f2 -d "=")
            echo -e " \033[104m[$L_AR_TITLE：$NowTime]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m 1:\t$L_AR_CUST_TIME ($L_UNIT：$L_TIME_D)\033[0m\n"
            echo -e "\033[93m 0:\t$L_CLOSE_FUNCTION\033[0m\n"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -n " $L_PLEASE_INPUT:"
            read at_fast_input
            case "$at_fast_input" in
                1)
                  clear
                  echo -en " » $L_AR_PLEASE_INPUT 1～30 $L_TIME_D："
                  read day_input
                  if ! echo "$day_input" | grep [0-9] >>/dev/null; then
                      echo -e "\033[1;92m » $L_INPUT_NOTIS_NUM\033[0m"
                  elif [ "$day_input" -lt 1 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MIN_ERR 1 $L_TIME_D!\033[0m"
                  elif [ "$day_input" -gt 30 ]; then
                      echo -e "\033[1;92m » $L_AR_TIME_MAX_ERR 30 $L_TIME_D!\033[0m"
                  else
                      echo -e "time=${day_input}/D\ndate=0\nrun=$home_dir/$core F2fs_GC\nin=0/5" > "$work_dir/$timed_config_dir/F2fsGc.conf"
                      echo " » $L_OPEND!"
                  fi
                  ;;
                0)
                  clear
                  rm "$work_dir/$timed_config_dir/F2fsGc.conf"
                  echo " » $L_CLOSED!"
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          *)
            echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
            ;;
      esac
      ;;
    10)
      clear
      if [ "$clearbox_stop_install" = 1 ]; then
          text_1="$L_CLOSE"
      else
          text_1="$L_OPEN"
      fi
      echo -e "\033[104m [$L_STOP_INSTALL]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -e "\033[93m 1:\t${text_1}${L_STOP_INSTALL}\033[0m\n"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT:"
      read sa_input
      case "$sa_input" in
          1)
            if [ "$clearbox_stop_install" != 1 ]; then
                echo -en " » $L_CONFIRM？(y): "
                read ok_input
                case "$ok_input" in
                    y | Y)
                      clear
                      "$home_dir/$core" StopInstall STOP
                      ;;
                    *)
                      echo -e "\033[92m » $L_CANCEL\033[0m"
                      ;;
                esac
            else
                clear
                "$home_dir/$core" StopInstall RESET
            fi
            ;;
          *)
            echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
            ;;
      esac
      ;;
    11)
      clear
      if [ "$clearbox_bind_path" = 1 ]; then
          text_1="$L_CLOSE"
      else
          text_1="$L_OPEN"
      fi
      echo -e "\033[104m [$L_PATH_BIND]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -e "\033[93m 1:\t${text_1}${L_PATH_BIND}\033[0m\n"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT:"
      read bm_input
      case "$bm_input" in
          1)
            if [ "$clearbox_bind_path" != 1 ]; then
                echo -en " » $L_CONFIRM？(y): "
                read ok_input
                case "$ok_input" in
                    y | Y)
                      clear
                      sed -i 's/clearbox_bind_path=0/clearbox_bind_path=1/g' "$work_dir/settings.prop"
                      "$home_dir/$core" "BindPath" "MOUNT"
                      ;;
                    *)
                      echo -e "\033[92m » $L_CANCEL\033[0m"
                      ;;
                esac
            else
                clear
                sed -i 's/clearbox_bind_path=1/clearbox_bind_path=0/g' "$work_dir/settings.prop"
                "$home_dir/$core" "BindPath" "UMOUNT"
            fi
            ;;
          *)
            echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
            ;;
      esac
      ;;
    12)
      clear
      if [ "$clearbox_stop_storage" = 1 ]; then
          text_1="$L_CLOSE"
      else
          text_1="$L_OPEN"
      fi
      echo -e "\033[104m [$L_STOP_STORAGE]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -e "\033[93m 1:\t${text_1}${L_STOP_STORAGE}\033[0m\n"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT:"
      read ss_input
      case "$ss_input" in
          1)
            if [ "$clearbox_stop_storage" != 1 ]; then
                echo -en " » $L_CONFIRM？(y): "
                read ok_input
                case "$ok_input" in
                    y | Y)
                      clear
                      "$home_dir/$core" StopStorage STOP
                      ;;
                    *)
                      echo -e "\033[92m » $L_CANCEL\033[0m"
                      ;;
                esac
            else
                clear
                "$home_dir/$core" StopStorage RESET
            fi
            ;;
          *)
            echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
            ;;
      esac
      ;;
    13)
      clear
      if [ "$clearbox_stop_cache" = 1 ]; then
          text_1="$L_CLOSE"
      else
          text_1="$L_OPEN"
      fi
      echo -e "\033[104m [$L_STOP_APP_CACHE]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -e "\033[93m 1:\t${text_1}${L_STOP_APP_CACHE}\033[0m\n"
      echo -e "\033[93m 2:\t$L_ADD_WHITELIST\033[0m\n"
      echo -e "\033[93m 3:\t$L_REMOVE_WHITELIST\033[0m\n"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT:"
      read sc_input
      case "$sc_input" in
          1)
            if [ "$clearbox_stop_cache" != 1 ]; then
                echo -en " » $L_CONFIRM？(y): "
                read ok_input
                case "$ok_input" in
                    y | Y)
                      clear
                      sed -i 's/clearbox_stop_cache=0/clearbox_stop_cache=1/g' "$work_dir/settings.prop"
                      if ! pgrep "StopCached" >/dev/null 2>&1; then
                          "$home_dir/$core" "StopCached"
                      fi
                      echo " » $L_OPEND!"
                      ;;
                    *)
                      clear
                      echo -e "\033[92m » $L_CANCEL\033[0m"
                      ;;
                esac
            else
                clear
                sed -i 's/clearbox_stop_cache=1/clearbox_stop_cache=0/g' "$work_dir/settings.prop"
                echo " » $L_CLOSED!"
            fi
            ;;
          2)
            clear
            echo -en " » $L_INPUT_PACKAGES："
            read packages_input
            [ -z "$packages_input" ] && packages_input="null"
            for package in $packages_input; do
                if grep "$package\$" "$work_dir/whitelist.prop" >> /dev/null; then
                    echo " » $package $L_ISIN_WHITELIST"
                    continue
                elif ! pm list package -3 | grep "package:$package\$" >> /dev/null; then
                    echo " » $package $L_NOTIN_PACKAGE_LIST!"
                    continue
                else
                    echo "$package" >> "$work_dir/whitelist.prop"
                    echo " » $package $L_ADD_WHITELIST_SUCCESS!"
                fi
            done
            ;;
          3)
            clear
            echo -en " » $L_INPUT_PACKAGES："
            read packages_input
            [ -z "$packages_input" ] && packages_input="null"
            for package in $packages_input; do
                if grep "$package\$" "$work_dir/whitelist.prop" >> /dev/null; then
                    sed -i /"$package"/d "$work_dir/whitelist.prop"
                    echo " $package $L_REMOVE_WHITELIST_SUCCESS!"
                else
                    echo " $package $L_NOTIN_WHITRLIST!"
                fi
            done
            ;;
          *)
            echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
            ;;
      esac
      ;;
    14)
      clear
      echo -e "\033[104m [$L_DISK_APP_PLUS]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -e "\033[93m 1:\t$L_DISK_GC\033[0m\n"
      echo -e "\033[93m 2:\t$L_FAST_DISK_GC\033[0m\n"
      echo -e "\033[93m 3:\t$L_DEXOAT\033[0m\n"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT:"
      read dg_input
      case "$dg_input" in
          1)
            clear
            "$home_dir/$core" F2fs_GC
            ;;
          2)
            clear
            "$home_dir/$core" Fast_GC
            ;;
          3)
            clear
            echo -e "\033[104m [DEXOAT]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m 1:\t$L_SYS_DEXOAT\033[0m\n"
            echo -e "\033[93m 2:\t$L_CUST_DEXOAT\033[0m\n"
            echo -e "\033[93m 3:\t$L_DEXOAT_RESET\033[0m\n"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -n " $L_PLEASE_INPUT:"
            read cust_mode
            case "$cust_mode" in
                1)
                  "$home_dir/$core" Dexoat_1
                  ;;
                2)
                  clear
                  echo -e "\033[104m [$L_MODE]\033[0m"
                  echo -e "\033[96m ==============================================\033[0m\n"
                  echo -e "\033[93m 1:\tSpeed $L_MODE\033[0m\n"
                  echo -e "\033[93m 2:\tSpeed-Profile $L_MODE\033[0m\n"
                  echo -e "\033[93m 3:\tEverything $L_MODE\033[0m\n"
                  echo -e "\033[96m ==============================================\033[0m\n"
                  echo -n " $L_PLEASE_INPUT:"
                  read mode
                  case "$mode" in
                      1)
                        clear
                        "$home_dir/$core" Dexoat_2 speed
                        ;;
                      2)
                        clear
                        "$home_dir/$core" Dexoat_2 speed-profile
                        ;;
                      3)
                        clear
                        "$home_dir/$core" Dexoat_2 everything
                        ;;
                      *)
                        echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                        ;;
                  esac
                  ;;
                3)
                  "$home_dir/$core" Dexoat_3
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          *)
            echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
            ;;
      esac
      ;;
    00)
      clear
      echo -e "\033[104m [$L_SETTINGS]\033[0m"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -e "\033[93m 1:\t$L_CLEAN_SETTINGS\033[0m\n"
      echo -e "\033[93m 2:\t$L_TUI_SWITCH\033[0m\n"
      echo -e "\033[93m 00:\t$L_ABOUT\033[0m\n"
      echo -e "\033[93m 01:\t$L_CONFIG_BACKUP\033[0m\n"
      echo -e "\033[93m 02:\t$L_UNINSTALL\033[0m\n"
      echo -e "\033[93m 03:\t$L_VIEW_LOG\033[0m\n"
      echo -e "\033[96m ==============================================\033[0m\n"
      echo -n " $L_PLEASE_INPUT:"
      read settings_option_input
      case "$settings_option_input" in
          1)
            clear
            if [ "$clearbox_auto_file_all" = 1 ]; then
                text_1="$L_CLOSE"
            else
                text_1="$L_OPEN"
            fi
            if [ "$clearbox_auto_fast_gc" = 1 ]; then
                text_2="$L_CLOSE"
            else
                text_2="$L_OPEN"
            fi
            echo -e "\033[104m [$L_CLEAN_SETTINGS]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m 1:\t$L_EXTERNAL_SETTINGS\033[0m\n"
            echo -e "\033[93m 2:\t${text_1}${L_ES_AUTO_FILE_ALL}\033[0m\n"
            echo -e "\033[93m 3:\t${text_2}${L_ES_AUTO_DISK_GC}\033[0m\n"
            echo -e "\033[93m 4:\t$L_ADD_WHITELIST\033[0m\n"
            echo -e "\033[93m 5:\t$L_REMOVE_WHITELIST\033[0m\n"
            echo -e "\033[93m 6:\t$L_CACHE_SKIP_SIZE\033[0m\n"
            echo -e "\033[93m 7:\t$L_CUST_FILE_ALL_PATH\033[0m\n"
            echo -e "\033[93m 8:\t$L_GC_SKIP_SIZE\033[0m\n"
            echo -e "\033[96m==============================================\033[0m\n"
            echo -n " $L_PLEASE_INPUT:"
            read clean_option_input
            case "$clean_option_input" in
                1)
                  clear
                  if [ "$clearbox_clear_disk" = 1 ]; then
                      text_1="$L_CLOSE"
                  else
                      text_1="$L_OPEN"
                  fi
                  if [ "$clearbox_file_all_disk" = 1 ]; then
                      text_2="$L_CLOSE"
                  else
                      text_2="$L_OPEN"
                  fi
                  if [ "$clearbox_clear_disk" = 1 ]; then
                      text_3="$L_CLOSE"
                  else
                      text_3="$L_OPEN"
                  fi
                  echo -e "\033[104m [$L_EXTERNAL_SETTINGS]\033[0m"
                  echo -e "\033[96m ==============================================\033[0m\n"
                  echo -e "\033[93m 1:\t${text_1}${L_EXTERNAL_FILE_CLEAN}\033[0m\n"
                  echo -e "\033[93m 2:\t${text_2}${L_EXTERNAL_FILE_ALL}\033[0m\n"
                  echo -e "\033[93m 3:\t${text_3}${L_EXTERNAL_FILE_CLEAN}\033[0m\n"
                  echo -e "\033[96m ==============================================\033[0m\n"
                  echo -n " $L_PLEASE_INPUT:"
                  read disk_option_input
                  case "$disk_option_input" in
                      1)
                        if [ "$clearbox_clear_disk" != 1 ]; then
                            echo -en " » $L_CONFIRM？(y): "
                            read ok_input
                            case "$ok_input" in
                                y | Y)
                                  clear
                                  sed -i 's/clearbox_clear_disk=0/clearbox_clear_disk=1/g' "$work_dir/settings.prop"
                                  echo " » $L_OPEND!"
                                  ;;
                                *)
                                  echo -e "\033[92m » $L_CANCEL\033[0m"
                                  ;;
                            esac
                        else
                            clear
                            sed -i 's/clearbox_clear_disk=1/clearbox_clear_disk=0/g' "$work_dir/settings.prop"
                            echo " » $L_CLOSED!"
                        fi
                        ;;
                      2)
                        if [ "$clearbox_file_all_disk" != 1 ]; then
                            echo -en " » $L_CONFIRM？(y): "
                            read ok_input
                            case "$ok_input" in
                                y | Y)
                                  clear
                                  sed -i 's/clearbox_file_all_disk=0/clearbox_file_all_disk=1/g' "$work_dir/settings.prop"
                                  echo " » $L_OPEND!"
                                  ;;
                                *)
                                  echo -e "\033[92m » $L_CANCEL\033[0m"
                                  ;;
                            esac
                        else
                            clear
                            sed -i 's/clearbox_file_all_disk=1/clearbox_file_all_disk=0/g' "$work_dir/settings.prop"
                            echo " » $L_CLOSED!"
                        fi
                        ;;
                      3)
                        if [ "$clearbox_clear_disk" != 1 ]; then
                            echo -en " » $L_CONFIRM？(y): "
                            read ok_input
                            case "$ok_input" in
                                y | Y)
                                  clear
                                  sed -i 's/clearbox_file_clear_disk=0/clearbox_file_clear_disk=1/g' "$work_dir/settings.prop"
                                  echo " » $L_OPEND!"
                                  ;;
                                *)
                                  echo -e "\033[92m » $L_CANCEL\033[0m"
                                  ;;
                            esac
                        else
                            clear
                            sed -i 's/clearbox_file_clear_disk=1/clearbox_file_clear_disk=0/g' "$work_dir/settings.prop"
                            echo " » $L_CLOSED!"
                        fi
                        ;;
                      *)
                        echo -e "\033[92m » $L_CANCEL\033[0m"
                        ;;
                  esac
                  ;;
                2)
                  if [ "$clearbox_auto_file_all" != 1 ]; then
                      echo -en " » $L_CONFIRM？(y): "
                      read ok_input
                      case "$ok_input" in
                          y | Y)
                            clear
                            sed -i 's/clearbox_auto_file_all=0/clearbox_auto_file_all=1/g' "$work_dir/settings.prop"
                            echo " » $L_OPEND!"
                            ;;
                          *)
                            echo -e "\033[92m » $L_CANCEL\033[0m"
                            ;;
                      esac
                  else
                      clear
                      sed -i 's/clearbox_auto_file_all=1/clearbox_auto_file_all=0/g' "$work_dir/settings.prop"
                      echo " » $L_CLOSED!"
                  fi
                  ;;
                3)
                  if [ "$clearbox_auto_fast_gc" != 1 ]; then
                      echo -en " » $L_CONFIRM？(y): "
                      read ok_input
                      case "$ok_input" in
                          y | Y)
                            clear
                            sed -i 's/clearbox_auto_fast_gc=0/clearbox_auto_fast_gc=1/g' "$work_dir/settings.prop"
                            echo " » $L_OPEND!"
                            ;;
                          *)
                            echo -e "\033[92m » $L_CANCEL\033[0m"
                            ;;
                      esac
                  else
                      clear
                      sed -i 's/clearbox_auto_fast_gc=1/clearbox_auto_fast_gc=0/g' "$work_dir/settings.prop"
                      echo " » $L_CLOSED!"
                  fi
                  ;;
                4)
                  clear
                  echo -en " » $L_INPUT_PACKAGES："
                  read packages_input
                  [ -z "$packages_input" ] && packages_input="null"
                  for package in $packages_input; do
                      if grep "$package\$" "$work_dir/ClearWhitelist.prop" >> /dev/null; then
                          echo " » $package $L_ISIN_WHITELIST"
                          continue
                      elif ! pm list package -3 | grep "package:$package\$" >> /dev/null; then
                          echo " » $package $L_NOTIN_PACKAGE_LIST!"
                          continue
                      fi
                      echo "$package" >> "$work_dir/ClearWhitelist.prop"
                      echo " » $package $L_ADD_WHITELIST_SUCCESS!"
                  done
                  ;;
                5)
                  clear
                  echo -en " » $L_INPUT_PACKAGES："
                  read packages_input
                  [ -z "$packages_input" ] && packages_input="null"
                  for package in $packages_input; do
                      if grep "$package\$" "$work_dir/ClearWhitelist.prop" >> /dev/null; then
                          sed -i /"$package"/d "$work_dir/ClearWhitelist.prop"
                          echo " $package $L_REMOVE_WHITELIST_SUCCESS!"
                      else
                          echo " $package $L_NOTIN_WHITRLIST!"
                      fi
                  done
                  ;;
                6)
                  clear
                  echo " » $L_NUM_SIZE_NOW $clearbox_clear_cache_size M"
                  echo -n " » $L_NUM_SIZE_ASK? (y/N):"
                  read ok_input
                  case "$ok_input" in
                      y | Y)
                        echo -n " » $L_INPUT_NOTIS_NUM:"
                        read size_input
                        if ! echo "$size_input" | grep [0-9] >>/dev/null; then
                            echo -e "\033[1;92m » $L_INPUT_NOTIS_NUM\033[0m"
                        elif [ "$size_input" -lt 0 ]; then
                            echo -e "\033[1;92m » $L_CACHE_SKIP_MIN_ERR 0 $L_SIZE_M\033[0m"
                        elif [ "$size_input" -gt 100 ]; then
                            echo -e "\033[1;92m » $L_CACHE_SKIP_MAX_ERR 100 $L_SIZE_M\033[0m"
                        else
                            sed -i 's/clearbox_clear_cache_size=[0-9]*/clearbox_clear_cache_size='"$size_input"'/g' "$work_dir/settings.prop"
                            echo " » SUCCESSFUL!"
                        fi
                        ;;
                      *)
                        echo -e "\033[92m » $L_CANCEL\033[0m"
                        ;;
                  esac
                  ;;
                7)
                  clear
                  echo " » PATH: $clearbox_file_all_dirname"
                  echo -en " » $L_INPUT_PATH："
                  read path_input
                  [ -z "$path_input" ] && path_input="Documents"
                  sed -i 's|clearbox_file_all_dirname=.*|clearbox_file_all_dirname='"$path_input"'|g' "$work_dir/settings.prop"
                  echo " » SUCCESSFUL!"
                  ;;
                8)
                  clear
                  echo " » $L_NUM_SIZE_NOW $clearbox_f2fs_gc_min_dirty"
                  echo -n " » $L_NUM_SIZE_ASK? (y/N):"
                  read ok_input
                  case "$ok_input" in
                      y | Y)
                        echo -n " » $L_INPUT_NOTIS_NUM:"
                        read size_input
                        if ! echo "$size_input" | grep [0-9] >>/dev/null; then
                            echo -e "\033[1;92m » $L_INPUT_NOTIS_NUM\033[0m"
                        else
                            sed -i 's/clearbox_f2fs_gc_min_dirty=[0-9]*/clearbox_f2fs_gc_min_dirty='"$size_input"'/g' "$work_dir/settings.prop"
                            echo " » SUCCESSFUL!"
                        fi
                        ;;
                      *)
                        echo -e "\033[92m » $L_CANCEL\033[0m"
                        ;;
                  esac
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
              esac
              ;;
          2)
            clear
            if [ -f "$home_dir/skip_mount" ]; then
                echo -n " » $L_ASK_OPEN_TUI (y/N):"
                read ok_input
                case "$ok_input" in
                    y | Y)
                      if rm "$home_dir/skip_mount" 2>/dev/null; then
                          echo " » $L_OPEND!"
                      else
                          echo " » $L_OPEN_ERR"
                      fi
                      ;;
                    *)
                      echo -e "\033[92m » $L_CANCEL\033[0m"
                      ;;
                esac
            else
                echo -n " » $L_ASK_CLOSE_TUI (y/N) :"
                read ok_input
                case "$ok_input" in
                    y | Y)
                      if touch "$home_dir/skip_mount" 2>/dev/null; then
                          echo " » $L_CLOSED!"
                      else
                          echo " » $L_CLOSE_ERR"
                      fi
                      ;;
                    *)
                      echo -e "\033[92m » $L_CANCEL\033[0m"
                      ;;
                esac
            fi
            ;;
          00)
            clear
            echo -e "\033[104m [$L_ABOUT ClearBox     $Version]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[92m     CCC  L     EEEEEE     A      RRRRR\033[0m"
            echo -e "\033[92m   C      L     E_____    A A     R    R\033[0m"
            echo -e "\033[92m   C      L     E        A AAA    R  R\033[0m"
            echo -e "\033[92m     CCC  LLLL  EEEEEE  A     A   R    R\033[0m\n\n"
            echo -e "\033[104m \t$Str_1\033[0m"
            echo -e "\033[104m \t\t$Str_2\033[0m\n"
            echo -e "\033[93m 1:\tGitHub\033[0m\n"
            echo -e "\033[93m 2:\tTelegram channel\033[0m\n"
            echo -e "\033[93m 3:\tTelegram group\033[0m\n"
            echo -e "\033[93m 4:\tYHChat\033[0m\n"
            if [ "$clearbox_debug_mode" = 1 ]; then
                DebugTitle=$(echo -en "\033[91m DEBUG MODE \033[0m")
                echo -e "\t\t\t\t$DebugTitle"
            fi
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -en " $L_PLEASE_INPUT:"
            read abort_option_input
            case "$abort_option_input" in
                1)
                  if ! am start -a android.intent.action.VIEW -d "https://github.com/FLYCOM-E/ClearBox" >>/dev/null; then
                      echo " » Error"
                  fi
                  ;;
                2)
                  if ! am start -a android.intent.action.VIEW -d "https://t.me/clearbox_update" >>/dev/null; then
                      echo " » Error"
                  fi
                  ;;
                3)
                  if ! am start -a android.intent.action.VIEW -d "https://t.me/clearbox_group" >>/dev/null; then
                      echo " » Error"
                  fi
                  ;;
                4)
                  if ! am start -a android.intent.action.VIEW -d "https://yhfx.jwznb.com/share?key=yigOTedUjh62&ts=1747355950" >>/dev/null; then
                      echo " » Error"
                  fi
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          01)
            clear
            echo -e "\033[104m [$L_CONFIG_BACKUP]\033[0m"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -e "\033[93m \t1:\tBACKUP\033[0m\n"
            echo -e "\033[93m \t2:\tRECOVER\033[0m\n"
            echo -e "\033[96m ==============================================\033[0m\n"
            echo -en " $L_PLEASE_INPUT:"
            read backup_option_input
            case "$backup_option_input" in
                1)
                  clear
                  "$home_dir/$core" configManager backup
                  ;;
                2)
                  clear
                  echo -en " » PATH: "
                  read configFile
                  "$home_dir/$core" configManager recovery "$configFile"
                  ;;
                *)
                  echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
                  ;;
            esac
            ;;
          02)
            echo -en " » $L_ASK_UNINSTALL🤯？(y/n): "
            read ok_input
            case "$ok_input" in
                y | Y)
                  sh "$home_dir/uninstall.sh"
                  echo " » Bye！"
                  clear && exit 0
                  ;;
                *)
                  echo -e "\033[92m » $L_CANCEL\033[0m"
                  ;;
            esac
            ;;
          03)
            clear
            echo -e "\033[104m [$L_VIEW_LOG]\033[0m"
            if [ -f "$work_dir/LOG.log" ]; then
                more "$work_dir/LOG.log"
            fi
            ;;
          *)
            echo -e "\033[91m » $L_INPUT_ERROR\033[0m"
            ;;
        esac
        ;;
    e | E | q | Q)
      clear
      echo " » Bye❤️❤️! "
      exit 0
      ;;
esac
echo -en "\033[92m » $L_ENTER\033[0m"; read null
}

# 运行函数
while true; do
    source "$work_dir/settings.prop"
    md_menu
    sleep 1
done
