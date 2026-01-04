CFLAGS = -O2 -s -Wall -llog

ifeq ($(strip $(CC)),)
    $(error 编译器$CC变量设置为空！The compiler $CC variable is set to empty!)
endif
ifeq ($(strip $(M_LANG)),)
    $(error 语言$M_LANG变量设置为空！The language $M_LANG variable is set to empty!)
else
    ifeq ($(M_LANG), zh_CN)
        CFLAGS += -Dzh_CN
    else ifeq ($(M_LANG), en_US)
        CFLAGS += -Den_US
    else ifeq ($(M_LANG), zh_TW)
        CFLAGS += -Dzh_TW
    endif
endif

home_dir = Module_Files
bin_dir = Module_Files/bin

FUNCTIONS_C = src/INCLUDE/functions.c
ALL_FILE = $(home_dir)/BashCore \
			$(bin_dir)/StopCached \
			$(bin_dir)/AppClean \
			$(bin_dir)/CacheClean \
			$(bin_dir)/Dexoat \
			$(bin_dir)/FreeZer \
			$(bin_dir)/StorageRuleClean \
			$(bin_dir)/StorageClean \
			$(bin_dir)/F2fs_GC \
			$(bin_dir)/StopInstall \
			$(bin_dir)/StopStorage \
			$(bin_dir)/FileManager \
			$(home_dir)/Post

all: $(ALL_FILE)
.PHONY: all

$(bin_dir)/StopCached: src/StopCached.c
	@mkdir -p $(bin_dir)
	$(CC) $(CFLAGS) src/StopCached.c $(FUNCTIONS_C) -o $(bin_dir)/StopCached

$(home_dir)/BashCore: src/BashCore.c
	$(CC) $(CFLAGS) src/BashCore.c -o $(home_dir)/BashCore

$(bin_dir)/AppClean: src/AppClean.c
	$(CC) $(CFLAGS) src/AppClean.c $(FUNCTIONS_C) -o $(bin_dir)/AppClean

$(bin_dir)/CacheClean: src/CacheClean.c
	$(CC) $(CFLAGS) src/CacheClean.c $(FUNCTIONS_C) -o $(bin_dir)/CacheClean

$(bin_dir)/Dexoat: src/Dexoat.c
	$(CC) $(CFLAGS) src/Dexoat.c -o $(bin_dir)/Dexoat

$(bin_dir)/FreeZer: src/FreeZer.c
	$(CC) $(CFLAGS) src/FreeZer.c -o $(bin_dir)/FreeZer

$(bin_dir)/StorageRuleClean: src/StorageRuleClean.c
	$(CC) $(CFLAGS) src/StorageRuleClean.c $(FUNCTIONS_C) -o $(bin_dir)/StorageRuleClean

$(bin_dir)/StorageClean: src/StorageClean.c
	$(CC) $(CFLAGS) src/StorageClean.c $(FUNCTIONS_C) -o $(bin_dir)/StorageClean

$(bin_dir)/F2fs_GC: src/F2fs_GC.c
	$(CC) $(CFLAGS) src/F2fs_GC.c -o $(bin_dir)/F2fs_GC

$(bin_dir)/StopInstall: src/StopInstall.c
	$(CC) $(CFLAGS) src/StopInstall.c -o $(bin_dir)/StopInstall

$(bin_dir)/StopStorage: src/StopStorage.c
	$(CC) $(CFLAGS) src/StopStorage.c -o $(bin_dir)/StopStorage

$(bin_dir)/FileManager: src/FileManager.c
	$(CC) $(CFLAGS) src/FileManager.c -o $(bin_dir)/FileManager

$(home_dir)/Post: src/Post.c
	$(CC) $(CFLAGS) src/Post.c -o $(home_dir)/Post

clean: 
	@rm $(ALL_FILE)
	@rm $(home_dir)/ClearBox.apk
	@rm -r $(home_dir)/语言包
	@rm -r $(bin_dir)

module_tar: 
	@cp APKS/ClearBox_$(M_LANG).apk $(home_dir)/ClearBox.apk
	@mkdir -p $(home_dir)/语言包 && cp LANG_Configs/$(M_LANG).conf $(home_dir)/语言包/Local.conf
	@cd $(home_dir) && zip -r ../ClearBox_$(M_TARGET)_$(M_API)_$(M_LANG).zip *
