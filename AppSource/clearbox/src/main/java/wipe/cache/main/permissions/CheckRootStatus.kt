package wipe.cache.main.permissions

import wipe.cache.krscript.R as KR
import android.content.DialogInterface
import android.Manifest
import android.content.Context
import android.os.Handler
import android.os.Looper
import androidx.core.content.PermissionChecker
import wipe.cache.common.shell.KeepShellPublic
import wipe.cache.common.ui.DialogHelper
import wipe.cache.main.R
import android.app.Activity
import com.google.android.material.dialog.MaterialAlertDialogBuilder

/**
 * 检查获取root权限
 * Created by helloklf on 2017/6/3.
 */

class CheckRootStatus(var context: Context, private var next: Runnable? = null) {
    var myHandler: Handler = Handler(Looper.getMainLooper())

    var thread: Thread? = null

    fun forceGetRoot() {
        dialogShowing = false
        if (lastCheckResult) {
            val nextRunnable = next
            if (nextRunnable != null) {
                myHandler.post(nextRunnable)
            }
        } else {
            var completed = false
            thread = Thread {
                rootStatus = KeepShellPublic.checkRoot()
                if (completed) return@Thread

                completed = true

                if (lastCheckResult) {
                    val nextRunnable = next
                    if (nextRunnable != null) {
                        myHandler.post(nextRunnable)
                    }
                } else {
                    myHandler.post {
                        KeepShellPublic.tryExit()
                        dialogShowing = true
                        val builder = MaterialAlertDialogBuilder(context)
                                .setTitle(R.string.error_root)
                                .setPositiveButton(R.string.btn_retry) { _: DialogInterface, _: Int ->
                                    KeepShellPublic.tryExit()
                                    if (thread != null && thread!!.isAlive && !thread!!.isInterrupted) {
                                        thread!!.interrupt()
                                        thread = null
                                    }
                                    forceGetRoot()
                                }
                                .setNegativeButton(R.string.btn_exit) { _: DialogInterface, _: Int ->
                                    (context as? Activity)?.finish()
                                }
                        if (!context.resources.getBoolean(R.bool.force_root)) {
                            builder.setNeutralButton(KR.string.btn_skip) { _: DialogInterface, _: Int ->
                                val nextRunnable = next
                                if (nextRunnable != null) {
                                    myHandler.post(nextRunnable)
                                }
                            }
                        }
                        DialogHelper.animDialog(builder).setCancelable(false)
                    }
                }
            }
            thread!!.start()
            Thread {
                Thread.sleep(1000 * 15)

                if (!completed) {
                    KeepShellPublic.tryExit()
                    myHandler.post {
                        dialogShowing = true
                        DialogHelper.confirm(
                            context,
                            context.getString(R.string.error_root),
                            context.getString(R.string.error_su_timeout),
                            null,
                            DialogHelper.DialogButton(context.getString(R.string.btn_retry), {
                                if (thread != null && thread!!.isAlive && !thread!!.isInterrupted) {
                                    thread!!.interrupt()
                                    thread = null
                                }
                                forceGetRoot()
                            }),
                            DialogHelper.DialogButton(context.getString(R.string.btn_exit), {
                                (context as? Activity)?.finish()
                            })
                        )
                    }
                }
            }.start()
        }
    }

    companion object {
        private var rootStatus = false

        @JvmStatic
        var dialogShowing = false
            private set

        private fun checkPermission(context: Context, permission: String): Boolean =
                PermissionChecker.checkSelfPermission(context, permission) == PermissionChecker.PERMISSION_GRANTED

        fun grantPermission(context: Context) {
            val cmd = StringBuilder()
            if (!checkPermission(context, Manifest.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS)) {
                cmd.append("dumpsys deviceidle whitelist +${context.packageName};\n")
            }
            KeepShellPublic.doCmdSync(cmd.toString())
        }

        val lastCheckResult: Boolean
            get() = rootStatus
    }
}
