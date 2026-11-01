package wipe.cache.krscript.config

import android.content.Context
import android.util.Log
import wipe.cache.common.shared.FileWrite
import wipe.cache.common.shell.KeepShellPublic
import wipe.cache.common.shell.RootFile
import wipe.cache.krscript.FileOwner
import java.io.File
import java.io.InputStream

class PathAnalysis(private var context: Context, private var parentDir: String = "") {
    private val ASSETS_FILE = "file:///android_asset/"

    // 解析路径时自动获得
    private var currentAbsPath: String = ""

    fun getCurrentAbsPath(): String {
        return currentAbsPath
    }

    fun parsePath(filePath: String): InputStream? {
        try {
            if (filePath.startsWith(ASSETS_FILE)) {
                currentAbsPath = filePath
                return context.assets.open(filePath.substring(ASSETS_FILE.length))
            } else {
                return getFileByPath(filePath)
            }
        } catch (_: Exception) {
            return null
        }
    }

    private fun pathConcat(parent: String, target: String): String {
        val isAssets = parent.startsWith(ASSETS_FILE)
        val pureParent = if (isAssets) parent.substring(ASSETS_FILE.length) else parent
        try {
            val parentFile = File(pureParent)
            val targetFile = File(target)
            val combinedFile = if (targetFile.isAbsolute) {
                targetFile
            } else {
                File(parentFile, target)
            }
            val canonicalPath = combinedFile.canonicalPath
            val parentCanonical = parentFile.canonicalPath
            if (!canonicalPath.startsWith(parentCanonical)) {
                Log.w("PathAnalysis", "Path traversal attempt blocked: $parent + $target")
            }
            return if (isAssets) {
                ASSETS_FILE + canonicalPath
            } else {
                canonicalPath
            }

        } catch (e: Exception) {
            Log.e("PathAnalysis", "Path concat failed", e)
            val separator = if (pureParent.endsWith("/")) "" else "/"
            return if (isAssets) {
                ASSETS_FILE + pureParent + separator + target
            } else {
                pureParent + separator + target
            }
        }
    }

    private fun useRootOpenFile(filePath: String): InputStream? {
        if (RootFile.fileExists(filePath)) {
            val dir = File(FileWrite.getPrivateFilePath(context, "kr-script"))
            if (!dir.exists()) {
                dir.mkdirs()
            }

            val cachePath = FileWrite.getPrivateFilePath(context, "kr-script/outside_file.cache")
            val fileOwner = FileOwner(context).fileOwner
            KeepShellPublic.doCmdSync(
                    "cp -f \"$filePath\" \"$cachePath\"\n" +
                            "chmod 777 \"$cachePath\"\n" +
                            "chown $fileOwner:$fileOwner \"$cachePath\"\n")
            File(cachePath).run {
                if (exists() && canRead()) {
                    return inputStream()
                }
            }
        }
        return null
    }

    // 在assets里查找文件
    private fun findAssetsResource(filePath: String): InputStream? {
        // 解析成绝对路径
        val relativePath = pathConcat(parentDir, filePath)
        try {
            try {
                // 首先在assets里查找相对路径
                val simplePath = relativePath.substring(ASSETS_FILE.length)
                context.assets.open(simplePath).run {
                    currentAbsPath = relativePath
                    return this
                }
            } catch (_: java.lang.Exception) {
                // 然后再尝试再assets里查找绝对路径
                context.assets.open(filePath).run {
                    currentAbsPath = ASSETS_FILE + filePath
                    return this
                }
            }
        } catch (_: java.lang.Exception) {
            return null
        }
    }

    // 在磁盘上查找文件
    private fun findDiskResource(filePath: String): InputStream? {
        if (parentDir.isNotEmpty()) {
            // 解析成绝对路径
            val relativePath = pathConcat(parentDir, filePath)
            // 尝试使用普通权限读取文件
            File(relativePath).run {
                if (exists() && canRead()) {
                    currentAbsPath = absolutePath
                    return inputStream()
                }
            }
            useRootOpenFile(relativePath)?.run {
                return this
            }
        }

        // 路径相对于当前配置文件没找到文件的话，继续查找相对于数据文件根目录的文件
        val privatePath = File( pathConcat(FileWrite.getPrivateFileDir(context), filePath)).absolutePath
        File(privatePath).run {
            if (exists() && canRead()) {
                currentAbsPath = absolutePath
                return inputStream()
            }
        }
        useRootOpenFile(privatePath)?.run {
            return this
        }

        return null
    }

    private fun getFileByPath(filePath: String): InputStream? {
        try {
            if (filePath.startsWith("/")) {
                currentAbsPath = filePath
                val javaFileInfo = File(filePath)
                return if (javaFileInfo.exists() && javaFileInfo.canRead()) {
                    javaFileInfo.inputStream()
                } else {
                    useRootOpenFile(filePath)
                }
            } else {
                // 如果当前配置文件来源于 assets，则查找依赖资源时也只去assets查找
                return if (parentDir.isNotEmpty() && parentDir.startsWith(ASSETS_FILE)) {
                    findAssetsResource(filePath)
                } else {
                    findDiskResource(filePath)
                }
            }
        } catch (ex: java.lang.Exception) {
            ex.printStackTrace()
        }
        return null
    }
}
