package com.projectkr.shell
import wipe.cache.module.R as KR
import android.content.DialogInterface
import android.Manifest
import android.app.Activity
import android.app.DownloadManager
import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.Color
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Handler
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.Toolbar
import android.view.KeyEvent
import android.view.View
import android.view.WindowManager
import android.webkit.*
import android.widget.LinearLayout
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast
import com.omarea.common.shared.FilePathResolver
import com.omarea.common.ui.DialogHelper
import com.omarea.common.ui.ProgressBarDialog
import com.omarea.common.ui.ThemeMode
import wipe.cache.module.WebViewInjector
import wipe.cache.module.downloader.Downloader
import wipe.cache.module.ui.ParamsFileChooserRender
import java.util.*
import androidx.activity.OnBackPressedCallback
import com.google.android.material.dialog.MaterialAlertDialogBuilder

class ActionPageOnline : AppCompatActivity() {
    private val progressBarDialog = ProgressBarDialog(this)
    private lateinit var themeMode: ThemeMode

    private lateinit var krOnlineRoot: View
    private lateinit var krOnlineWebview: WebView
    private lateinit var krDownloadUrl: TextView
    private lateinit var krDownloadState: View
    private lateinit var krDownloadName: TextView
    private lateinit var krDownloadNameCopy: View
    private lateinit var krDownloadUrlCopy: View
    private lateinit var krDownloadProgress: ProgressBar

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        themeMode = ThemeModeState.switchTheme(this)

        setContentView(R.layout.activity_action_page_online)
        val toolbar = findViewById<View>(R.id.toolbar) as Toolbar
        setSupportActionBar(toolbar)
        setTitle(R.string.app_name)

        supportActionBar!!.setHomeButtonEnabled(true)
        supportActionBar!!.setDisplayHomeAsUpEnabled(true)
        toolbar.setNavigationOnClickListener { finish() }

        krOnlineRoot = findViewById(R.id.kr_online_root)
        krOnlineWebview = findViewById(R.id.kr_online_webview)
        krDownloadUrl = findViewById(R.id.kr_download_url)
        krDownloadState = findViewById(R.id.kr_download_state)
        krDownloadName = findViewById(R.id.kr_download_name)
        krDownloadNameCopy = findViewById(R.id.kr_download_name_copy)
        krDownloadUrlCopy = findViewById(R.id.kr_download_url_copy)
        krDownloadProgress = findViewById(R.id.kr_download_progress)
        
        onBackPressedDispatcher.addCallback(this, object : OnBackPressedCallback(true) {
            override fun handleOnBackPressed() {
                if (krOnlineWebview.canGoBack()) {
                    krOnlineWebview.goBack()
                } else {
                    isEnabled = false
                    onBackPressedDispatcher.onBackPressed()
                }
            }
        })
        
        loadIntentData()
    }

    private fun hideWindowTitle() {
        if (Build.VERSION.SDK_INT >= 21) {
            val decorView = window.decorView
            val option = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            decorView.systemUiVisibility = option
            window.statusBarColor = Color.TRANSPARENT
        }
        val actionBar = supportActionBar
        actionBar!!.hide()
    }

    private fun setWindowTitleBar() {
        val window = window
        window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
        window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)

        var flags = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN

        if (!themeMode.isDarkMode) {
            window.statusBarColor = Color.WHITE
            window.navigationBarColor = Color.WHITE
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                flags = flags or View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR or View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR
            } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                flags = flags or View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
            }
        }
        getWindow().decorView.systemUiVisibility = flags
        krOnlineRoot.fitsSystemWindows = true
    }

    private fun loadIntentData() {
        val intent = this.intent
        if (intent.extras != null) {
            val extras = intent.extras
            if (extras != null) {
                if (extras.containsKey("title")) {
                    title = extras.getString("title") ?: ""
                }

                setWindowTitleBar()
                when {
                    extras.containsKey("config") -> initWebview(extras.getString("config"))
                    extras.containsKey("url") -> initWebview(extras.getString("url"))
                }

                if (extras.containsKey("downloadUrl")) {
                    val downloader = Downloader(this)
                    val url = extras.getString("downloadUrl") ?: return
                    val taskAliasId = if (extras.containsKey("taskId")) extras.getString("taskId") ?: UUID.randomUUID().toString() else UUID.randomUUID().toString()

                    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M &&
                            checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                            downloader.saveTaskStatus(taskAliasId, 0)
                            requestPermissions(arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE), 2)
                            DialogHelper.helpInfo(this, "", getString(KR.string.kr_write_external_storage))
                            return
                        }
                    }

                    val downloadId = downloader.downloadBySystem(url, null, null, taskAliasId)
                    if (downloadId != null) {
                        krDownloadUrl.text = url
                        val autoClose = extras.containsKey("autoClose") && extras.getBoolean("autoClose")
                        downloader.saveTaskStatus(taskAliasId, 0)
                        watchDownloadProgress(downloadId, autoClose, taskAliasId)
                    } else {
                        downloader.saveTaskStatus(taskAliasId, -1)
                    }
                }
            }
        }
    }

    private fun initWebview(url: String?) {
        krOnlineWebview.visibility = View.VISIBLE
        krOnlineWebview.webChromeClient = object : WebChromeClient() {
            override fun onJsAlert(view: WebView?, url: String?, message: String?, result: JsResult?): Boolean {
                DialogHelper.animDialog(
                        MaterialAlertDialogBuilder(this@ActionPageOnline)
                                .setMessage(message)
                                .setPositiveButton(KR.string.btn_confirm) { _: DialogInterface, _: Int -> }
                                .setOnDismissListener { result?.confirm() }
                                .create()
                )?.setCancelable(false)
                return true
            }

            override fun onJsConfirm(view: WebView?, url: String?, message: String?, result: JsResult?): Boolean {
                DialogHelper.animDialog(
                        MaterialAlertDialogBuilder(this@ActionPageOnline)
                                .setMessage(message)
                                .setPositiveButton(KR.string.btn_confirm) { _: DialogInterface, _: Int -> result?.confirm() }
                                .setNeutralButton(KR.string.btn_cancel) { _: DialogInterface, _: Int -> result?.cancel() }
                                .create()
                )?.setCancelable(false)
                return true
            }
        }

        krOnlineWebview.webViewClient = object : WebViewClient() {
            override fun onPageFinished(view: WebView?, url: String?) {
                super.onPageFinished(view, url)
                progressBarDialog.hideDialog()
                view?.run { setTitle(this.title) }
            }

            override fun onPageStarted(view: WebView?, url: String?, favicon: Bitmap?) {
                super.onPageStarted(view, url, favicon)
                progressBarDialog.showDialog(getString(R.string.please_wait))
            }

            override fun shouldOverrideUrlLoading(view: WebView?, request: WebResourceRequest?): Boolean {
                return try {
                    val requestUrl = request?.url
                    if (requestUrl != null && requestUrl.scheme?.startsWith("http") != true) {
                        startActivity(Intent(Intent.ACTION_VIEW, Uri.parse(requestUrl.toString())))
                        true
                    } else {
                        super.shouldOverrideUrlLoading(view, request)
                    }
                } catch (e: Exception) {
                    super.shouldOverrideUrlLoading(view, request)
                }
            }
        }

        krOnlineWebview.loadUrl(url ?: return)

        WebViewInjector(krOnlineWebview,
                object : ParamsFileChooserRender.FileChooserInterface {
                    override fun openFileChooser(fileSelectedInterface: ParamsFileChooserRender.FileSelectedInterface): Boolean {
                        return chooseFilePath(fileSelectedInterface)
                    }
                }).inject(this, url.startsWith("file:///android_asset"))
    }

    private var fileSelectedInterface: ParamsFileChooserRender.FileSelectedInterface? = null
    private val ACTION_FILE_PATH_CHOOSER = 65400

    private fun chooseFilePath(fileSelectedInterface: ParamsFileChooserRender.FileSelectedInterface): Boolean {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return try {
                val intent = Intent(Intent.ACTION_GET_CONTENT)
                intent.type = "*/*"
                intent.addCategory(Intent.CATEGORY_OPENABLE)
                startActivityForResult(intent, ACTION_FILE_PATH_CHOOSER)
                this.fileSelectedInterface = fileSelectedInterface
                true
            } catch (ex: java.lang.Exception) {
                false
            }
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE), 2)
            Toast.makeText(this, getString(KR.string.kr_write_external_storage), Toast.LENGTH_LONG).show()
            return false
        } else {
            return try {
                val intent = Intent(Intent.ACTION_GET_CONTENT)
                intent.type = "*/*"
                intent.addCategory(Intent.CATEGORY_OPENABLE)
                startActivityForResult(intent, ACTION_FILE_PATH_CHOOSER)
                this.fileSelectedInterface = fileSelectedInterface
                true
            } catch (ex: java.lang.Exception) {
                false
            }
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        if (requestCode == ACTION_FILE_PATH_CHOOSER) {
            val result = if (data == null || resultCode != Activity.RESULT_OK) null else data.data
            if (fileSelectedInterface != null) {
                if (result != null) fileSelectedInterface?.onFileSelected(getPath(result))
                else fileSelectedInterface?.onFileSelected(null)
            }
            this.fileSelectedInterface = null
        }
        super.onActivityResult(requestCode, resultCode, data)
    }

    private fun getPath(uri: Uri): String? {
        return try { FilePathResolver().getPath(this, uri) } catch (ex: java.lang.Exception) { null }
    }

    override fun onDestroy() {
        stopWatchDownloadProgress()
        super.onDestroy()
    }

    private fun stopWatchDownloadProgress() {
        progressPolling?.cancel()
        progressPolling = null
    }

    var progressPolling: Timer? = null

    private fun watchDownloadProgress(downloadId: Long, autoClose: Boolean, taskAliasId: String) {
        krDownloadState.visibility = View.VISIBLE

        val downloadManager = getSystemService(Context.DOWNLOAD_SERVICE) as DownloadManager
        val query = DownloadManager.Query().setFilterById(downloadId)

        krDownloadNameCopy.setOnClickListener {
            val myClipboard = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
            myClipboard.setPrimaryClip(ClipData.newPlainText("text", krDownloadName.text.toString()))
            Toast.makeText(this@ActionPageOnline, getString(R.string.copy_success), Toast.LENGTH_SHORT).show()
        }
        krDownloadUrlCopy.setOnClickListener {
            val myClipboard = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
            myClipboard.setPrimaryClip(ClipData.newPlainText("text", krDownloadUrl.text.toString()))
            Toast.makeText(this@ActionPageOnline, getString(R.string.copy_success), Toast.LENGTH_SHORT).show()
        }

        val handler = Handler()
        val downloader = Downloader(this)
        progressPolling = Timer()
        progressPolling?.schedule(object : TimerTask() {
            override fun run() {
                val cursor = downloadManager.query(query)
                var fileName = ""
                var absPath = ""
                if (cursor.moveToFirst()) {
                    val downloadBytesIdx = cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR)
                    val totalBytesIdx = cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TOTAL_SIZE_BYTES)
                    val totalBytes = cursor.getLong(totalBytesIdx)
                    val downloadBytes = cursor.getLong(downloadBytesIdx)
                    val ratio = (downloadBytes * 100 / totalBytes).toInt()
                    if (fileName.isEmpty()) {
                        try {
                            val nameColumn = cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_LOCAL_URI)
                            fileName = cursor.getString(nameColumn)
                            absPath = FilePathResolver().getPath(this@ActionPageOnline, Uri.parse(fileName)) ?: ""
                            if (absPath.isNotEmpty()) fileName = absPath
                        } catch (ex: java.lang.Exception) {}
                    }

                    handler.post {
                        krDownloadName.text = fileName
                        krDownloadProgress.progress = ratio
                        krDownloadProgress.isIndeterminate = false
                        setTitle(KR.string.kr_download_downloading)
                        downloader.saveTaskStatus(taskAliasId, ratio)
                    }

                    if (ratio >= 100) {
                        downloader.saveTaskCompleted(downloadId, absPath)
                        handler.post {
                            setTitle(KR.string.kr_download_completed)
                            krDownloadProgress.visibility = View.GONE
                            stopWatchDownloadProgress()
                            val result = Intent()
                            result.putExtra("absPath", absPath)
                            setResult(0, result)
                            if (autoClose) finish()
                        }
                    }
                }
            }
        }, 200, 500)
    }
}
