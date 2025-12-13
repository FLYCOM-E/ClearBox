CFLAGS = --static -O2 -s -Wall

home_dir = Module_Files
bin_dir = Module_Files/wipe_cache

ALL_FILE = $(home_dir)/BashCore \
			$(bin_dir)/StopCache \
			$(bin_dir)/AppClean \
			$(bin_dir)/CacheClean \
			$(bin_dir)/Dexoat \
			$(bin_dir)/FreeZer \
			$(bin_dir)/StorageRuleClean \
			$(bin_dir)/StorageClean \
			$(bin_dir)/F2fs_GC \
			$(bin_dir)/StopInstall \
			$(bin_dir)/StopStorage \
			$(bin_dir)/FileClean \
			$(bin_dir)/FileAll

all: $(ALL_FILE)
.PHONY: all

$(bin_dir)/StopCache: src/StopCache.c
	@mkdir -p $(bin_dir)
	$(CC) $(CFLAGS) src/StopCache.c -o $(bin_dir)/StopCache

$(home_dir)/BashCore: src/BashCore.c
	$(CC) $(CFLAGS) src/BashCore.c -o $(home_dir)/BashCore

$(bin_dir)/AppClean: src/AppClean.c
	$(CC) $(CFLAGS) src/AppClean.c -o $(bin_dir)/AppClean

$(bin_dir)/CacheClean: src/CacheClean.c
	$(CC) $(CFLAGS) src/CacheClean.c -o $(bin_dir)/CacheClean

$(bin_dir)/Dexoat: src/Dexoat.c
	$(CC) $(CFLAGS) src/Dexoat.c -o $(bin_dir)/Dexoat

$(bin_dir)/FreeZer: src/FreeZer.c
	$(CC) $(CFLAGS) src/FreeZer.c -o $(bin_dir)/FreeZer

$(bin_dir)/StorageRuleClean: src/StorageRuleClean.c
	$(CC) $(CFLAGS) src/StorageRuleClean.c -o $(bin_dir)/StorageRuleClean

$(bin_dir)/StorageClean: src/StorageClean.c
	$(CC) $(CFLAGS) src/StorageClean.c -o $(bin_dir)/StorageClean

$(bin_dir)/F2fs_GC: src/F2fs_GC.c
	$(CC) $(CFLAGS) src/F2fs_GC.c -o $(bin_dir)/F2fs_GC

$(bin_dir)/StopInstall: src/StopInstall.c
	$(CC) $(CFLAGS) src/StopInstall.c -o $(bin_dir)/StopInstall

$(bin_dir)/StopStorage: src/StopStorage.c
	$(CC) $(CFLAGS) src/StopStorage.c -o $(bin_dir)/StopStorage

$(bin_dir)/FileClean: src/FileClean.c
	$(CC) $(CFLAGS) src/FileClean.c -o $(bin_dir)/FileClean

$(bin_dir)/FileAll: src/FileAll.c
	$(CC) $(CFLAGS) src/FileAll.c -o $(bin_dir)/FileAll

clean: 
	@rm $(ALL_FILE)
	@cd $(home_dir) && rm -rf wipe_cache
	@echo " » 清理完成！\n » Clean Done! "

module_tar: 
	@cd $(home_dir) && zip -r ../ClearBox.zip *
	@echo " » 打包完成，成品：ClearBox.zip！\n » Tar Done, Is: ClearBox.zip! "
