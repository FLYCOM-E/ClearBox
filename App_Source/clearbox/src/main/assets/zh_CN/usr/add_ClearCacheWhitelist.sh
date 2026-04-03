#!/system/bin/sh
#此脚本用于将读取的包名添加进白名单
for i in "$Package"; do
    echo "$i" >> "$work_dir/ClearWhitelist.prop"
    if [[ $? = 0 ]]; then
        echo " $i 已成功加入白名单！"
    fi
done
