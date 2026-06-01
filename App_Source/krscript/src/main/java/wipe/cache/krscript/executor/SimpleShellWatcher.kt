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

        val inputStream = process.getInputStream()
        val errorStream = process.getErrorStream()
        val reader = Thread(object : Runnable {
            override fun run() {
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
                } catch (ignored: Exception) {
                }
            }
        })
        val readerError = Thread(object : Runnable {
            override fun run() {
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
                } catch (ignored: Exception) {
                }
            }
        })
        val processFinal = process
        val waitExit = Thread(object : Runnable {
            override fun run() {
                var status = -1
                try {
                    status = processFinal.waitFor()
                } catch (e: InterruptedException) {
                    e.printStackTrace()
                } finally {
                    shellHandlerBase.sendMessage(
                        shellHandlerBase.obtainMessage(
                            ShellHandlerBase.EVENT_EXIT,
                            status
                        )
                    )
                    if (reader.isAlive()) {
                        reader.interrupt()
                    }
                    if (readerError.isAlive()) {
                        readerError.interrupt()
                    }
                    if (onExit != null) {
                        onExit.run()
                    }
                }
            }
        })

        reader.start()
        readerError.start()
        waitExit.start()
    }
}
