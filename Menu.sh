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
source "$work_dir/settings.prop"
Version=$(ClearBox -v | cut -f3 -d " ")
#exec 2>>/dev/null
exec 2>>"$work_dir/运行日志.log"
######
echo "[ $(date) ]：打开终端UI" >> "$work_dir/运行日志.log"
######
# 菜单函数
function md_menu()
{
clear
"$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[欢迎使用 ClearBox    "$Version"]\033[0m")
 ==============================================

 1：一键优化清理             2：自定义规则清理

 3：清除垃圾文件及空文件夹   4：清空所有软件缓存
     
 5：深度文件清理             6：软件规则清理
 
 7：清空系统缓存             8：阻止软件更新安装
 
 9：阻止缓存生成功能         10：磁铁（文件归类
 
 11：磁盘优化（GC）          00：模块管理

 ==============================================
                           --- 键入 E 退出 ---
 请输入相应序号:"
   read in_put
     case "$in_put" in
       1)
         clear
         sh "$home_dir/all.sh" ClearAll &
         wait
         ;;
       2)
         clear
         sh "$home_dir/all.sh" List_Dir &
         wait
         ;;
       3)
         clear
         sh "$home_dir/all.sh" All_Dir &
         wait
         ;;
       4)
         clear
         sh "$home_dir/all.sh" ClearCache &
         wait
         ;;
       5)
         clear
         count=0
         echo "      "
         "$bin_dir/busybox" echo -e "\033[44m[深度清理，请备份重要文档！！]\033[0m"
         echo " =============================================="
         echo "      "
         for FN in $(ls "$work_dir/文件格式配置/"); do
            Name=$(echo "$FN" | cut -f1 -d ".")
            count=$((count + 1))
            Num[$count]="$count,$Name"
            echo "     $count：清理 $Name"
            echo "      "
         done
         echo " =============================================="
         echo "      "
         echo -ne " 请输入相应序号: "
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
                    sh "$home_dir/all.sh" File_Clear "$Fname" &
                    wait
                    break
                    ;;
                  *)
                    "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                    break
                    ;;
                esac
            fi
            if [ "$C_num" = "$count" ]; then
                "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                break
            fi
         done
         ;;
       6)
         clear
         count=0
         echo "      "
         "$bin_dir/busybox" echo -e "\033[44m[软件清理，建议预检查配置文件]\033[0m"
         echo " =============================================="
         echo "      "
         for FN in $(ls "$work_dir/清理规则/"); do
            Name=$(cat "$work_dir/清理规则/$FN" | grep '@' | cut -f2 -d '/')
            count=$((count + 1))
            Num[$count]="$count,$Name"
            echo "     $count：清理 $Name"
            echo "      "
         done
         echo " =============================================="
         echo "      "
         echo -ne " 请输入相应序号: "
         read put1
         C_num=0
         while true; do
            C_num=$((C_num + 1))
            if [ "$(echo "${Num[$C_num]}" | cut -f1 -d ',')" = "$put1" ]; then
                Fname=$(echo "${Num[$C_num]}" | cut -f2 -d ',')
                clear
                sh "$home_dir/all.sh" Clear_App "$Fname" &
                wait
                break
            fi
            if [ "$C_num" = "$count" ]; then
                "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
                break
            fi
         done
         ;;
       7)
         echo -ne " » 确认？(y): "
         read put_2
         case "$put_2" in
           y | Y)
             clear
             sh "$home_dir/all.sh" Clear_SCache &
             wait
             ;;
           *)
             "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
             ;;
         esac
         ;;
       8)
         clear
         if [ "$stopinstall" = 1 ]; then
             i1="关闭"
         else
             i1="开启"
         fi
         "$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[APP更新安装管理]\033[0m")
 ==============================================

     1：$i1阻止APP更新安装

 ==============================================

 请输入相应序号:"
         read put1
           case "$put1" in
             1)
               if [ "$i1" = "开启" ]; then
                   echo -ne " » 确认？(y): "
                   read put_3
                   case "$put_3" in
                     y | Y)
                       clear
                       chmod 551 /data/app
                       echo " » 已开启阻止更新！"
                       if [ "$stopinstall" = 0 ]; then
                           sed -i 's/stopinstall=0/stopinstall=1/g' "$work_dir/settings.prop"
                       fi
                       ;;
                     *)
                       "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                       ;;
                   esac
               else
                   clear
                   chmod 771 /data/app
                   echo " » 已关闭阻止更新！"
                   sed -i 's/stopinstall=1/stopinstall=0/g' "$work_dir/settings.prop"
               fi
               ;;
             *)
               "$bin_dir/busybox" echo -ne "\033[1;32m » 输入错误！！正在返回主页！\033[0m"
               ;;
           esac
           ;;
       9)
         clear
         if [ "$stopcache" = 0 ]; then
             i2="开启"
         else
             i2="关闭"
         fi
         "$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[阻止缓存]\033[0m")
 ==============================================

     1：$i2阻止生成缓存功能
     
     2：软件加入白名单
     
     3：取消白名单软件
  
 ==============================================

 请输入相应序号:"
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
               if [ "$(getenforce)" = "Enforcing" ]; then
                   setenforce 0
                   OffSelinux=1
               fi
               echo -ne " » 请输入软件包名（空格分隔）："
               read packages
                 if [ "$packages" = "" ]; then
                     "$bin_dir/busybox" echo -ne "\033[1;32m » 输入为空！！正在返回主页！\033[0m"
                 fi
                 for package in $packages; do
                     if grep "$package\$" "$work_dir/whitelist.prop" >> /dev/null; then
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
                         echo "$package" >> "$work_dir/whitelist.prop"
                         echo " » $package 已成功加入白名单!"
                     fi
                 done
                 # Reset SELinux
                 if [ "$OffSelinux" = 1 ]; then
                     setenforce 1
                 fi
               ;;
             3)
               clear
               echo -ne " » 请输入软件包名（空格分隔）："
               read packages
                 if [ "$packages" = "" ]; then
                     "$bin_dir/busybox" echo -ne "\033[1;32m » 输入为空！！正在返回主页！\033[0m"
                 fi
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
       10)
         echo -ne " » 确认？(y): "
         read put_4
         case "$put_4" in
           y | Y)
             clear
             sh "$home_dir/all.sh" File_All &
             wait
             ;;
           *)
             "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
             ;;
         esac
         ;;
       11)
         clear
         sh "$home_dir/all.sh" F2fs_GC &
         wait
         ;;
       00)
         clear
         "$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[模块管理菜单]\033[0m")
 ==============================================
 
     0：立即生效当前配置（免重启）
     
     1：定期运行优化整理
     
     2：定期运行文件归类
     
     3：定期清理空文件夹
     
     4：清理设置
     
     00：卸载模块(！

 ==============================================

 请输入相应序号:"
         read put3
           case "$put3" in
             0)
               clear
               ClearBox -U
               ;;
             1)
               clear
               NowClearTime=$(cat "$work_dir/CRON/ClearCache/root" | cut -f3 -d ' ' | cut -f2 -d '/')
               "$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[设定时间    $(echo "当前设置时间：$NowClearTime")]\033[0m")
 ==============================================
      
      1：自定义输入间隔时间（单位：天）
      
      0：关闭定期优化
 
 ==============================================

 请输入相应序号:"
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
                         echo "0 0 */$day_num * * sh $home_dir/all.sh ClearAll" > "$work_dir/CRON/ClearCache/root"
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
               "$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[设定时间    $(echo "当前设置时间：$NowFileAllTime")]\033[0m")
 ==============================================
      
      1：自定义输入间隔时间（单位：小时）
      
      0：关闭定期整理
 
 ==============================================

 请输入相应序号:"
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
                         echo "0 */$N_num * * * sh $home_dir/all.sh File_All" > "$work_dir/CRON/FileAll/root"
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
               "$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[设定时间    $(echo "当前设置时间：$NowFileAllTime")]\033[0m")
 ==============================================
      
      1：自定义输入间隔时间（单位：分钟）
      
      0：关闭定期清理空文件夹
 
 ==============================================

 请输入相应序号:"
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
                         echo "*/$S_num * * * * sh $home_dir/all.sh All_Dir" > "$work_dir/CRON/ClearDir/root"
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
             4)
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
               "$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[清理设置]\033[0m")
 ==============================================

     1：外部储存相关设置

     2：$i4一键及定时自动清理时清理所有文件

     3：$i5一键及定时自动清理时运行文件归类功能
     
     4：新增清理白名单
     
     5：取消白名单软件
     
 ==============================================

 请输入相应序号:"
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
                     if [ "$ClearApk_disk" = 0 ]; then
                         i8="开启"
                     else
                         i8="关闭"
                     fi
                     if [ "$ClearZip_disk" = 0 ]; then
                         i9="开启"
                     else
                         i9="关闭"
                     fi
                     if [ "$ClearFont_disk" = 0 ]; then
                         i10="开启"
                     else
                         i10="关闭"
                     fi
                     if [ "$ClearIso_disk" = 0 ]; then
                         i11="开启"
                     else
                         i11="关闭"
                     fi
                     "$bin_dir/busybox" echo -ne "
$(echo -e "\033[44m[外部储存相关]\033[0m")
 ==============================================

     1：$i6清理外部储存缓存

     2：$i7外部储存文件归类

     3：$i8清理外部储存安装包
     
     4：$i9清理外部储存压缩包
     
     5：$i10清理外部储存字体文件
     
     6：$i11清理外部储存镜像文件
     
 ==============================================

 请输入相应序号:"
                     read put6
                     case "$put6" in
                         1)
                           if [ "$i6" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read cleardisk
                               case "$cleardisk" in
                                 y | Y)
                                   clear
                                   if [ "$cleardisk" = 0 ]; then
                                       sed -i 's/cleardisk=0/cleardisk=1/g' "$work_dir/settings.prop"
                                       echo " » 已开启！"
                                   fi
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
                           if [ "$i6" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read Fileall_Disk
                               case "$Fileall_Disk" in
                                 y | Y)
                                   clear
                                   if [ "$Fileall_Disk" = 0 ]; then
                                       sed -i 's/Fileall_Disk=0/Fileall_Disk=1/g' "$work_dir/settings.prop"
                                       echo " » 已开启！"
                                   fi
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
                           if [ "$i6" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read ClearApk_disk
                               case "$ClearApk_disk" in
                                 y | Y)
                                   clear
                                   if [ "$ClearApk_disk" = 0 ]; then
                                       sed -i 's/ClearApk_disk=0/ClearApk_disk=1/g' "$work_dir/settings.prop"
                                       echo " » 已开启！"
                                   fi
                                   ;;
                                 *)
                                   "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                                   ;;
                               esac
                           else
                               clear
                               sed -i 's/ClearApk_disk=1/ClearApk_disk=0/g' "$work_dir/settings.prop"
                               echo " » 已关闭！"
                           fi
                           ;;
                         4)
                           if [ "$i6" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read ClearZip_disk
                               case "$ClearZip_disk" in
                                 y | Y)
                                   clear
                                   if [ "$ClearZip_disk" = 0 ]; then
                                       sed -i 's/ClearZip_disk=0/ClearZip_disk=1/g' "$work_dir/settings.prop"
                                       echo " » 已开启！"
                                   fi
                                   ;;
                                 *)
                                   "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                                   ;;
                               esac
                           else
                               clear
                               sed -i 's/ClearZip_disk=1/ClearZip_disk=0/g' "$work_dir/settings.prop"
                               echo " » 已关闭！"
                           fi
                           ;;
                         5)
                           if [ "$i6" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read ClearFont_disk
                               case "$ClearFont_disk" in
                                 y | Y)
                                   clear
                                   if [ "ClearFont_disk" = 0 ]; then
                                       sed -i 's/ClearFont_disk=0/ClearFont_disk=1/g' "$work_dir/settings.prop"
                                       echo " » 已开启！"
                                   fi
                                   ;;
                                 *)
                                   "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                                   ;;
                               esac
                           else
                               clear
                               sed -i 's/ClearFont_disk=1/ClearFont_disk=0/g' "$work_dir/settings.prop"
                               echo " » 已关闭！"
                           fi
                           ;;
                         6)
                           if [ "$i6" = "开启" ]; then
                               echo -ne " » 确认？(y): "
                               read ClearIso_disk
                               case "$ClearIso_disk" in
                                 y | Y)
                                   clear
                                   if [ "$ClearIso_disk" = 0 ]; then
                                       sed -i 's/ClearIso_disk=0/ClearIso_disk=1/g' "$work_dir/settings.prop"
                                       echo " » 已开启！"
                                   fi
                                   ;;
                                 *)
                                   "$bin_dir/busybox" echo -ne "\033[1;32m » 您选择了否！正在返回主页！\033[0m"
                                   ;;
                               esac
                           else
                               clear
                               sed -i 's/ClearIso_disk=1/ClearIso_disk=0/g' "$work_dir/settings.prop"
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
                             if [ "$clearall" = 0 ]; then
                                 sed -i 's/clearall=0/clearall=1/g' "$work_dir/settings.prop"
                                 echo " » 已开启！"
                             fi
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
                             if [ "$fileall" = 0 ]; then
                                sed -i 's/fileall=0/fileall=1/g' "$work_dir/settings.prop"
                                echo " » 已开启！"
                             fi
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
                     if [ "$(getenforce)" = "Enforcing" ]; then
                         setenforce 0
                         OffSelinux=1
                     fi
                     echo -ne " » 请输入软件包名（空格分隔）："
                     read packages
                     if [ "$packages" = "" ]; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 输入为空！！正在返回主页！\033[0m"
                     fi
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
                     if [ "$OffSelinux" = 1 ]; then
                         setenforce 1
                     fi
                     ;;
                   5)
                     clear
                     echo -ne " » 请输入软件包名（空格分隔）："
                     read packages
                     if [ "$packages" = "" ]; then
                         "$bin_dir/busybox" echo -ne "\033[1;32m » 输入为空！！正在返回主页！\033[0m"
                     fi
                     for package in $packages; do
                         if grep "$package\$" "$work_dir/ClearWhitelist.prop" >> /dev/null; then
                             sed -i /"$package"/d "$work_dir/ClearWhitelist.prop"
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
             00)
               clear
               echo -ne " » 是否卸载模块？(y/n): "
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
    md_menu
    sleep 0.9
done




