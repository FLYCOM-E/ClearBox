/*
                    GNU GENERAL PUBLIC
                        Version 3

         此 Code 来自 ClearBox 模块，用于 NCDU 功能
*/

#include "INCLUDE/main.h"

#define SERVER_NAME "ncdu"

#define DIR_MAX 1024

struct info
{
    char name[NAME_MAX];
    char path[PATH_MAX];
    char mode;
    int64_t byte;
    double size;
    char unit;
};

static int cmp_byte(const void * a, const void * b);

int ncdu(char * path)
{
    struct info path_info[DIR_MAX];
    
    struct dirent * entry;
    DIR * path_dp = opendir(path);
    if (path_dp == NULL)
    {
        write_log(work_dir, SERVER_NAME, L_OPEN_PATH_FAILED, path, strerror(errno));
        return -1;
    }
    
    int count = 0;
    while ((entry = readdir(path_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        // NAME
        snprintf(path_info[count].name, sizeof(path_info[count].name), "%s", entry -> d_name);
        
        // PATH
        snprintf(path_info[count].path, sizeof(path_info[count].path), "%s/%s", path, entry -> d_name);
        
        // MODE
        struct stat st;
        if (lstat(path_info[count].path, &st) == -1)
        {
            continue;
        }
        if (S_ISREG(st.st_mode))
        {
            path_info[count].mode = 'F';
        }
        else if (S_ISDIR(st.st_mode))
        {
            path_info[count].mode = 'D';
        }
        else
        {
            path_info[count].mode = 'N'; // NULL
        }
        
        // SIZE
        path_info[count].byte = get_path_size(path_info[count].path);
        path_info[count].size = byte_to_size(path_info[count].byte, &path_info[count].unit);
        
        count++;
        if (count >= DIR_MAX)
        {
            break;
        }
    }
    
    qsort(path_info, (size_t)count, sizeof(struct info), cmp_byte);
    
    for (int i = 0; i < count; i++)
    {
        // PRINT:
        // NAME PATH SIZE UNIT MODE
        printf("%s|%s|%.2f|%c|%c\n",
                path_info[i].name,
                path_info[i].path,
                path_info[i].size,
                path_info[i].unit,
                path_info[i].mode
              );
    }
    
    closedir(path_dp);
    return 0;
}

static int cmp_byte(const void * a, const void * b)
{
    const struct info * a_p = (const struct info *)a;
    const struct info * b_p = (const struct info *)b;
    
    if (a_p -> byte > b_p -> byte)
    {
        return -1;
    }
    if (a_p -> byte < b_p -> byte)
    {
        return 1;
    }
    return 0;
}
