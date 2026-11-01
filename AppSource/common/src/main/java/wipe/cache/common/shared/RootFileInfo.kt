package wipe.cache.common.shared

class RootFileInfo {
    constructor()

    var parentDir: String = ""
    var filePath: String = ""
    var isDirectory: Boolean = false
    var fileSize: Long = 0;

    val fileName: String
        get() {
            if (filePath.endsWith("/")) {
                return filePath.substring(0, filePath.length - 1)
            }
            return filePath
        }

    val absolutePath: String
        get() = "$parentDir/$fileName"

    fun length(): Long {
        return this.fileSize
    }
}
