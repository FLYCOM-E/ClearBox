#!/system/bin/sh
source "$work_dir/settings.prop"
if [ "$clearbox_debug_mode" = 1 ]; then
    exec 2>>"$work_dir/LOG.log"
else
    exec 2>>/dev/null
fi
######
echo '
<?xml version="1.0" encoding="UTF-8" ?>
<root>
'
echo '
<text>
    <slices>
        <title> </title>
        <slice size="17" color="#FF9200">深度清理，請備份重要檔案！！</slice>
    </slices>
</text>
'

if [ -z "$(ls "$work_dir/文件格式配置/")" ]; then
    echo '
    <text>
        <slices>
            <title> </title>
            <slice size="30" color="#FF3E00">無檔案格式配置！</slice>
        </slices>
    </text>
    '
    exit 0
fi

for FN in "$work_dir/文件格式配置"/*; do
    Name=$(echo "$FN" | cut -f6 -d '/' | cut -f1 -d ".")
    echo "
    <group title=\" \">
        <action interruptible=\"false\" auto-off=\"false\" warning=\"清理內/外部儲存 $Name？\">
            <title>清理 $Name</title>
                <set>
                \"$home_dir/BashCore\" File_Clear \"$Name\"
                </set>
        </action>
    </group>
    "
done
echo '
</root>
'
