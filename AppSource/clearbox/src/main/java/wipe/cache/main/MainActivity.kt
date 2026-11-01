package wipe.cache.main

import wipe.cache.krscript.R as KR
import android.Manifest
import android.content.ComponentName
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.view.LayoutInflater
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.CompoundButton
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.Toolbar
import androidx.core.app.ActivityCompat
import androidx.core.content.PermissionChecker
import wipe.cache.common.shared.FilePathResolver
import wipe.cache.common.ui.DialogHelper
import wipe.cache.common.ui.ProgressBarDialog
import wipe.cache.common.ui.SpringUtils
import wipe.cache.krscript.config.PageConfigReader
import wipe.cache.krscript.config.PageConfigSh
import wipe.cache.krscript.model.*
import wipe.cache.krscript.ui.ActionListFragment
import wipe.cache.krscript.ui.ParamsFileChooserRender
import android.os.Looper

class MainActivity : AppCompatActivity() {
    private val progressBarDialog = ProgressBarDialog(this)
    private val handler = Handler(Looper.getMainLooper())
    private var krScriptConfig = KrScriptConfig()

    private lateinit var mainTabhost2: View
    private lateinit var mainTabhost3: View

    private fun checkPermission(permission: String): Boolean = PermissionChecker.checkSelfPermission(this, permission) == PermissionChecker.PERMISSION_GRANTED

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ThemeModeState.switchTheme(this)
        setContentView(R.layout.activity_main)

        val toolbar = findViewById<View>(R.id.toolbar) as Toolbar
        setSupportActionBar(toolbar)
        setTitle(R.string.app_name)

        mainTabhost2 = findViewById(R.id.main_tabhost_2)
        mainTabhost3 = findViewById(R.id.main_tabhost_3)

        SpringUtils.springEntrance(mainTabhost2)
        SpringUtils.springEntrance(mainTabhost3)

        krScriptConfig = KrScriptConfig()

        Thread {
            val cachedPages = PageCache.pageItems
            val page2Config = krScriptConfig.getPageListConfig()
            val favoritesConfig = krScriptConfig.getFavoriteConfig()

            val pages = cachedPages ?: run {
                handler.post {
                    window.decorView.post {
                        if (!isFinishing && !isDestroyed) {
                            progressBarDialog.showDialog(getString(R.string.please_wait))
                        }
                    }
                }
                getItems(page2Config!!)
            }
            val favorites = PageCache.homeItems ?: getItems(favoritesConfig!!)

            PageCache.homeItems = null
            PageCache.pageItems = null

            handler.post {
                progressBarDialog.hideDialog()

                if (!favorites.isNullOrEmpty()) {
                    updateFavoritesTab(favorites, favoritesConfig!!)
                } else {
                    mainTabhost2.visibility = View.GONE
                }

                if (!pages.isNullOrEmpty()) {
                    updateMoreTab(pages, page2Config!!)
                } else {
                    mainTabhost3.visibility = View.GONE
                }
            }
        }.start()
        
        if (!(checkPermission(Manifest.permission.READ_EXTERNAL_STORAGE) && checkPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE))) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE), 111)
        }
    }

    private fun getItems(pageNode: PageNode): ArrayList<NodeInfoBase>? {
        var items: ArrayList<NodeInfoBase>? = null
        if (pageNode.pageConfigSh.isNotEmpty()) {
            items = PageConfigSh(this, pageNode.pageConfigSh, null).execute()
        }
        if (items == null && pageNode.pageConfigPath.isNotEmpty()) {
            items = PageConfigReader(this.applicationContext, pageNode.pageConfigPath, null).readConfigXml()
        }
        return items
    }

    private fun updateFavoritesTab(items: ArrayList<NodeInfoBase>, pageNode: PageNode) {
        val favoritesFragment = ActionListFragment.create(items, getKrScriptActionHandler(pageNode, true), null, ThemeModeState.getThemeMode())
        supportFragmentManager.beginTransaction().replace(R.id.list_favorites, favoritesFragment).commitAllowingStateLoss()
    }

    private fun updateMoreTab(items: ArrayList<NodeInfoBase>, pageNode: PageNode) {
        val allItemFragment = ActionListFragment.create(items, getKrScriptActionHandler(pageNode, false), null, ThemeModeState.getThemeMode())
        supportFragmentManager.beginTransaction().replace(R.id.list_pages, allItemFragment).commitAllowingStateLoss()
    }

    private fun reloadFavoritesTab() {
        Thread {
            val favoritesConfig = krScriptConfig.getFavoriteConfig()
            val favorites = getItems(favoritesConfig!!)
            favorites?.run {
                handler.post { updateFavoritesTab(this, favoritesConfig) }
            }
        }.start()
    }

    private fun reloadMoreTab() {
        Thread {
            val page2Config = krScriptConfig.getPageListConfig()
            val pages = getItems(page2Config!!)
            pages?.run {
                handler.post { updateMoreTab(this, page2Config) }
            }
        }.start()
    }

    private fun getKrScriptActionHandler(pageNode: PageNode, isFavoritesTab: Boolean): KrScriptActionHandler {
        return object : KrScriptActionHandler {
            override fun onActionCompleted(runnableNode: RunnableNode) {
                if (runnableNode.autoFinish) {
                    finishAndRemoveTask()
                } else if (runnableNode.reloadPage) {
                    if (isFavoritesTab) reloadFavoritesTab() else reloadMoreTab()
                }
            }

            override fun addToFavorites(clickableNode: ClickableNode, addToFavoritesHandler: KrScriptActionHandler.AddToFavoritesHandler) {
                val page = clickableNode as? PageNode
                    ?: if (clickableNode is RunnableNode) pageNode
                    else return

                val intent = Intent()
                intent.component = ComponentName(this@MainActivity.applicationContext, ActionPage::class.java)
                intent.addFlags(Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS)
                intent.addFlags(Intent.FLAG_ACTIVITY_NO_HISTORY)
                if (clickableNode is RunnableNode) intent.putExtra("autoRunItemId", clickableNode.key)
                intent.putExtra("page", page)
                addToFavoritesHandler.onAddToFavorites(clickableNode, intent)
            }

            override fun onSubPageClick(pageNode: PageNode) { openPage(pageNode) }

            override fun openFileChooser(fileSelectedInterface: ParamsFileChooserRender.FileSelectedInterface): Boolean {
                return chooseFilePath(fileSelectedInterface)
            }
        }
    }

    private var fileSelectedInterface: ParamsFileChooserRender.FileSelectedInterface? = null
    private val ACTION_FILE_PATH_CHOOSER = 65400
    private val ACTION_FILE_PATH_CHOOSER_INNER = 65300

    private fun chooseFilePath(extension: String) {
        try {
            val intent = Intent(this, ActivityFileSelector::class.java)
            intent.putExtra("extension", extension)
            startActivityForResult(intent, ACTION_FILE_PATH_CHOOSER_INNER)
        } catch (_: java.lang.Exception) {
            Toast.makeText(this, R.string.start_file_manager_error, Toast.LENGTH_SHORT).show()
        }
    }

    private fun chooseFilePath(fileSelectedInterface: ParamsFileChooserRender.FileSelectedInterface): Boolean {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return try {
                val suffix = fileSelectedInterface.suffix()
                if (!suffix.isNullOrEmpty()) {
                    chooseFilePath(suffix)
                } else {
                    val intent = Intent(Intent.ACTION_GET_CONTENT)
                    val mimeType = fileSelectedInterface.mimeType()
                    intent.type = mimeType ?: "*/*"
                    intent.addCategory(Intent.CATEGORY_OPENABLE)
                    startActivityForResult(intent, ACTION_FILE_PATH_CHOOSER)
                }
                this.fileSelectedInterface = fileSelectedInterface
                true
            } catch (_: java.lang.Exception) {
                false
            }
        }
        if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            Toast.makeText(this, getString(KR.string.kr_write_external_storage), Toast.LENGTH_LONG).show()
            requestPermissions(arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE), 2)
            return false
        }
        return try {
            val suffix = fileSelectedInterface.suffix()
            if (!suffix.isNullOrEmpty()) {
                chooseFilePath(suffix)
            } else {
                val intent = Intent(Intent.ACTION_GET_CONTENT)
                val mimeType = fileSelectedInterface.mimeType()
                intent.type = mimeType ?: "*/*"
                intent.addCategory(Intent.CATEGORY_OPENABLE)
                startActivityForResult(intent, ACTION_FILE_PATH_CHOOSER)
            }
            this.fileSelectedInterface = fileSelectedInterface
            true
        } catch (_: java.lang.Exception) {
            false
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        if (requestCode == ACTION_FILE_PATH_CHOOSER) {
            val result = if (data == null || resultCode != RESULT_OK) null else data.data
            if (fileSelectedInterface != null) {
                if (result != null) fileSelectedInterface?.onFileSelected(getPath(result))
                else fileSelectedInterface?.onFileSelected(null)
            }
            this.fileSelectedInterface = null
        } else if (requestCode == ACTION_FILE_PATH_CHOOSER_INNER) {
            val absPath = if (data == null || resultCode != RESULT_OK) null else data.getStringExtra("file")
            fileSelectedInterface?.onFileSelected(absPath)
            this.fileSelectedInterface = null
        }
        super.onActivityResult(requestCode, resultCode, data)
    }

    private fun getPath(uri: Uri): String? {
        return try { FilePathResolver().getPath(this, uri) } catch (_: Exception) { null }
    }

    fun openPage(pageNode: PageNode) { OpenPageHelper(this).openPage(pageNode) }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            R.id.option_menu_info -> {
                val layoutInflater = LayoutInflater.from(this)
                val layout = layoutInflater.inflate(R.layout.dialog_about, null)
                val transparentUi = layout.findViewById<CompoundButton>(R.id.transparent_ui)
                val themeConfig = ThemeConfig(this)
                transparentUi.setOnClickListener {
                    val isChecked = (it as CompoundButton).isChecked
                    if (isChecked) {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                            themeConfig.setAllowTransparentUI(true)
                            finish()
                            startActivity(intent)
                        } else {
                            if (!checkPermission(Manifest.permission.READ_EXTERNAL_STORAGE)) {
                                it.isChecked = false
                                Toast.makeText(this@MainActivity, KR.string.kr_write_external_storage, Toast.LENGTH_SHORT).show()
                            } else {
                                themeConfig.setAllowTransparentUI(true)
                                finish()
                                startActivity(intent)
                            }
                        }
                    } else {
                        themeConfig.setAllowTransparentUI(false)
                        finish()
                        startActivity(intent)
                    }
                }
                transparentUi.isChecked = themeConfig.getAllowTransparentUI()
                DialogHelper.customDialog(this, layout)
            }
            R.id.option_menu_reboot -> { DialogPower(this).showPowerMenu() }
        }
        return super.onOptionsItemSelected(item)
    }
}
