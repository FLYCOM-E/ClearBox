package com.projectkr.shell.permissions
import wipe.cache.module.R as KR
import android.content.DialogInterface
import android.Manifest
import android.content.Context
import android.os.Build
import android.os.Handler
import android.os.Looper
import androidx.core.content.PermissionChecker
import com.omarea.common.shell.KeepShellPublic
import com.omarea.common.ui.DialogHelper
import com.projectkr.shell.R
import kotlin.system.exitProcess
import com.google.android.material.dialog.MaterialAlertDialogBuilder

/**
 * 检查获取root权限
 * Created by helloklf on 2017/6/3.
 */

class CheckRootStatus(var context: Context, private var next: Runnable? = null) {
    var myHandler: Handler = Handler(Looper.getMainLooper())

    var therad: Thread? = null

    fun forceGetRoot() {
        if (lastCheckResult) {
            val nextRunnable = next
            if (nextRunnable != null) {
                myHandler.post(nextRunnable)
            }
        } else {
            var completed = false
            therad = Thread {
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
                        val builder = MaterialAlertDialogBuilder(context)
                                .setTitle(R.string.error_root)
                                .setPositiveButton(R.string.btn_retry) { _: DialogInterface, _: Int ->
                                    KeepShellPublic.tryExit()
                                    if (therad != null && therad!!.isAlive && !therad!!.isInterrupted) {
                                        therad!!.interrupt()
                                        therad = null
                                    }
                                    forceGetRoot()
                                }
                                .setNegativeButton(R.string.btn_exit) { _: DialogInterface, _: Int ->
                                    exitProcess(0)
                                }
                        if (context.resources.getBoolean(R.bool.force_root) != true) {
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
            therad!!.start()
            Thread(Runnable {
                Thread.sleep(1000 * 15)

                if (!completed) {
                    KeepShellPublic.tryExit()
                    myHandler.post {
                        DialogHelper.confirm(context,
                                context.getString(R.string.error_root),
                                context.getString(R.string.error_su_timeout),
                                null,
                                DialogHelper.DialogButton(context.getString(R.string.btn_retry), {
                                    if (therad != null && therad!!.isAlive && !therad!!.isInterrupted) {
                                        therad!!.interrupt()
                                        therad = null
                                    }
                                    forceGetRoot()
                                }),
                                DialogHelper.DialogButton(context.getString(R.string.btn_exit), {
                                    exitProcess(0)
                                }))
                    }
                }
            }).start()
        }
    }

    companion object {
        private var rootStatus = false
        private fun checkPermission(context: Context, permission: String): Boolean =
                PermissionChecker.checkSelfPermission(context, permission) == PermissionChecker.PERMISSION_GRANTED

        fun grantPermission(context: Context) {
            val cmds = StringBuilder()
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                if (!checkPermission(context, Manifest.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS)) {
                    cmds.append("dumpsys deviceidle whitelist +${context.packageName};\n")
                }
            }
            KeepShellPublic.doCmdSync(cmds.toString())
        }

        val lastCheckResult: Boolean
            get() = rootStatus
    }
}
