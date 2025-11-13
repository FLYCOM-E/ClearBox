#!/system/bin/sh
#此脚本来自ClearBox模块，用于提供终端菜单UI
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
if [ "$DebugPro" = 1 ]; then
    exec 2>>"$work_dir/运行日志.log"
else
    exec 2>>/dev/null
fi
Version=$(ClearBox -v | cut -f3 -d " ")
######
# 菜单函数
md_menu()
{
clear
"$bin_dir/busybox" echo -ne "
 $(echo -e " \033[44m[欢迎使用 ClearBox]\033[0m")
 ==============================================

  1：一键优化清理 \t 2：自定义规则清理

  3：清除垃圾/空文件夹 \t 4：清空所有软件缓存
     
  5：深度文件清理 \t 6：软件规则清理
 
  7：清空系统缓存 \t 8：自动清理
 
  9：阻止软件更新安装 \t 10：内部储存固定功能
 
  11：阻止缓存生成功能 \t 12：磁铁（文件归类
 
  13：磁盘 & 软件优化 \t 00：模块管理

 ==============================================
                            --- 键入 E 退出 ---
  请输入相应序号:"
   read in_put
   case "$in_put" in
       1)
         clear
         "$home_dir/BashCore" ClearAll
         ;;
       2)
         clear
         "$home_dir/BashCore" List_Dir
         ;;
       3)
         clear
         "$home_dir/BashCore" All_Dir
         ;;
       4)
         clear
         "$home_dir/BashCore" ClearCache
         ;;
       5)
         clear
         count=0
         "$bin_dir/busybox" echo -e "\n \033[44m[深度清理，请备份重要文档！！]\033[0m"
         echo -e " ==============================================\n"
         for FN in "$work_dir/文件格式配置"/*; do
            Name=$(echo "$FN" | cut -f6 -d '/' | cut -f1 -d ".")
            count=$((count + 1))
            Num[$count]="$count,$Name"
            echo -e " \t$count：清理 $Name\n"
         done
         echo -e " ==============================================\n"
         echo -n " 请输入相应序号: "
         read put1
         C_num=0
         while true; do
            C_num=$((C_num + 1))
            if [ "$(echo "${Num[$C_num]}" | cut -f1 -d ',')" = "$put1" ]; then
                Fname=$(echo "${Num[$C_num]}" | cut -f2 -d ',')
                echo -ne " » 确认？(y): "
                read put_2
                case "$put_2" in
                    y | Y)
                      clear
                      "$home_dir/BashCore" File_Clear "$Fname"
                      break
                      ;;
                    *)
                      "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                      break
                      ;;
                esac
            fi
            [ "$C_num" = "$count" ] && "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m" && break
         done
         ;;
       6)
         clear
         [ -z "$(ls "$work_dir/清理规则/")" ] && echo " » 无App清理配置！" && return 1
         count=0
         "$bin_dir/busybox" echo -e "\n \033[44m[软件清理，建议预检查配置文件]\033[0m"
         echo -e " ==============================================\n"
         for FN in "$work_dir/清理规则"/*; do
            Name=$(cat "$FN" | grep '@' | cut -f2 -d '/')
            count=$((count + 1))
            Num[$count]="$count,$Name"
            echo -e " \t$count：清理 $Name\n"
         done
         echo -e " ==============================================\n"
         echo -n " 请输入相应序号: "
         read put1
         C_num=0
         while true; do
            C_num=$((C_num + 1))
            if [ "$(echo "${Num[$C_num]}" | cut -f1 -d ',')" = "$put1" ]; then
                Fname=$(echo "${Num[$C_num]}" | cut -f2 -d ',')
                clear
                "$home_dir/BashCore" Clear_App "$Fname"
                break
            fi
            [ "$C_num" = "$count" ] && "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m" && break
         done
         ;;
       7)
         echo -ne " » 确认？(y): "
         read put_2
         case "$put_2" in
             y | Y)
               clear
               "$home_dir/BashCore" Clear_SCache
               ;;
             *)
               "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
               ;;
         esac
         ;;
       8)
         clear
         "$bin_dir/busybox" echo -e "\033[44m[自动清理]\033[0m"
         echo -e " ==============================================\n"
         echo -e " \t1：定期运行优化整理\n"
         echo -e " \t2：定期运行文件归类\n"
         echo -e " \t3：定期清理空文件夹\n"
         echo -e " ==============================================\n"
         echo -n " 请输入相应序号:"
         read put4
         case "$put4" in
             1)
               clear
               NowClearTime=$(cat "$work_dir/CRON/ClearCache/root" | cut -f3 -d ' ' | cut -f2 -d '/')
               "$bin_dir/busybox" echo -e " \033[44m[设定时间    当前设置时间：$NowClearTime)]\033[0m"
               echo -e " ==============================================\n"
               echo -e " \t1：自定义输入间隔时间（单位：天）\n"
               echo -e " \t0：关闭定期优化\n"
               echo -e " ==============================================\n"
               echo -n " 请输入相应序号:"
               read put4
               case "$put4" in
                   1)
                     clear
                     echo -ne " » 请输入纯数字，范围 1～30 天："
                     read day_num
                     if ! echo "$day_num" | grep [0-9] >>/dev/null; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 请输入纯数字！设置失败！！\033[0m"
                     elif [ "$day_num" -lt 1 ]; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 间隔天数少于1天！设置失败！！\033[0m"
                     elif [ "$day_num" -ge 30 ]; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 设置天数超过30天！设置失败！！\033[0m"
                     else
                         echo "0 0 */$day_num * * $home_dir/BashCore ClearAll" > "$work_dir/CRON/ClearCache/root"
                         echo " » 设定成功！"
                     fi
                     ;;
                   0)
                     clear
                     echo -n "" > "$work_dir/CRON/ClearCache/root"
                     echo " » 已关闭定期优化！"
                     ;;
                   *)
                     "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                     ;;
               esac
               ;;
             2)
               clear
               NowFileAllTime=$(cat "$work_dir/CRON/FileAll/root" | cut -f2 -d ' ' | cut -f2 -d '/')
               "$bin_dir/busybox" echo -e " \033[44m[设定时间    当前设置时间：$NowFileAllTime)]\033[0m"
               echo -e " ==============================================\n"
               echo -e " \t1：自定义输入间隔时间（单位：小时）\n"
               echo -e " \t0：关闭定期整理\n"
               echo -e " ==============================================\n"
               echo -n " 请输入相应序号:"
               read put4
               case "$put4" in
                   1)
                     clear
                     echo -ne " » 请输入纯数字，范围 1～24 小时："
                     read N_num
                     if ! echo "$N_num" | grep [0-9] >>/dev/null; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 请输入纯数字！设置失败！！\033[0m"
                     elif [ "$N_num" -lt 1 ]; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 间隔天数少于1小时！设置失败！！\033[0m"
                     elif [ "$N_num" -ge 24 ]; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 设置时间超过24小时！设置失败！！\033[0m"
                     else
                         echo "0 */$N_num * * * $home_dir/BashCore File_All" > "$work_dir/CRON/FileAll/root"
                         echo " » 设定成功！"
                     fi
                     ;;
                   0)
                     clear
                     echo -n "" > "$work_dir/CRON/FileAll/root"
                     echo " » 已关闭定期整理！"
                     ;;
                   *)
                     "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                     ;;
               esac
               ;;
             3)
               clear
               NowFileAllTime=$(cat "$work_dir/CRON/ClearDir/root" | cut -f1 -d ' ' | cut -f2 -d '/')
               "$bin_dir/busybox" echo -e " \033[44m[设定时间    当前设置时间：$NowFileAllTime)]\033[0m"
               echo -e " ==============================================\n"
               echo -e " \t1：自定义输入间隔时间（单位：分钟）\n"
               echo -e " \t0：关闭定期清理空文件夹\n"
               echo -e " ==============================================\n"
               echo -n " 请输入相应序号:"
               read put4
               case "$put4" in
                   1)
                     clear
                     echo -ne " » 请输入纯数字，范围 60 分钟："
                     read S_num
                     if ! echo "$S_num" | grep [0-9] >>/dev/null; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 请输入纯数字！设置失败！！\033[0m"
                     elif [ "$S_num" -lt 1 ]; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 间隔天数少于1分钟！设置失败！！\033[0m"
                     elif [ "$S_num" -ge 60 ]; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 设置时间超过1小时！设置失败！！\033[0m"
                     else
                         echo "*/$S_num * * * * $home_dir/BashCore All_Dir" > "$work_dir/CRON/ClearDir/root"
                         echo " » 设定成功！"
                     fi
                     ;;
                   0)
                     clear
                     echo -n "" > "$work_dir/CRON/ClearDir/root"
                     echo " » 已关闭定期清理空文件夹！"
                     ;;
                   *)
                     "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                     ;;
               esac
               ;;
         esac
         ;;
       9)
         clear
         if [ "$stopinstall" = 1 ]; then
             i1="关闭"
         else
             i1="开启"
         fi
         "$bin_dir/busybox" echo -e " \033[44m[APP更新安装管理]\033[0m"
         echo -e " ==============================================\n"
         echo -e " \t1：$i1阻止APP更新安装\n"
         echo -e " ==============================================\n"
         echo -n " 请输入相应序号:"
         read put1
         case "$put1" in
             1)
               if [ "$i1" = "开启" ]; then
                   echo -ne " » 确认？(y): "
                   read put_3
                   case "$put_3" in
                       y | Y)
                         clear
                         "$home_dir/BashCore" StopInstall STOP
                         ;;
                       *)
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                         ;;
                   esac
               else
                   clear
                   "$home_dir/BashCore" StopInstall RESET
               fi
               ;;
             *)
               "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
               ;;
         esac
         ;;
       10)
         clear
         if [ "$stopstorage" = 1 ]; then
             i1="关闭"
         else
             i1="开启"
         fi
         "$bin_dir/busybox" echo -e " \033[44m[内部储存固定功能]\033[0m"
         echo -e " ==============================================\n"
         echo -e " \t1：$i1固定内部储存\n"
         echo -e " ==============================================\n"
         echo -n " 请输入相应序号:"
         read put1
         case "$put1" in
             1)
               if [ "$i1" = "开启" ]; then
                   echo -ne " » 确认？(y): "
                   read put_3
                   case "$put_3" in
                       y | Y)
                         clear
                         sh "$home_dir/BashCore" StopStorage STOP
                         ;;
                       *)
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                         ;;
                   esac
               else
                   clear
                   "$home_dir/BashCore" StopStorage RESET
               fi
               ;;
             *)
               "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
               ;;
         esac
         ;;
       11)
         clear
         if [ "$stopcache" = 0 ]; then
             i2="开启"
         else
             i2="关闭"
         fi
         "$bin_dir/busybox" echo -e " \033[44m[阻止缓存]\033[0m"
         echo -e " ==============================================\n"
         echo -e " \t1：$i2阻止生成缓存功能\n"
         echo -e " \t2：软件加入白名单\n"
         echo -e " \t3：取消白名单软件\n"
         echo -e " ==============================================\n"
         echo -n " 请输入相应序号:"
         read put2
         case "$put2" in
             1)
               if [ "$i2" = "开启" ]; then
                   echo -ne " » 确认？(y): "
                   read put
                   case "$put" in
                       y | Y)
                         clear
                         if [ "$stopcache" = 0 ]; then
                             sed -i 's/stopcache=0/stopcache=1/g' "$work_dir/settings.prop"
                         fi
                         echo " » 已开启，重启生效 ~"
                         ;;
                       *)
                         clear
                         "$bin_dir/busybox" echo -ne "\033[1;32m 您选择了否！正在返回主页！\033[0m"
                         ;;
                   esac
               else
                   clear
                   sed -i 's/stopcache=1/stopcache=0/g' "$work_dir/settings.prop"
                   echo " » 已关闭，重启生效 ~"
               fi
               ;;
             2)
               clear
               # Off SELinux
               [ "$(getenforce)" = "Enforcing" ] && setenforce 0 && OffSelinux=1
               echo -ne " » 请输入软件包名（空格分隔）："
               read packages
               [ -z "$packages" ] && "$bin_dir/busybox" echo -ne "\033[1;32m » 输入为空！！正在返回主页！\033[0m"
               for package in $packages; do
                   if grep "$package\$" "$work_dir/whitelist.prop" >> /dev/null; then
                       echo " » $package 已存在白名单"
                       continue
                   elif ! pm list package | grep "package:$package\$" >> /dev/null; then
                       echo " » $package 不在软件包列表"
                       continue
                   else
                       echo "$package" >> "$work_dir/whitelist.prop"
                       echo " » $package 已成功加入白名单!"
                   fi
               done
               # Reset SELinux
               [ "$OffSelinux" = 1 ] && setenforce 1
               ;;
             3)
               clear
               echo -ne " » 请输入软件包名（空格分隔）："
               read packages
               [ -z "$packages" ] && "$bin_dir/busybox" echo -ne "\033[1;32m » 输入为空！！正在返回主页！\033[0m"
               for package in $packages; do
                   if grep "$package\$" "$work_dir/whitelist.prop" >> /dev/null; then
                       sed -i /"$package"/d "$work_dir/whitelist.prop"
                       echo " $package 已成功从白名单中移除！"
                   else
                       echo " $package 不在白名单中！"
                   fi
               done
               ;;
             *)
               "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
               ;;
           esac
           ;;
       12)
         echo -ne " » 确认？(y): "
         read put_4
         case "$put_4" in
             y | Y)
               clear
               "$home_dir/BashCore" File_All
               ;;
             *)
               "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
               ;;
         esac
         ;;
       13)
         clear
         "$bin_dir/busybox" echo -e " \033[44m[磁盘 & 软件优化]\033[0m"
         echo -e " ==============================================\n"
         echo -e " \t1：磁盘 GC\n"
         echo -e " \t2：Dexoat 预编译\n"
         echo -e " ==============================================\n"
         echo -n " 请输入相应序号:"
         read put3
         case "$put3" in
             1)
               clear
               "$home_dir/BashCore" F2fs_GC
               ;;
             2)
               clear
               "$bin_dir/busybox" echo -e " \033[44m[DEXOAT]\033[0m"
               echo -e " ==============================================\n"
               echo -e " \t1：触发系统 Dexoat\n"
               echo -e " \t2：自定义模式全部编译\n"
               echo -e " ==============================================\n"
               echo -n " 请输入相应序号:"
               read put3
               case "$put3" in
                   1)
                     "$home_dir/BashCore" Dexoat_1
                     ;;
                   2)
                     clear
                     "$bin_dir/busybox" echo -e " \033[44m[模式选择]\033[0m"
                     echo -e " ==============================================\n"
                     echo -e " \t1：Speed 模式\n"
                     echo -e " \t2：Speed-Profile 模式\n"
                     echo -e " \t3：Everything 模式\n"
                     echo -e " ==============================================\n"
                     echo -n " 请输入相应序号:"
                     read put3
                     case "$put3" in
                         1)
                             clear
                             "$home_dir/BashCore" Dexoat_2 speed
                             ;;
                         2)
                             clear
                             "$home_dir/BashCore" Dexoat_2 speed-profile
                             ;;
                         3)
                             clear
                             "$home_dir/BashCore" Dexoat_2 everything
                             ;;
                         *)
                             "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                             ;;
                     esac
                     ;;
                   *)
                     "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                     ;;
               esac
               ;;
             *)
               "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
               ;;
         esac
         ;;
       00)
         clear
         "$bin_dir/busybox" echo -e " \033[44m[模块管理菜单]\033[0m"
         echo -e " ==============================================\n"
         echo -e " \t0：立即生效当前配置（免重启）\n"
         echo -e " \t1：清理设置\n"
         echo -e " \t00：关于\n"
         echo -e " ==============================================\n"
         echo -n " 请输入相应序号:"
         read put3
         case "$put3" in
             0)
               clear
               ClearBox -U
               ;;
             1)
               clear
               if [ "$clearall" = 0 ]; then
                   i4="开启"
               else
                   i4="关闭"
               fi
               if [ "$fileall" = 0 ]; then
                   i5="开启"
               else
                   i5="关闭"
               fi
               "$bin_dir/busybox" echo -e " \033[44m[清理设置]\033[0m"
               echo -e " ==============================================\n"
               echo -e " \t1：外部储存相关设置\n"
               echo -e " \t2：$i4一键及定时自动清理时清理所有文件\n"
               echo -e " \t3：$i5一键及定时自动清理时运行文件归类\n"
               echo -e " \t4：新增清理白名单\n"
               echo -e " \t5：取消白名单软件\n"
               echo -e " \t6：缓存清理跳过大小\n"
               echo -e " ==============================================\n"
               echo -n " 请输入相应序号:"
               read put5
               case "$put5" in
                   1)
                     clear
                     if [ "$cleardisk" = 0 ]; then
                         i6="开启"
                     else
                         i6="关闭"
                     fi
                     if [ "$Fileall_Disk" = 0 ]; then
                         i7="开启"
                     else
                         i7="关闭"
                     fi
                     if [ "$FileClear_Disk" = 0 ]; then
                         i8="开启"
                     else
                         i8="关闭"
                     fi
                     "$bin_dir/busybox" echo -e " \033[44m[外部储存相关]\033[0m"
                     echo -e " ==============================================\n"
                     echo -e " \t1：$i6清理外部储存缓存\n"
                     echo -e " \t2：$i7外部储存文件归类\n"
                     echo -e " \t3：$i8外部储存文件清理\n"
                     echo -e " ==============================================\n"
                     echo -n " 请输入相应序号:"
                     read put6
                     case "$put6" in
                         1)
                           if [ "$i6" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read cleardisk_i
                               case "$cleardisk_i" in
                                   y | Y)
                                     clear
                                     [ "$cleardisk" = 0 ] && sed -i 's/cleardisk=0/cleardisk=1/g' "$work_dir/settings.prop" && echo " » 已开启！"
                                     ;;
                                   *)
                                     "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                                     ;;
                               esac
                           else
                               clear
                               sed -i 's/cleardisk=1/cleardisk=0/g' "$work_dir/settings.prop"
                               echo " » 已关闭！"
                           fi
                           ;;
                         2)
                           if [ "$i7" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read Fileall_Disk_i
                               case "$Fileall_Disk_i" in
                                   y | Y)
                                     clear
                                     [ "$Fileall_Disk" = 0 ] && sed -i 's/Fileall_Disk=0/Fileall_Disk=1/g' "$work_dir/settings.prop" && echo " » 已开启！"
                                     ;;
                                   *)
                                     "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                                     ;;
                               esac
                           else
                               clear
                               sed -i 's/Fileall_Disk=1/Fileall_Disk=0/g' "$work_dir/settings.prop"
                               echo " » 已关闭！"
                           fi
                           ;;
                         3)
                           if [ "$i8" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read FileClear_Disk_i
                               case "$FileClear_Disk_i" in
                                   y | Y)
                                     clear
                                     [ "$FileClear_Disk" = 0 ] && sed -i 's/FileClear_Disk=0/FileClear_Disk=1/g' "$work_dir/settings.prop" && echo " » 已开启！"
                                     ;;
                                   *)
                                     "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                                     ;;
                               esac
                           else
                               clear
                               sed -i 's/FileClear_Disk=1/FileClear_Disk=0/g' "$work_dir/settings.prop"
                               echo " » 已关闭！"
                           fi
                           ;;
                         *)
                           "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                           ;;
                     esac
                     ;;
                   2)
                     if [ "$i3" = "开启" ]; then
                         echo -ne " » 确认？(y): "
                         read put_5
                         case "$put_5" in
                             y | Y)
                               clear
                               [ "$clearall" = 0 ] && sed -i 's/clearall=0/clearall=1/g' "$work_dir/settings.prop" && echo " » 已开启！"
                               ;;
                             *)
                               "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                               ;;
                         esac
                     else
                         clear
                         sed -i 's/clearall=1/clearall=0/g' "$work_dir/settings.prop"
                         echo " » 已关闭！"
                     fi
                     ;;
                   3)
                     if [ "$i4" = "开启" ]; then
                         echo -ne " » 确认？(y): "
                         read put_6
                         case "$put_6" in
                             y | Y)
                               clear
                               [ "$fileall" = 0 ] && sed -i 's/fileall=0/fileall=1/g' "$work_dir/settings.prop" && echo " » 已开启！"
                               ;;
                             *)
                               "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                               ;;
                         esac
                     else
                         clear
                         sed -i 's/fileall=1/fileall=0/g' "$work_dir/settings.prop"
                         echo " » 已关闭！"
                     fi
                     ;;
                   4)
                     clear
                     # Off SELinux
                     [ "$(getenforce)" = "Enforcing" ] && setenforce 0 && OffSelinux=1
                     echo -ne " » 请输入软件包名（空格分隔）："
                     read packages
                     [ -z "$packages" ] && "$bin_dir/busybox" echo -ne "\033[1;32m » 输入为空！！正在返回主页！\033[0m"
                     for package in $packages; do
                         if grep "$package\$" "$work_dir/ClearWhitelist.prop" >> /dev/null; then
                             echo " » $package 已存在白名单"
                             continue
                         elif ! pm list package | grep "package:$package\$" >> /dev/null; then
                             echo " » $package 不在软件包列表"
                             continue
                         else
                             if pm list package -s | grep "package:$package\$" >> /dev/null; then
                                 echo " » $package 请不要添加系统软件"
                                 continue
                             fi
                             echo "$package" >> "$work_dir/ClearWhitelist.prop"
                             echo " » $package 已成功加入白名单!"
                         fi
                     done
                     # Reset SELinux
                     [ "$OffSelinux" = 1 ] && setenforce 1
                     ;;
                   5)
                     clear
                     echo -ne " » 请输入软件包名（空格分隔）："
                     read packages
                     [ -z "$packages" ] && "$bin_dir/busybox" echo -ne "\033[1;32m » 输入为空！！正在返回主页！\033[0m"
                     for package in $packages; do
                         if grep "$package\$" "$work_dir/ClearWhitelist.prop" >> /dev/null; then
                             sed -i /"$package"/d "$work_dir/ClearWhitelist.prop"
                             echo " $package 已成功从白名单中移除！"
                         else
                             echo " $package 不在白名单中！"
                         fi
                     done
                     ;;
                   6)
                     clear
                     echo " » 当前最小大小为 $ClearCacheSize M"
                     echo -n " » 是否设置新值？(y/N):"
                     read put
                     case "$put" in
                         y | Y)
                           echo -n " » 请输入新值：(单位：兆（M）. 范围：100M):"
                           read put
                           if ! echo "$put" | grep [0-9] >>/dev/null; then
                               "$bin_dir/busybox" echo -ne "\033[1;32m » 请输入纯数字！设置失败！！\033[0m"
                           elif [ "$put" -lt 1 ]; then
                               "$bin_dir/busybox" echo -ne "\033[1;32m » 设置大小小于1兆！设置失败！！\033[0m"
                           elif [ "$put" -ge 100 ]; then
                               "$bin_dir/busybox" echo -ne "\033[1;32m » 设置大小超过100兆！设置失败！！\033[0m"
                           else
                               sed -i 's/ClearCacheSize=[0-9]*/ClearCacheSize='"$put"'/g' "$work_dir/settings.prop"
                               echo " » 设定成功！"
                           fi
                           ;;
                         *)
                           "$bin_dir/busybox" echo -ne "\033[1;32m » 正在返回主页！\033[0m"
                           ;;
                     esac
                     ;;
                   *)
                     "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                     ;;
                 esac
                 ;;
             00)
               clear
               [ "$(getenforce)" = "Enforcing" ] && setenforce 0 && OffSelinux=1
               "$bin_dir/busybox" echo -e " \033[44m[关于 ClearBox     $Version]\033[0m"
               echo -e " ==============================================\n"
               echo -e "     CCC  L     EEEEEE     A      RRRRR"
               echo -e "   C      L     E_____    A A     R    R"
               echo -e "   C      L     E        A AAA    R  R"
               echo -e "     CCC  LLLL  EEEEEE  A     A   R    R\n\n"
               echo -e " \t1：GitHub\n"
               echo -e " \t2：模块内测\n"
               echo -e " \t3：卸载模块（！\n"
               if [ "$DebugPro" = "1" ]; then
                   DebugTitle=$(echo -ne "\033[1;31m 调试模式已打开 \033[0m")
                   echo -e "                               $DebugTitle\n"
               fi
               echo -e " ==============================================\n"
               echo -n " 请输入相应序号:"
               read put7
               case "$put7" in
                   1)
                     if am start -a android.intent.action.VIEW -d "https://github.com/FLYCOM-E/ClearBox" >>/dev/null; then
                         echo " » 跳转成功！"
                     else
                         echo " » 跳转失败！"
                     fi
                     ;;
                   2)
                     if am start -a android.intent.action.VIEW -d "https://yhfx.jwznb.com/share?key=yigOTedUjh62&ts=1747355950" >>/dev/null; then
                         echo " » 跳转成功！"
                     else
                         echo " » 跳转失败！"
                     fi
                     ;;
                   3)
                     echo -ne " » 确定完全卸载 ClearBox 并清理残留😉？(y/n): "
                     read unput
                     case "$unput" in
                         y | Y)
                           sh "$home_dir/uninstall.sh" &
                           wait && clear && exit 0
                           ;;
                         *)
                           "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                           ;;
                     esac
                     ;;
                   *)
                     "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                     ;;
               esac
               [ "$OffSelinux" = 1 ] && setenforce 1
               ;;
             *)
               "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
               ;;
           esac
           ;;
       e | E)
         clear
         exit 0
         ;;
       *)
         "$bin_dir/busybox" echo -ne "\033[1;32m » 已经是最后一层了呦，键入 E 退出！\033[0m"
         ;;
     esac
}

######
# 运行函数
while true; do
    source "$work_dir/settings.prop"
    md_menu
    sleep 1
done
