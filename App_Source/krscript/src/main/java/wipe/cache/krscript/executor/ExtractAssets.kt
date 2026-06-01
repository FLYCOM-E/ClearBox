package wipe.cache.krscript.executor

import android.content.Context
import wipe.cache.common.shared.FileWrite.getPrivateFilePath
import wipe.cache.common.shared.FileWrite.writePrivateFile
import wipe.cache.common.shared.FileWrite.writePrivateShellFile

/**
 * Created by Hello on 2018/04/03.
 */
class ExtractAssets(private val context: Context) {
    private fun extractScript(fileName: String?): String? {
        var fileName = fileName
        if (fileName == null || fileName.isEmpty()) {
            return null
        }

        if (extractHisotry.containsKey(fileName)) {
            return extractHisotry.get(fileName)
        }

        if (fileName.startsWith("file:///android_asset/")) {
            fileName = fileName.substring("file:///android_asset/".length)
        }

        val filePath = writePrivateShellFile(fileName, fileName, context)

        if (filePath != null) {
            extractHisotry.put(fileName, filePath)
        }

        return filePath
    }

    fun extractResource(fileName: String?): String? {
        var fileName = fileName
        if (fileName == null || fileName.isEmpty()) {
            return null
        }

        if (extractHisotry.containsKey(fileName)) {
            return extractHisotry.get(fileName)
        }

        if (fileName.endsWith(".sh")) {
            return extractScript(fileName)
        }
        if (fileName.startsWith("file:///android_asset/")) {
            fileName = fileName.substring("file:///android_asset/".length)
        }
        val filePath = writePrivateFile(context.getAssets(), fileName, fileName, context)

        if (filePath != null) {
            extractHisotry.put(fileName, filePath)
        }

        return filePath
    }

    fun extractResources(dir: String?): String? {
        var dir = dir
        if (dir == null || dir.isEmpty()) {
            return null
        }

        if (extractHisotry.containsKey(dir)) {
            return extractHisotry.get(dir)
        }

        if (dir.startsWith("file:///android_asset/")) {
            dir = dir.substring("file:///android_asset/".length)
        } else if (dir.endsWith("/")) {
            dir = dir.substring(0, dir.length - 1)
        }

        try {
            val files = context.getAssets().list(dir)
            if (files != null && files.size > 0) {
                for (file in files) {
                    val relativePath = dir + "/" + file
                    extractResources(relativePath)
                }
                val outputDir = getExtractPath(dir)
                extractHisotry.put(dir, outputDir)
                return outputDir
            } else {
                return extractResource(dir)
            }
        } catch (ignored: Exception) {
        }

        return ""
    }

    fun getExtractPath(file: String): String {
        return getPrivateFilePath(
            context,
            (if (file.startsWith("file:///android_asset/")) (file.substring("file:///android_asset/".length)) else file)
        )
    }

    companion object {
        // 用于记录已经提取过的资源，避免重复提取浪费性能
        private val extractHisotry = HashMap<String?, String?>()
    }
}
