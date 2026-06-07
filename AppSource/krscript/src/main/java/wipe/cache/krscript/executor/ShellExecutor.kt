package wipe.cache.krscript.executor

import android.content.Context
import android.os.Build
import android.util.Log
import android.widget.Toast
import wipe.cache.krscript.executor.ScriptEnvironment.executeResultRoot
import wipe.cache.krscript.executor.ScriptEnvironment.executeShell
import wipe.cache.krscript.executor.ScriptEnvironment.runtime
import wipe.cache.krscript.model.RunnableNode
import wipe.cache.krscript.model.RunnableNode.Companion.shellModeBgTask
import wipe.cache.krscript.model.ShellHandlerBase
import java.io.DataOutputStream

/**
 * Created by Hello on 2018/04/01.
 */
class ShellExecutor {
    private var started = false
    private val sessionTag = "clearbox_" + System.currentTimeMillis()
    private fun killProcess(context: Context) {
        executeResultRoot(
            context,
            String.format("kill -s 1 `pgrep -f %s`", sessionTag),
            null
        )
        // KeepShellPublic.INSTANCE.doCmdSync(String.format("kill -s 1 `pgrep -f %s`", sessionTag));
    }

    /**
     * 执行脚本
     */
    fun execute(
        context: Context,
        nodeInfo: RunnableNode,
        cmds: String?,
        onExit: Runnable?,
        params: HashMap<String?, String?>?,
        shellHandlerBase: ShellHandlerBase
    ): Process? {
        if (started) {
            return null
        }

        val process = runtime
        if (process == null) {
            Toast.makeText(context, "START ERR", Toast.LENGTH_SHORT).show()
            if (onExit != null) {
                onExit.run()
            }
        } else {
            val forceStopRunnable: Runnable? =
                if (nodeInfo.interruptable || nodeInfo.shell == shellModeBgTask) (object :
                    Runnable {
                    override fun run() {
                        /*
                    // 没啥用，这个pid和在shell创建的子进程不是父子关系，杀死此进程对shell里创建的进程毫无影响
                    int pid = -1;
                    if (process.getClass().getName().equals("java.lang.UNIXProcess")) {
                        try {
                            Class cl = process.getClass();
                            Field field = cl.getDeclaredField("pid");
                            field.setAccessible(true);
                            Object pidObject = field.get(process);
                            pid = (Integer) pidObject;
                        } catch (Exception ignored) {}
                    }
                    */
                        killProcess(context)

                        try {
                            process.getInputStream().close()
                        } catch (ignored: Exception) {
                        }
                        try {
                            process.getOutputStream().close()
                        } catch (ignored: Exception) {
                        }
                        try {
                            process.getErrorStream().close()
                        } catch (ignored: Exception) {
                        }

                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                            try {
                                process.destroyForcibly()
                            } catch (ex: Exception) {
                                Log.e("KrScriptError", "" + ex.message)
                            }
                        } else {
                            try {
                                process.destroy()
                            } catch (ex: Exception) {
                                Log.e("KrScriptError", "" + ex.message)
                            }
                        }
                    }
                }) else null
            SimpleShellWatcher().setHandler(context, process, shellHandlerBase, onExit)

            val outputStream = process.getOutputStream()
            val dataOutputStream = DataOutputStream(outputStream)
            try {
                shellHandlerBase.sendMessage(
                    shellHandlerBase.obtainMessage(
                        ShellHandlerBase.EVENT_START,
                        "shell@android:\n"
                    )
                )
                shellHandlerBase.sendMessage(
                    shellHandlerBase.obtainMessage(
                        ShellHandlerBase.EVENT_START,
                        cmds + "\n\n"
                    )
                )
                shellHandlerBase.onStart(forceStopRunnable)
                dataOutputStream.writeBytes("sleep 0.2;\n")

                executeShell(context, dataOutputStream, cmds, params, nodeInfo, sessionTag)
            } catch (ex: Exception) {
                process.destroy()
            }
            started = true
        }
        return process
    }
}
