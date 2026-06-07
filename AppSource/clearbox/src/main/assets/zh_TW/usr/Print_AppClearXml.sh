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
        <slice size="17" color="#FF9200">軟體清理，建議預檢查配置檔</slice>
    </slices>
</text>
'

if [ -z "$(ls "$work_dir/$app_config_dir/")" ]; then
    echo '
    <text>
        <slices>
            <title> </title>
            <slice size="30" color="#FF3E00">無App清理配置！</slice>
        </slices>
    </text>
    '
    exit 0
fi

for FN in "$work_dir/$app_config_dir"/*; do
    Name=$(grep '@' "$FN" | head -n1 | cut -f2 -d '/')
    Package=$(grep '@' "$FN" | head -n1 | cut -f1 -d '/' | cut -f2 -d '@')
    echo "
    <group title=\" \">
        <action interruptible=\"false\" auto-off=\"false\" confirm=\"false\">
        <title>清理 $Name</title>
            <set>
            \"$home_dir/$core\" Clear_App \"$Package\"
            </set>
        </action>
    </group>
    "
done
echo '
</root>
'
