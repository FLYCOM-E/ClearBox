CFLAGS = --static -O2 -s -Wall

home_dir = Module_Files
core_bin_dir = Module_Files/system/bin
bin_dir = Module_Files/wipe_cache

ALL_FILE = $(core_bin_dir)/ClearBox \
			$(core_bin_dir)/StopCache \
			$(home_dir)/BashCore \
			$(bin_dir)/AppClean \
			$(bin_dir)/data_cache \
			$(bin_dir)/Dexoat \
			$(bin_dir)/FreeZer \
			$(bin_dir)/system_cache \
			$(bin_dir)/wipe_list_dir \
			$(bin_dir)/f2fs_GC \
			$(bin_dir)/StopInstall \
			$(bin_dir)/StopStorage

all: $(ALL_FILE)
.PHONY: all

$(core_bin_dir)/ClearBox: src/ClearBox.c
	@mkdir -p $(core_bin_dir)
	$(CC) $(CFLAGS) src/ClearBox.c -o $(core_bin_dir)/ClearBox

$(core_bin_dir)/StopCache: src/StopCache.c
	@mkdir -p $(core_bin_dir)
	$(CC) $(CFLAGS) src/StopCache.c -o $(core_bin_dir)/StopCache

$(home_dir)/BashCore: src/BashCore.c
	$(CC) $(CFLAGS) src/BashCore.c -o $(home_dir)/BashCore

$(bin_dir)/AppClean: src/AppClean.c
	$(CC) $(CFLAGS) src/AppClean.c -o $(bin_dir)/AppClean

$(bin_dir)/data_cache: src/data_cache.c
	$(CC) $(CFLAGS) src/data_cache.c -o $(bin_dir)/data_cache

$(bin_dir)/Dexoat: src/Dexoat.c
	$(CC) $(CFLAGS) src/Dexoat.c -o $(bin_dir)/Dexoat

$(bin_dir)/FreeZer: src/FreeZer.c
	$(CC) $(CFLAGS) src/FreeZer.c -o $(bin_dir)/FreeZer

$(bin_dir)/system_cache: src/system_cache.c
	$(CC) $(CFLAGS) src/system_cache.c -o $(bin_dir)/system_cache

$(bin_dir)/wipe_list_dir: src/wipe_list_dir.c
	$(CC) $(CFLAGS) src/wipe_list_dir.c -o $(bin_dir)/wipe_list_dir

$(bin_dir)/f2fs_GC: src/f2fs_GC.c
	$(CC) $(CFLAGS) src/f2fs_GC.c -o $(bin_dir)/f2fs_GC

$(bin_dir)/StopInstall: src/StopInstall.c
	$(CC) $(CFLAGS) src/StopInstall.c -o $(bin_dir)/StopInstall

$(bin_dir)/StopStorage: src/StopStorage.c
	$(CC) $(CFLAGS) src/StopStorage.c -o $(bin_dir)/StopStorage

clean: 
	@rm $(ALL_FILE)
	@cd $(home_dir) && rm -rf system
	@echo " » 清理完成！\n » Clean Done! "

module_tar: 
	@cd $(home_dir) && zip -r ../ClearBox.zip *
	@echo " » 打包完成，成品：ClearBox.zip！\n » Tar Done, Is: ClearBox.zip! "
