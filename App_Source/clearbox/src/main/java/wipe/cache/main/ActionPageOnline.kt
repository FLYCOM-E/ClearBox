package wipe.cache.main

import wipe.cache.krscript.R as KR
import android.content.DialogInterface
import android.Manifest
import android.app.Activity
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
import android.widget.TextView
import android.widget.Toast
import wipe.cache.common.ui.DialogHelper
import wipe.cache.common.ui.ProgressBarDialog
import wipe.cache.common.ui.ThemeMode
import wipe.cache.krscript.WebViewInjector
import wipe.cache.krscript.ui.ParamsFileChooserRender
import java.util.*
import androidx.activity.OnBackPressedCallback
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import wipe.cache.common.shared.FilePathResolver
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsControllerCompat

class ActionPageOnline : AppCompatActivity() {
    private val progressBarDialog = ProgressBarDialog(this)
    private lateinit var themeMode: ThemeMode

    private lateinit var krOnlineRoot: View
    private lateinit var krOnlineWebview: WebView

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
            WindowCompat.setDecorFitsSystemWindows(window, false)
            window.statusBarColor = Color.TRANSPARENT
        }
        supportActionBar?.hide()
    }
    
    private fun setWindowTitleBar() {
        val window = window
        
        WindowCompat.setDecorFitsSystemWindows(window, false)
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
    
        if (!themeMode.isDarkMode) {
            @Suppress("DEPRECATION")
            window.statusBarColor = Color.WHITE
            @Suppress("DEPRECATION")
            window.navigationBarColor = Color.WHITE
            
            WindowInsetsControllerCompat(window, window.decorView).apply {
                isAppearanceLightStatusBars = true
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    isAppearanceLightNavigationBars = true
                }
            }
        }
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
        super.onDestroy()
    }
}
