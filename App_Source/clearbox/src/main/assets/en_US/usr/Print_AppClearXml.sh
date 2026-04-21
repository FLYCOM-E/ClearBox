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
        <slice size="17" color="#FF9200">Software cleanup, it is recommended to pre-check the configuration files</slice>
    </slices>
</text>
'

if [ -z "$(ls "$work_dir/AppCleanRules/")" ]; then
    echo '
    <text>
        <slices>
            <title> </title>
            <slice size="30" color="#FF3E00">No App cleanup configurations found!</slice>
        </slices>
    </text>
    '
    exit 0
fi

for FN in "$work_dir/AppCleanRules"/*; do
    Name=$(grep '@' "$FN" | head -n1 | cut -f2 -d '/')
    Package=$(grep '@' "$FN" | head -n1 | cut -f1 -d '/' | cut -f2 -d '@')
    echo "
    <group title=\" \">
        <action interruptible=\"false\" auto-off=\"false\" confirm=\"false\">
        <title>Clean $Name</title>
            <set>
            \"$home_dir/BashCore\" Clear_App \"$Package\"
            </set>
        </action>
    </group>
    "
done
echo '
</root>
'
