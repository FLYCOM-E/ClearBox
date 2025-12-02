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
			$(bin_dir)/f2fs_GC

all: $(ALL_FILE)
.PHONY: all

$(core_bin_dir)/ClearBox: src/ClearBox.c
	$(CC) $(CFLAGS) src/ClearBox.c -o $(core_bin_dir)/ClearBox

$(core_bin_dir)/StopCache: src/StopCache.c
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

clean: 
	@rm $(ALL_FILE)
	@echo "清理完成！Clean Done! "

module_tar: 
	@cd $(home_dir) && zip -r ../ClearBox.zip *
	@echo "打包完成，成品：ClearBox.zip！Tar Done, Is: ClearBox.zip! "
