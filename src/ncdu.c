/*
                    GNU GENERAL PUBLIC
                        Version 3

         此 Code 来自 ClearBox 模块，用于 NCDU 功能
*/

#include "INCLUDE/main.h"

#define SERVER_NAME "ncdu"

struct info
{
    char name[NAME_MAX];
    char path[PATH_MAX];
    char mode;
    double size;
    char unit;
};

int ncdu(char * path)
{
    struct info path_info;
    struct dirent * entry;
    DIR * path_dp = opendir(path);
    if (path_dp == NULL)
    {
        write_log(work_dir, SERVER_NAME, L_OPEN_PATH_FAILED, path, strerror(errno));
        return -1;
    }
    
    while ((entry = readdir(path_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        // NAME
        snprintf(path_info.name, sizeof(path_info.name), "%s", entry -> d_name);
        
        // PATH
        snprintf(path_info.path, sizeof(path_info.path), "%s/%s", path, entry -> d_name);
        
        // MODE
        struct stat st;
        if (lstat(path_info.path, &st) == -1)
        {
            continue;
        }
        if (S_ISREG(st.st_mode))
        {
            path_info.mode = 'F';
        }
        else if (S_ISDIR(st.st_mode))
        {
            path_info.mode = 'D';
        }
        else
        {
            path_info.mode = 'N'; // NULL
        }
        
        // SIZE
        path_info.size = byte_to_size(get_path_size(path_info.path), &path_info.unit);
        
        // PRINT:
        // NAME PATH SIZE UNIT MODE
        printf("%s|%s|%.2f|%c|%c\n", 
                path_info.name,
                path_info.path,
                path_info.size,
                path_info.unit,
                path_info.mode
              );
    }
    
    closedir(path_dp);
    return 0;
}
