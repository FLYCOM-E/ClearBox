/*
                    GNU GENERAL PUBLIC
                        Version 3
*/

#include "main.h"

void set_language(LangType * lang)
{
    char lang_str[PROP_VALUE_MAX] = "";
    FILE * local_lang_p = popen("settings get system system_locales", "r");
    if (local_lang_p)
    {
        fgets(lang_str, sizeof(lang_str), local_lang_p);
        lang_str[strcspn(lang_str, "\n")] = '\0';
        char * p = strchr(lang_str, ',');
        if (p)
        {
            * p = '\0';
        }
        pclose(local_lang_p);
    }
    if (strstr(lang_str, "zh-CN") ||
       strstr(lang_str, "Hans"))
    {
        * lang = LANG_zh_CN;
    }
    else if (strstr(lang_str, "zh-TW") ||
            strstr(lang_str, "Hant"))
    {
        * lang = LANG_zh_TW;
    }
    else
    {
        * lang = LANG_en_US;
    }
    return;
}
