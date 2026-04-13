// 通用宏
#define L_NOT_USE_ROOT " » Please use Root permissions! #%u\n"
#define L_ARGS_FAILED " » Insufficient arguments provided!\n"
#define L_ARGS_FAILED_2 " » Incorrect arguments provided!\n"
#define L_CONFIG_PATH_NOTFOUND " » Configuration path does not exist/is not accessible!\n"
#define L_CONFIG_NOTFOUND " » Configuration file %s does not exist!\n"
#define L_CONFIG_PATH_TOOLONG " » Configuration path is too long!\n"
#define L_BIN_PATH_NOTFOUND " » Bin path does not exist/is not accessible!\n"
#define L_BIN_PATH_TOOLONG " » Bin path is too long!\n"
#define L_PACKAGE_TOOLONG " » Provided package name is too long! Limit %d\n"
#define L_ARG_PACKAGE_ERR " » No package name provided!\n"
#define L_MODE_TOOLONG " » Mode name is too long!\n"
#define L_ARG_MODE_ERR " » No mode provided!\n"
#define L_MODE_ERR " » Unknown mode! [%s]\n"
#define L_ARG_CONFIGPATH_ERR " » No configuration directory provided!\n"
#define L_NOCONFIG " » No configuration found!\n"
#define L_ARG_BINPATH_ERR " » No Bin directory provided!\n"
#define L_OPEN_PATH_FAILED " » Failed to open directory %s: %s\n"
#define L_OPEN_FILE_FAILED " » Failed to open file %s: %s\n"
#define L_PATH_NOTISDIR " » %s is not a directory！\n"
#define L_PATH_NOTISFILE " » %s is not a file！\n"
#define L_GET_APPLIST_ERROR " » Failed to get application list!\n"
#define L_GET_SYSTEM_VERSION_ERR " » Failed to get system version!\n"
#define L_PATH_STAT_FAILED " » Failed to get information for %s!\n"
#define L_W_SETPROP_ERR " » Warning: PROP setting failed!\n"
#define L_MOVE_ERROR " » Failed to move %s: %s\n"
#define L_DELETE_ERROR " » Failed to delete %s: %s\n"
#define L_SERVER_START_ERR " » Process startup failed: %s\n"

// For StopCached
#define L_SCD_START_SUCCESS " » Cache blocking has been started! #%u"
#define L_SCD_GETAPP_ERR_EXIT " » Process has exited, reason: GETTOPAPP ERROR\n"

// For Timed
#define L_TD_CONFIG_ERROR " » %s configuration error, Skip.\n"
#define L_TD_LINE_ERR_VALUE " » %s configuration: line %d %s corresponding value error. Ignored this line\n"
#define L_TD_LINE_ERR_KEY " » %s configuration: line %d %s undefined error. Ignored this line\n"
#define L_TD_MAX_CONFIG " » Reached configuration loading limit %d. Skipped more configurations\n"
#define L_TD_CONFIG_SUCCESS " » %s configuration loaded successfully\n"
#define L_TD_START_SUCCESS " » Service started successfully！A total of %d configurations loaded！#%u"
#define L_TD_W_POST_TITLE_TOOLONG " » Warning: %s configuration notification title length exceeds limit！Truncated\n"
#define L_TD_W_POST_MESSAGE_TOOLONG " » Warning: %s configuration notification message length exceeds limit！Truncated\n"
#define L_TD_CONFIG_WRITE_ERROR " » %s configuration write-back failed.\n"

// For AppClean.c
#define L_AC_CLEAR " » Cleaning %s &\n"
#define L_AC_CONFIG_NOTFOUND " » No App cleaning configuration found!\n"
#define L_AC_CONFIG_APP_NOTFOUND " » Application specified in %s configuration not found!\n"
#define L_AC_CONFIG_PACKAGE_ERR " » Declaration error in %s configuration, package name not correctly filled!\n"
#define L_AC_CONFIG_APPNAME_ERR " » Declaration error in %s configuration, application name not correctly filled!\n"
#define L_AC_CONFIG_ERR " » Error in %s configuration! Please correctly fill the declaration on the first line\n"
#define L_AC_CONFIG_ERR_1 " » %s configuration line %d contains a dangerous error: starts with '／'!\n"
#define L_AC_CONFIG_ERR_2 " » %s configuration line %d contains an error: path traversal!\n"
#define L_AC_CLEAR_PATH_ERR " » Failed to clean %s!\n"
#define L_AC_CLEAR_PATH_SUCCESS " » %s processed successfully! %ld MB\n"
#define L_AC_CLEAN_SUCCESSFUL " » Software rules applied successfully! Total cleaned: %d MB\n"
#define L_AC_CLEAN_FAILED " » Specified application configuration not found or an error occurred! #%d\n"

// For CacheClean.c
#define L_CC_CLEAR_FAILED " » Failed to clean internal storage application cache\n"
#define L_CC_CLEAR_SUCCESSFUL " » Internal storage application cache cleaning completed\n » Total cleaned: %d MB\n"
#define L_CC_CLEAR_FAILED_SD " » Failed to clean external storage application cache\n"
#define L_CC_CLEAR_SUCCESSFUL_SD " » External storage application cache cleaning completed\n » Total cleaned: %d MB\n"
#define L_CC_CLEAR " » %s cache cleared\n"
#define L_CC_CLEAR_SKIP " » Skipping %s\n"
#define L_CC_CLEAR_APPCACHE_DONE " » A total of %d apps cleaned, %d apps did not require cleaning\n"
#define L_CC_CLEAR_SYSTEMCACHE " » System cache cleared! Recommend restarting the system!\n"

// For Dexoat.c
#define L_DO_RUN_SYSTEM " » Performing system default Dexoat...\n"
#define L_DO_RUN_CUST " » Running mode %s compilation...\n"

// For F2fs_GC.c
#define L_FG_ERR_NOF2FS " » Your device does not use the F2FS filesystem\n » Maintenance is only supported in F2FS environments!\n"
#define L_FG_ERR_CHECK " » Your device does not support the current GC function\n"
#define L_FG_DIRTY " » Current dirty segments: %d\n"
#define L_FG_FREE " » Current free segments: %d\n\n"
#define L_FG_START " » GC has started, please wait patiently. It is recommended to run in the background!\n"
#define L_FG_ERR_WRITESYSFS " » GC startup failed! Failed to write to node!\n"
#define L_FG_ERR_TIMEOUT " » GC wait timeout, waiting has been terminated!\n"
#define L_FG_END " » GC run completed, operation has ended!\n"
#define L_FG_RUN_MS " » Has been running for %d minutes %d seconds...\n"
#define L_FG_RUN_M " » Has been running for %d minutes...\n"
#define L_FG_RUN_S " » Has been running for %d seconds...\n"
#define L_FG_END_DIRTY " » Disk dirty blocks reduced by %d\n"
#define L_FG_END_DIRTY_2 " » Disk dirty blocks increased by %d\n » GC may still be optimizing or may not be suitable for your device!\n"
#define L_FG_DONE " » GC completed!\n"
#define L_FG_FAST_GC_DONE " » Fast disk optimization completed. You can try more aggressive GC optimization (・∀・)\n"

// For FileManager.c
#define L_FM_ALL_FAILED_STORAGE " » Internal storage file organization failed!\n"
#define L_FM_ALL_SUCCESSFUL_STORAGE " » Internal storage file organization successful!\n"
#define L_FM_ALL_FAILED_SD " » External storage file organization failed!\n"
#define L_FM_ALL_SUCCESSFUL_SD " » External storage file organization successful!\n"

#define L_FM_CR_FAILED_STORAGE " » Internal storage file cleanup failed!\n"
#define L_FM_CR_SUCCESSFUL_STORAGE " » Internal storage file cleanup successful!\n"
#define L_FM_CR_FAILED_SD " » External storage file cleanup failed!\n"
#define L_FM_CR_SUCCESSFUL_SD " » External storage file cleanup successful!\n"

#define L_FM_ALL_START " » Organizing %s ...\n"
#define L_FM_ALL_END " » Organized %d %s(s)\n"
#define L_FM_CR_START " » Cleaning %s ...\n"
#define L_FM_CR_END " » Cleaned %d %s(s)\n"

// For FreeZer.c
#define L_OPEN_FREEZER_SUCCESSFUL " » Android native tombstone has been opened (^^)\n"

// For SetInstall.c
#define L_SI_OPEN_FAILED_STORAGE " » Failed to block updates in internal storage!\n"
#define L_SI_OPEN_SUCCESSFUL_STORAGE " » Updates have been blocked in internal storage!\n"
#define L_SI_OFF_FAILED_STORAGE " » Failed to disable update blocking in internal storage!\n"
#define L_SI_OFF_SUCCESSFUL_STORAGE " » Update blocking has been disabled in internal storage!\n"

// For SetStorage.c
#define L_SS_OPEN_STOP_STORAGE_SUCCESSFUL " » Internal storage pinning has been enabled!\n"
#define L_SS_OPEN_STOP_STORAGE_FAILED " » Failed to enable internal storage pinning!\n"
#define L_SS_OFF_STOP_STORAGE_SUCCESSFUL " » Internal storage pinning has been disabled!\n"
#define L_SS_OFF_STOP_STORAGE_FAILED " » Failed to disable internal storage pinning!\n"

// For StorageClean.c
#define L_SC_SUCCESSFUL_STORAGE " » Internal storage cleanup complete!\n » Total cleaned: %d empty files/folders and log files\n"
#define L_SC_SUCCESSFUL_SD " » External storage %s cleanup complete!\n » Total cleaned: %d empty files/folders and log files\n"
#define L_SC_CLEAR " » Cleaning %s storage cache\n"
#define L_SC_CLEAR_ERR " » Failed to clean %s storage cache!\n"
#define L_SC_CLEAR_CACHE_DONE " » Total cleaned: %d app caches!\n"
#define L_SC_CLEAR_DIRTY_ERR " » Failed to clean empty files/folders and log files!\n"

// For StorageRuleClean.c
#define L_SR_START " » Processing %s configuration📍\n"
#define L_SR_W_CONFIG_STATTPATH_ERR " » Warning: Starting path specified in %s configuration does not exist!\n"
#define L_SR_CLEAR_SUCCESSFUL " » Cleaned %s successfully! %ld MB\n"
#define L_SR_CLEAR_FAILED " » Failed to clean %s\n"
#define L_SR_LINE_FAILED_PATH_ERR " » Line %d error: Path error/inaccessible\n"
#define L_SR_END " » Custom directory processing completed!\n"
