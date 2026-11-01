package wipe.cache.krscript.executor

import android.content.Context
import wipe.cache.common.shell.ShellTranslation
import wipe.cache.krscript.model.ShellHandlerBase
import java.io.BufferedReader
import java.io.InputStreamReader

class SimpleShellWatcher {
    /**
     * 设置日志处理Handler
     * 
     * @param process          Runtime进程
     * @param shellHandlerBase ShellHandlerBase
     */
    fun setHandler(
        context: Context,
        process: Process,
        shellHandlerBase: ShellHandlerBase,
        onExit: Runnable?
    ) {
        val shellTranslation = ShellTranslation(context)

        val inputStream = process.inputStream
        val errorStream = process.errorStream
        val reader = Thread {
            var line: String?
            try {
                val bufferedReader = BufferedReader(InputStreamReader(inputStream, "UTF-8"))
                while ((bufferedReader.readLine().also { line = it }) != null) {
                    shellHandlerBase.sendMessage(
                        shellHandlerBase.obtainMessage(
                            ShellHandlerBase.EVENT_REDE,
                            shellTranslation.resolveRow(line!!) + "\n"
                        )
                    )
                }
            } catch (_: Exception) {
                // .
            }
        }
        val readerError = Thread {
            var line: String?
            try {
                val bufferedReader = BufferedReader(InputStreamReader(errorStream, "UTF-8"))
                while ((bufferedReader.readLine().also { line = it }) != null) {
                    shellHandlerBase.sendMessage(
                        shellHandlerBase.obtainMessage(
                            ShellHandlerBase.EVENT_READ_ERROR,
                            shellTranslation.resolveRow(line!!) + "\n"
                        )
                    )
                }
            } catch (_: Exception) {
                // .
            }
        }
        val waitExit = Thread {
            var status = -1
            try {
                status = process.waitFor()
            } catch (e: InterruptedException) {
                e.printStackTrace()
            } finally {
                shellHandlerBase.sendMessage(
                    shellHandlerBase.obtainMessage(
                        ShellHandlerBase.EVENT_EXIT,
                        status
                    )
                )
                if (reader.isAlive) {
                    reader.interrupt()
                }
                if (readerError.isAlive) {
                    readerError.interrupt()
                }
                onExit?.run()
            }
        }

        reader.start()
        readerError.start()
        waitExit.start()
    }
}
