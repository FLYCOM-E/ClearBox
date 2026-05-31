package wipe.cache.common.shared

import android.annotation.SuppressLint
import android.content.ContentUris
import android.content.Context
import android.database.Cursor
import android.net.Uri
import android.os.Environment
import android.provider.DocumentsContract
import android.provider.MediaStore
import android.provider.OpenableColumns
import java.io.BufferedOutputStream
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream

class FilePathResolver {
    /**
     * 返回文件本地绝对路径
     * 
     * @param context
     * @param uri
     * @return path of the selected image file from gallery
     */
    @SuppressLint("NewApi")
    fun getPath(context: Context, uri: Uri): String? {
        // DocumentProvider
        if (DocumentsContract.isDocumentUri(context, uri)) {
            // ExternalStorageProvider

            if (isExternalStorageDocument(uri)) {
                val docId = DocumentsContract.getDocumentId(uri)
                val split: Array<String?> =
                    docId.split(":".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()
                val type = split[0]

                if ("primary".equals(type, ignoreCase = true)) {
                    return (Environment.getExternalStorageDirectory().toString() + "/"
                            + split[1])
                }
            } else if (isDownloadsDocument(uri)) {
                val id = DocumentsContract.getDocumentId(uri)
                if (Regex("^[0-9]{1,}").matches(id)) {
                    val contentUri = ContentUris.withAppendedId(
                        Uri.parse("content://downloads/public_downloads"),
                        id.toLong()
                    )
                    return getDataColumn(context, contentUri, null, null)
                } else {
                    // 拷贝到缓存目录并返回文件
                    val fileName = getFileName(context, uri)
                    val cacheDir: File = getDocumentCacheDir(context)
                    val file: File? = generateFileName(fileName, cacheDir)
                    var destinationPath: String? = null
                    if (file != null) {
                        destinationPath = file.getAbsolutePath()
                        saveFileFromUri(context, uri, destinationPath)
                    }

                    return destinationPath
                }
            } else if (isMediaDocument(uri)) {
                val docId = DocumentsContract.getDocumentId(uri)
                val split: Array<String?> =
                    docId.split(":".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()
                val type = split[0]

                var contentUri: Uri? = null
                if ("image" == type) {
                    contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI
                } else if ("video" == type) {
                    contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI
                } else if ("audio" == type) {
                    contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI
                }

                val selection = "_id=?"
                val selectionArgs = arrayOf<String?>(split[1])

                return getDataColumn(
                    context, contentUri!!, selection,
                    selectionArgs
                )
            }
        } else if ("content".equals(uri.getScheme(), ignoreCase = true)) {
            // Return the remote address

            if (isGooglePhotosUri(uri)) return uri.getLastPathSegment()

            return getDataColumn(context, uri, null, null)
        } else if ("file".equals(uri.getScheme(), ignoreCase = true)) {
            return uri.getPath()
        }

        return null
    }

    /**
     * Get the value of the data column for this Uri. This is useful for
     * MediaStore Uris, and other file-based ContentProviders.
     * 
     * @param context       The context.
     * @param uri           The Uri to query.
     * @param selection     (Optional) Filter used in the query.
     * @param selectionArgs (Optional) Selection arguments used in the query.
     * @return The value of the _data column, which is typically a file path.
     */
    private fun getDataColumn(
        context: Context, uri: Uri,
        selection: String?, selectionArgs: Array<String?>?
    ): String? {
        var cursor: Cursor? = null
        val column = "_data"
        val projection = arrayOf<String?>(column)

        try {
            cursor = context.getContentResolver().query(
                uri, projection,
                selection, selectionArgs, null
            )
            if (cursor != null && cursor.moveToFirst()) {
                val index = cursor.getColumnIndexOrThrow(column)
                return cursor.getString(index)
            }
        } catch (ignored: Exception) {
        } finally {
            if (cursor != null) cursor.close()
        }
        return null
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is ExternalStorageProvider.
     */
    private fun isExternalStorageDocument(uri: Uri): Boolean {
        return "com.android.externalstorage.documents" == uri
            .getAuthority()
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is DownloadsProvider.
     */
    private fun isDownloadsDocument(uri: Uri): Boolean {
        return "com.android.providers.downloads.documents" == uri
            .getAuthority()
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is MediaProvider.
     */
    private fun isMediaDocument(uri: Uri): Boolean {
        return "com.android.providers.media.documents" == uri
            .getAuthority()
    }

    /**
     * @param uri The Uri to check.
     * @return Whether the Uri authority is Google Photos.
     */
    private fun isGooglePhotosUri(uri: Uri): Boolean {
        return "com.google.android.apps.photos.content" == uri
            .getAuthority()
    }

    fun getFileName(context: Context, uri: Uri): String? {
        val mimeType = context.getContentResolver().getType(uri)
        var filename: String? = null
        if (mimeType == null) {
            val path = getPath(context, uri)
            if (path == null) {
                filename = getName(uri.toString())
            } else {
                val file = File(path)
                filename = file.getName()
            }
        } else {
            val returnCursor = context.getContentResolver().query(
                uri, null,
                null, null, null
            )
            if (returnCursor != null) {
                val nameIndex = returnCursor.getColumnIndex(OpenableColumns.DISPLAY_NAME)
                returnCursor.moveToFirst()
                filename = returnCursor.getString(nameIndex)
                returnCursor.close()
            }
        }

        return filename
    }

    fun getName(filename: String?): String? {
        if (filename == null) {
            return null
        }
        val index = filename.lastIndexOf('/')
        return filename.substring(index + 1)
    }

    companion object {
        fun getDocumentCacheDir(context: Context): File {
            val dir = File(context.getCacheDir(), "documents")
            if (!dir.exists()) {
                dir.mkdirs()
            }

            return dir
        }

        fun generateFileName(name: String?, directory: File?): File? {
            var name = name
            if (name == null) {
                return null
            }

            var file = File(directory, name)

            if (file.exists()) {
                var fileName: String? = name
                var extension = ""
                val dotIndex = name.lastIndexOf('.')
                if (dotIndex > 0) {
                    fileName = name.substring(0, dotIndex)
                    extension = name.substring(dotIndex)
                }

                var index = 0

                while (file.exists()) {
                    index++
                    name = fileName + '(' + index + ')' + extension
                    file = File(directory, name)
                }
            }

            try {
                if (!file.createNewFile()) {
                    return null
                }
            } catch (e: IOException) {
                return null
            }

            return file
        }


        private fun saveFileFromUri(context: Context, uri: Uri, destinationPath: String?) {
            var `is`: InputStream? = null
            var bos: BufferedOutputStream? = null
            try {
                `is` = context.getContentResolver().openInputStream(uri)
                bos = BufferedOutputStream(FileOutputStream(destinationPath, false))
                val buf = ByteArray(1024)
                `is`!!.read(buf)
                do {
                    bos.write(buf)
                } while (`is`.read(buf) != -1)
            } catch (ignored: IOException) {
            } finally {
                try {
                    if (`is` != null) `is`.close()
                    if (bos != null) bos.close()
                } catch (e: IOException) {
                    e.printStackTrace()
                }
            }
        }
    }
}
