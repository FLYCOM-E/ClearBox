#!/system/bin/sh
# ClearBox
[ ! "$(whoami)" = "root" ] && echo " » 请授予root权限！Please grant root privileges!" && exit 1
######
export home_dir="${0%/*}"
app_config_dir="AppCleanRules"
storage_config_dir="CleanConfigs"
file_config_dir="FileConfigs"
bind_config_dir="BindConfigs"
###### The first stage. wait for boot = 1, timeout auto disable module
first_stage=0
set=0
while [ "$(getprop sys.boot_completed)" != 1 ]; do
    if [ "$set" = 120 ]; then
        touch "$home_dir/disable"
        exit 1
    fi
    first_stage=1
    set=$((set + 1))
    sleep 5
done
######
export work_dir="/data/adb/wipe_cache"
mkdir -p "$work_dir"
echo -en "home_dir=$home_dir\nwork_dir=$work_dir\n" > "$work_dir/PATH"
######
sh "$home_dir/DirtyClear.sh"
######
StartSettings()
{
    PropInit()
    {
        [ -z "$clearbox_stop_cache" ] && echo "clearbox_stop_cache=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_stop_install" ] && echo "clearbox_stop_install=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_stop_storage" ] && echo "clearbox_stop_storage=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_auto_file_all" ] && echo "clearbox_auto_file_all=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_auto_fast_gc" ] && echo "clearbox_auto_fast_gc=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_clear_disk" ] && echo "clearbox_clear_disk=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_file_all_disk" ] && echo "clearbox_file_all_disk=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_bind_path" ] && echo "clearbox_bind_path=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_file_clear_disk" ] && echo "clearbox_file_clear_disk=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_debug_mode" ] && echo "clearbox_debug_mode=0" >> "$work_dir/settings.prop"
        [ -z "$clearbox_clear_cache_size" ] && echo "clearbox_clear_cache_size=5" >> "$work_dir/settings.prop"
    }
    ######
    touch "$work_dir/settings.prop" 2>/dev/null
    source "$work_dir/settings.prop"
    PropInit
    ######
    mkdir -p "$work_dir/$app_config_dir"
    mkdir -p "$work_dir/$storage_config_dir"
    mkdir -p "$work_dir/$file_config_dir"
    mkdir -p "$work_dir/$file_config_dir"
    mkdir -p "$work_dir/$bind_config_dir"
    ######
    [ ! -f "$work_dir/whitelist.prop" ] && touch "$work_dir/whitelist.prop"
    [ ! -f "$work_dir/ClearWhitelist.prop" ] && touch "$work_dir/ClearWhitelist.prop"
    ######
    if [ "$(ls "$work_dir/$file_config_dir/")" = "" ]; then
        if [ -d "$home_dir/$file_config_dir" ]; then
            cp -r "$home_dir/$file_config_dir/"* "$work_dir/$file_config_dir/"
        fi
    fi
    if [ "$(ls "$work_dir/$app_config_dir/")" = "" ]; then
        if [ -d "$home_dir/AppConfigs" ]; then
            cmd package list package | cut -f2 -d ':' >"$work_dir/$app_config_dir/AppList.txt"
            for file in "$home_dir/AppConfigs/"*; do
                name="$(basename $file | sed 's/\.[^.]*$//')"
                if grep "$name" "$work_dir/$app_config_dir/AppList.txt" >/dev/null 2>&1; then
                    cp "$file" "$work_dir/$app_config_dir/"
                fi
            done
            rm -f "$work_dir/$app_config_dir/AppList.txt"
        fi
    fi   
    for file in "$work_dir/$file_config_dir/"*; do
        name1=$(echo "$file" | cut -f1 -d '.')
        name2=$(echo "$file" | cut -f2 -d '.')
        if [ ! "$name2" = "conf" ]; then
            mv "$work_dir/$file_config_dir/$name1.$name2" "$work_dir/$file_config_dir/$name1.conf"
        fi
    done
    ######
    source "$work_dir/settings.prop"
}
######
# Settings
StartSettings
# Remove backup.log and Backup old log
if [ -f "$work_dir/LOG.log" ]; then
    rm -f "$work_dir/LOG.log.bak"
    mv "$work_dir/LOG.log" "$work_dir/LOG.log.bak"
fi
# Start Log
echo "====== Boot Time $(date) ======" > "$work_dir/LOG.log"
if [ "$first_stage" = 1 ]; then
    exec 2>>"$work_dir/LOG.log"
fi
# Chmod 700
chmod -R 700 "$home_dir/"
chmod -R 700 "$work_dir/"
###### The second stage. wait for storaged up
set=0
while [ ! -d "/storage/emulated/0/" ]; do
    [ "$set" = 60 ] && break
    set=$((set + 1))
    sleep 5
done
if [ "$first_stage" = 1 ]; then
    sleep 30
fi
######
if [ "$clearbox_bind_path" = 1 ]; then
    "$home_dir/$core" BindPath 2> "$work_dir/LOG.log"
fi
######
if [ "$clearbox_stop_cache" = 1 ]; then
    if ! pgrep "StopCached" >/dev/null 2>&1; then
        "$home_dir/BashCore" "StopCached"
    fi
fi
if pgrep "ClearBox Timed" >/dev/null 2>&1; then
    kill $(pgrep "ClearBox Timed")
fi
"$home_dir/BashCore" "Timed"
######
exit 0
