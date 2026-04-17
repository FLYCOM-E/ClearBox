package com.projectkr.shell

import android.Manifest
import android.app.Activity
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.os.Environment
import android.view.KeyEvent
import android.view.View
import android.widget.ListView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.Toolbar
import androidx.core.app.ActivityCompat
import androidx.core.content.PermissionChecker
import com.omarea.common.ui.ProgressBarDialog
import com.projectkr.shell.ui.AdapterFileSelector
import java.io.File
import androidx.activity.OnBackPressedCallback

class ActivityFileSelector : AppCompatActivity() {
    companion object {
        val MODE_FILE = 0
        val MODE_FOLDER = 1
    }

    private var adapterFileSelector: AdapterFileSelector? = null
    var extension = ""
    var mode = MODE_FILE

    private lateinit var fileSelectorList: ListView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_file_selector)

        val toolbar = findViewById<View>(R.id.toolbar) as Toolbar
        setSupportActionBar(toolbar)

        supportActionBar!!.setHomeButtonEnabled(true)
        supportActionBar!!.setDisplayHomeAsUpEnabled(true)
        toolbar.setNavigationOnClickListener { finish() }

        fileSelectorList = findViewById(R.id.file_selector_list)

        val extras = intent.extras
        if (extras != null) {
            if (extras.containsKey("extension")) {
                extension = "" + extras.getString("extension")
                if (!extension.startsWith(".")) extension = ".$extension"
                if (extension.isNotEmpty()) title = title.toString() + "($extension)"
            }
            if (extras.containsKey("mode")) {
                mode = extras.getInt("mode")
                if (mode == MODE_FOLDER) title = getString(R.string.title_activity_folder_selector)
            }
        }
        
       onBackPressedDispatcher.addCallback(this, object : OnBackPressedCallback(true) {
            override fun handleOnBackPressed() {
                if (adapterFileSelector != null && adapterFileSelector!!.goParent()) {
                    
                } else {
                    isEnabled = false
                    setResult(Activity.RESULT_CANCELED, Intent())
                    onBackPressedDispatcher.onBackPressed()
                }
            }
       })
       
       loadData()
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        var grant = true
        for (result in grantResults) {
            if (result == PackageManager.PERMISSION_DENIED) grant = false
        }
        if (requestCode == 111) {
            if (!grant) Toast.makeText(applicationContext, "没有读取文件的权限！", Toast.LENGTH_LONG).show()
            else loadData()
        }
    }

    private fun checkPermission(permission: String): Boolean =
            PermissionChecker.checkSelfPermission(this, permission) == PermissionChecker.PERMISSION_GRANTED

    private fun requestPermissions() {
        ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE), 111)
    }

    override fun onResume() {
        super.onResume()
        loadData()
    }

    private fun loadData() {
        if (checkPermission(Manifest.permission.READ_EXTERNAL_STORAGE) && checkPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            val sdcard = File(Environment.getExternalStorageDirectory().absolutePath)
            if (sdcard.exists() && sdcard.isDirectory) {
                val list = sdcard.listFiles()
                if (list == null) {
                    Toast.makeText(applicationContext, "获取文件列表失败！", Toast.LENGTH_LONG).show()
                    return
                }
                val onSelected = Runnable {
                    val file: File? = adapterFileSelector!!.selectedFile
                    if (file != null) {
                        setResult(Activity.RESULT_OK, Intent().putExtra("file", file.absolutePath))
                        finish()
                    }
                }
                adapterFileSelector = if (mode == MODE_FOLDER) {
                    AdapterFileSelector.FolderChooser(sdcard, onSelected, ProgressBarDialog(this))
                } else {
                    AdapterFileSelector.FileChooser(sdcard, onSelected, ProgressBarDialog(this), extension)
                }
                fileSelectorList.adapter = adapterFileSelector
            }
        } else {
            requestPermissions()
        }
    }
}
