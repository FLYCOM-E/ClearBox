package wipe.cache.common.shell

import android.util.Log
import wipe.cache.common.shared.RootFileInfo

/**
 * Created by Hello on 2018/07/06.
 */

object RootFile {
    fun fileExists(path: String): Boolean {
        return KeepShellPublic.doCmdSync("if [[ -f \"$path\" ]]; then echo 1; fi;").equals("1")
    }

    fun dirExists(path: String): Boolean {
        return KeepShellPublic.doCmdSync("if [[ -d \"$path\" ]]; then echo 1; fi;").equals("1")
    }

    // 处理像 "drwxrwx--x   3 root     root         4096 1970-07-14 17:13 vendor_de/" 这样的数据行
    private fun shellFileInfoRow(row: String, parent: String): RootFileInfo? {
        if (row.startsWith("total ")) {
            return null
        }

        try {
            val file = RootFileInfo()
            val columns = row.trim().split(" ");
            val size = columns[0]
            file.fileSize = size.toLong() * 1024;

            val fileName = row.substring(row.indexOf(size) + size.length + 1);

            if (fileName == "./" || fileName == "../") {
                return null
            }

            if (fileName.endsWith("/")) {
                file.filePath = fileName.substring(0, fileName.length - 1)
                file.isDirectory = true
            } else if (fileName.endsWith("@")) {
                file.filePath = fileName.substring(0, fileName.length - 1)
            } else if (fileName.endsWith("|")) {
                file.filePath = fileName.substring(0, fileName.length - 1)
            } else if (fileName.endsWith("*")) {
                file.filePath = fileName.substring(0, fileName.length - 1)
            } else {
                file.filePath = fileName
            }

            file.parentDir = parent

            return file
        } catch (_: Exception) {
            return null
        }
    }

    fun list(path: String): ArrayList<RootFileInfo> {
        val absPath = if (path.endsWith("/")) path.subSequence(0, path.length - 1).toString() else path
        val files = ArrayList<RootFileInfo>()
        if (dirExists(absPath)) {
            val outputInfo = KeepShellPublic.doCmdSync("busybox ls -1Fs \"$absPath\"")
            Log.d(">>>> files", outputInfo)
            if (outputInfo != "error") {
                val rows = outputInfo.split("\n")
                for (row in rows) {
                    val file = shellFileInfoRow(row, absPath)
                    if (file != null) {
                        files.add(file)
                    } else {
                        Log.e(">>>> Scene", "MapDirError Row -> " + row)
                    }
                }
            }
        } else {
            Log.e(">>>> dir lost", absPath)
        }

        return files
    }
}
