package wipe.cache.main

import android.content.Context
import wipe.cache.krscript.executor.ScriptEnvironment
import wipe.cache.krscript.model.PageNode
import java.io.InputStream
import java.nio.charset.Charset
import java.util.HashMap

class KrScriptConfig {
    private val EXECUTOR_CORE_DEFAULT = "file:///android_asset/executor.sh"
    private val PAGE_LIST_CONFIG_DEFAULT = "file:///android_asset/more.xml"
    private val FAVORITE_CONFIG_DEFAULT = "file:///android_asset/favorites.xml"
    private val BEFORE_START_SH_DEFAULT = ""
    private val DEFAULT_CONFIG = "Config.conf"
    
    fun init(context: Context): KrScriptConfig {
        return init(context, null)
    }

    /**
     * 初始化配置并初始化脚本执行环境。
     *
     * @param context Context
     * @param knownRootResult 调用方已完成的 Root 检查结果；传 null 则跳过 Root 检查（由 ScriptEnvironment 自行检测）
     */
    fun init(context: Context, knownRootResult: Boolean?): KrScriptConfig {
        if (configInfo == null) {
            configInfo = HashMap()
            configInfo!![EXECUTOR_CORE] = EXECUTOR_CORE_DEFAULT
            configInfo!![PAGE_LIST_CONFIG] = PAGE_LIST_CONFIG_DEFAULT
            configInfo!![FAVORITE_CONFIG] = FAVORITE_CONFIG_DEFAULT
            configInfo!![TOOLKIT_DIR] = TOOLKIT_DIR_DEFAULT
            configInfo!![BEFORE_START_SH] = BEFORE_START_SH_DEFAULT

            try {
                val fileName = DEFAULT_CONFIG 
                var actualFileName = fileName
                if (fileName.startsWith(ASSETS_FILE)) {
                    actualFileName = fileName.substring(ASSETS_FILE.length)
                }
                val inputStream = context.assets.open(actualFileName)
                val bytes = ByteArray(inputStream.available())
                inputStream.read(bytes)
                val rows = String(bytes, Charset.defaultCharset()).split("\n")
                for (row in rows) {
                    val rowText = row.trim()
                    if (!rowText.startsWith("#") && rowText.contains("=")) {
                        val separator = rowText.indexOf("=")
                        val key = rowText.substring(0, separator).trim()
                        val value = rowText.substring(separator + 2, rowText.length - 1).trim()
                        configInfo!!.remove(key)
                        configInfo!![key] = value
                    }
                }
            } catch (_: Exception) {
            }

            ScriptEnvironment.init(context, getExecutorCore(), getToolkitDir(), knownRootResult)
        }

        return this
    }

    fun getVariables(): HashMap<String, String>? {
        return configInfo
    }

    private fun getExecutorCore(): String {
        val info = configInfo
        if (info != null && info.containsKey(EXECUTOR_CORE)) {
            return info[EXECUTOR_CORE] ?: EXECUTOR_CORE_DEFAULT
        }
        return EXECUTOR_CORE_DEFAULT
    }

    private fun getToolkitDir(): String {
        val info = configInfo
        if (info != null && info.containsKey(TOOLKIT_DIR)) {
            return info[TOOLKIT_DIR] ?: BEFORE_START_SH_DEFAULT
        }
        return TOOLKIT_DIR_DEFAULT
    }

    fun getPageListConfig(): PageNode? {
        val info = configInfo
        if (info != null) {
            val pageInfo = PageNode("")
            if (info.containsKey(PAGE_LIST_CONFIG_SH)) {
                pageInfo.pageConfigSh = info[PAGE_LIST_CONFIG_SH] ?: ""
            }
            if (info.containsKey(PAGE_LIST_CONFIG)) {
                pageInfo.pageConfigPath = info[PAGE_LIST_CONFIG] ?: ""
            }
            return pageInfo
        }
        return null
    }

    fun getFavoriteConfig(): PageNode? {
        val info = configInfo
        if (info != null) {
            val pageInfo = PageNode("")
            if (info.containsKey(FAVORITE_CONFIG_SH)) {
                pageInfo.pageConfigSh = info[FAVORITE_CONFIG_SH] ?: ""
            }
            if (info.containsKey(FAVORITE_CONFIG)) {
                pageInfo.pageConfigPath = info[FAVORITE_CONFIG] ?: ""
            }
            return pageInfo
        }
        return null
    }

    fun getBeforeStartSh(): String? {
        val info = configInfo
        if (info != null && info.containsKey(BEFORE_START_SH)) {
            return info[BEFORE_START_SH]
        }
        return BEFORE_START_SH_DEFAULT
    }

    companion object {
        private const val ASSETS_FILE = "file:///android_asset/"
        private const val TOOLKIT_DIR = "toolkit_dir"
        private const val TOOLKIT_DIR_DEFAULT = "file:///android_asset/toolkit"
        private const val EXECUTOR_CORE = "executor_core"
        private const val PAGE_LIST_CONFIG = "page_list_config"
        private const val PAGE_LIST_CONFIG_SH = "page_list_config_sh"
        private const val FAVORITE_CONFIG = "favorite_config"
        private const val FAVORITE_CONFIG_SH = "favorite_config_sh"
        private const val BEFORE_START_SH = "Start_sh"
        private var configInfo: HashMap<String, String>? = null
    }
}
