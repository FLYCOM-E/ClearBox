package wipe.cache.main

import android.Manifest
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.PermissionChecker
import androidx.core.splashscreen.SplashScreen.Companion.installSplashScreen
import wipe.cache.common.shell.ShellExecutor
import wipe.cache.krscript.executor.ScriptEnvironment
import wipe.cache.main.permissions.CheckRootStatus
import java.io.BufferedReader
import java.io.DataOutputStream

class SplashActivity : AppCompatActivity() {
    private val myHandler = Handler(Looper.getMainLooper())
    private var navigating = false

    override fun onCreate(savedInstanceState: Bundle?) {
        val splashScreen = installSplashScreen()
        super.onCreate(savedInstanceState)
        splashScreen.setKeepOnScreenCondition { !navigating && !CheckRootStatus.dialogShowing }

        if (ScriptEnvironment.isInited) {
            if (isTaskRoot) {
                gotoHome()
            } else {
                finish()
            }
            return
        }

        CheckRootStatus(this) { startToFinish() }.forceGetRoot()
    }

    private fun checkPermission(permission: String): Boolean = PermissionChecker.checkSelfPermission(this.applicationContext, permission) == PermissionChecker.PERMISSION_GRANTED

    private fun startToFinish() {
        Thread {
            CheckRootStatus.grantPermission(this)
            if (!(checkPermission(Manifest.permission.READ_EXTERNAL_STORAGE) && checkPermission(
                    Manifest.permission.WRITE_EXTERNAL_STORAGE
                ))
            ) {
                ActivityCompat.requestPermissions(
                    this@SplashActivity,
                    arrayOf(
                        Manifest.permission.READ_EXTERNAL_STORAGE,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.MOUNT_UNMOUNT_FILESYSTEMS,
                        Manifest.permission.REQUEST_IGNORE_BATTERY_OPTIMIZATIONS,
                        Manifest.permission.WAKE_LOCK
                    ),
                    0x11
                )
            }
            val config = KrScriptConfig().init(this, CheckRootStatus.lastCheckResult)
            runBeforeStartSh(config)
            myHandler.post { gotoHome() }
        }.start()
    }

    private fun runBeforeStartSh(config: KrScriptConfig) {
        val beforeStartSh = config.getBeforeStartSh() ?: return
        if (beforeStartSh.isEmpty()) {
            return
        }
        try {
            val process = if (CheckRootStatus.lastCheckResult) ShellExecutor.getSuperUserRuntime() else ShellExecutor.getRuntime()
            val outputStream = DataOutputStream(process.outputStream)
            ScriptEnvironment.executeShell(
                this,
                outputStream,
                beforeStartSh,
                config.getVariables() as HashMap<String?, String?>?,
                null,
                "splash"
            )
            StreamReadThread(process.inputStream.bufferedReader()).start()
            StreamReadThread(process.errorStream.bufferedReader()).start()
            process.waitFor()
        } catch (_: Exception) {
        }
    }

    private fun gotoHome() {
        navigating = true
        if (this.intent != null && this.intent.hasExtra("JumpActionPage") && this.intent.getBooleanExtra("JumpActionPage", false)) {
            val actionPage = Intent(this.applicationContext, ActionPage::class.java)
            actionPage.putExtras(this.intent)
            startActivity(actionPage)
        } else {
            val home = Intent(this.applicationContext, MainActivity::class.java)
            startActivity(home)
        }
        finish()
    }

    private class StreamReadThread(private val reader: BufferedReader) : Thread() {
        override fun run() {
            try {
                while (true) {
                    val line = reader.readLine() ?: break
                    Log.d("ClearBoxSplash", line)
                }
            } catch (_: Exception) {
            }
        }
    }
}
