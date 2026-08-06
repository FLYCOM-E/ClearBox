#!/system/bin/env sh
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
        <slice size="17" color="#FF9200">$L_AC_WARN</slice>
    </slices>
</text>
'

if [ -z "$(ls "$work_dir/$app_config_dir/")" ]; then
    echo '
    <text>
        <slices>
            <title> </title>
            <slice size="30" color="#FF3E00">$L_APP_CONFIG_NOT_FOUND</slice>
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
        <title>$L_CLEAN $Name</title>
            <set>
            \"$home_dir/$core\" --clear-app-cust \"$Package\"
            </set>
        </action>
    </group>
    "
done
echo '
</root>
'
