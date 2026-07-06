#!/system/bin/env sh
#此脚本用于将读取的包名添加进白名单
for pk in $Package; do
    echo "$pk" >> "$work_dir/ClearWhitelist.prop"
    if [[ $? = 0 ]]; then
        echo " » $pk 已成功加入白名单！"
    fi
done
