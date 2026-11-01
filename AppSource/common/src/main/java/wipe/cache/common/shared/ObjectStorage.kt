package wipe.cache.common.shared

import android.content.Context
import android.widget.Toast
import java.io.*

open class ObjectStorage<T : Serializable>(private val context: Context) {
    private val objectStorageDir = "objects/"
    protected fun getSaveDir(configFile: String): String {
        return FileWrite.getPrivateFilePath(context, objectStorageDir + configFile)
    }

    open fun load(configFile: String): T? {
        val file = File(getSaveDir(configFile))
        if (file.exists()) {
            var fileInputStream: FileInputStream? = null;
            var objectInputStream: ObjectInputStream? = null;
            try {
                fileInputStream = FileInputStream(file)
                objectInputStream = ObjectInputStream(fileInputStream)
                return objectInputStream.readObject() as T?
            } catch (_: Exception) {
            } finally {
                try {
                    objectInputStream?.close()
                    fileInputStream?.close()
                } catch (_: Exception) {
                }
            }
        }
        return null
    }

    open fun save(obj: T?, configFile: String): Boolean {
        val file = File(getSaveDir(configFile))
        val parentFile = file.parentFile
        if (parentFile?.exists() == false) {
            parentFile.mkdirs()
        }
        if (obj != null) {
            var fileOutputStream: FileOutputStream? = null
            var objectOutputStream: ObjectOutputStream? = null
            try {
                fileOutputStream = FileOutputStream(file)
                objectOutputStream = ObjectOutputStream(fileOutputStream)
                objectOutputStream.writeObject(obj)
                return true
            } catch (_: Exception) {
                Toast.makeText(context, "Storage set error", Toast.LENGTH_SHORT).show()
                return false
            } finally {
                try {
                    objectOutputStream?.close()
                    fileOutputStream?.close()
                } catch (_: Exception) {
                    // .
                }
            }
        } else {
            if (file.exists()) {
                file.delete()
            }
        }
        return true
    }
}
