package wipe.cache.main.ui

import android.os.Handler
import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import android.widget.TextView
import android.widget.Toast
import com.google.android.material.snackbar.Snackbar
import wipe.cache.common.ui.DialogHelper.Companion.confirm
import wipe.cache.common.ui.ProgressBarDialog
import wipe.cache.main.R
import java.io.File
import java.io.FileFilter
import java.util.Locale

class AdapterFileSelector private constructor(
    rootDir: File,
    fileSelected: Runnable,
    progressBarDialog: ProgressBarDialog,
    extension: String?
) : BaseAdapter() {
    private var fileArray: Array<File?>? = null
    private var fileSelected: Runnable? = null
    private var currentDir: File? = null
    var selectedFile: File? = null
        private set
    private val handler = Handler()
    private var progressBarDialog: ProgressBarDialog? = null
    private var extension: String? = null
    private var hasParent = false // 是否还有父级
    private var rootDir = "/" // 根目录
    private val leaveRootDir = true // 是否允许离开设定的rootDir到更父级的目录去
    private var folderChooserMode = false // 是否是目录选择模式（目录选择模式下不显示文件，长按目录选中）

    init {
        init(rootDir, fileSelected, progressBarDialog, extension)
    }

    private fun init(
        rootDir: File,
        fileSelected: Runnable,
        progressBarDialog: ProgressBarDialog,
        extension: String?
    ) {
        this.rootDir = rootDir.getAbsolutePath()
        this.fileSelected = fileSelected
        this.progressBarDialog = progressBarDialog
        if (extension != null) {
            if (extension.startsWith(".")) {
                this.extension = extension
            } else {
                this.extension = "." + extension
            }
        }
        loadDir(rootDir)
    }

    private fun loadDir(dir: File) {
        progressBarDialog!!.showDialog("Loading...")
        Thread(object : Runnable {
            override fun run() {
                val parent = dir.getParentFile()
                if (parent != null) {
                    val parentPath = parent.getAbsolutePath()
                    hasParent =
                        parent.exists() && parent.canRead() && (leaveRootDir || !(rootDir.startsWith(
                            parentPath
                        ) && rootDir.length > parentPath.length))
                } else {
                    hasParent = false
                }

                if (dir.exists() && dir.canRead()) {
                    val files = dir.listFiles(object : FileFilter {
                        override fun accept(fileItem: File): Boolean {
                            if (folderChooserMode) {
                                return fileItem.isDirectory()
                            } else {
                                return fileItem.exists() && (!fileItem.isFile() || extension == null || extension!!.isEmpty() || fileItem.getName()
                                    .endsWith(extension!!))
                            }
                        }
                    })

                    // 文件排序
                    for (i in files!!.indices) {
                        for (j in i + 1..<files.size) {
                            if ((files[j]!!.isDirectory() && files[i]!!.isFile())) {
                                val t = files[i]
                                files[i] = files[j]
                                files[j] = t
                            } else if (files[j]!!.isDirectory() == files[i]!!.isDirectory() && (files[j]!!.getName()
                                    .lowercase(
                                        Locale.getDefault()
                                    ).compareTo(
                                        files[i]!!.getName().lowercase(
                                            Locale.getDefault()
                                        )
                                    ) < 0)
                            ) {
                                val t = files[i]
                                files[i] = files[j]
                                files[j] = t
                            }
                        }
                    }
                    fileArray = files
                }
                currentDir = dir
                handler.post(object : Runnable {
                    override fun run() {
                        notifyDataSetChanged()
                        progressBarDialog!!.hideDialog()
                    }
                })
            }
        }).start()
    }

    fun goParent(): Boolean {
        if (hasParent) {
            loadDir(File(currentDir!!.getParent()))
            return true
        }
        return false
    }

    override fun getCount(): Int {
        if (hasParent) {
            if (fileArray == null) {
                return 1
            }
            return fileArray!!.size + 1
        } else {
            if (fileArray == null) {
                return 0
            }
            return fileArray!!.size
        }
    }

    fun refresh() {
        if (this.currentDir != null) {
            this.loadDir(currentDir!!)
        }
    }

    override fun getItem(position: Int): Any? {
        if (hasParent) {
            if (position == 0) {
                return File(currentDir!!.getParent())
            } else {
                return fileArray!![position - 1]
            }
        } else {
            return fileArray!![position]
        }
    }

    override fun getItemId(position: Int): Long {
        return 0
    }

    override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
        val view: View
        if (hasParent && position == 0) {
            view = View.inflate(parent.getContext(), R.layout.list_item_dir, null)
            ((view.findViewById<View?>(R.id.ItemTitle)) as TextView).setText("...")
            view.setOnClickListener(object : View.OnClickListener {
                override fun onClick(v: View?) {
                    goParent()
                }
            })
            return view
        } else {
            val file = getItem(position) as File
            if (file.isDirectory()) {
                view = View.inflate(parent.getContext(), R.layout.list_item_dir, null)
                view.setOnClickListener(object : View.OnClickListener {
                    override fun onClick(v: View?) {
                        if (!file.exists()) {
                            Toast.makeText(
                                view.getContext(),
                                "File is deleted, please repick",
                                Toast.LENGTH_SHORT
                            ).show()
                            return
                        }
                        val files = file.listFiles()
                        if (files != null && files.size > 0) {
                            loadDir(file)
                        } else {
                            Snackbar.make(view, R.string.not_file_in_dir, Snackbar.LENGTH_SHORT)
                                .show()
                        }
                    }
                })
                if (folderChooserMode) {
                    view.setOnLongClickListener(object : View.OnLongClickListener {
                        override fun onLongClick(v: View?): Boolean {
                            confirm(
                                view.getContext(),
                                view.getContext().getString(R.string.pick_dir),
                                file.getAbsolutePath(),
                                object : Runnable {
                                    override fun run() {
                                        if (!file.exists()) {
                                            Toast.makeText(
                                                view.getContext(),
                                                "Dir is deleted, please repick",
                                                Toast.LENGTH_SHORT
                                            ).show()
                                            return
                                        }
                                        selectedFile = file
                                        fileSelected!!.run()
                                    }
                                },
                                object : Runnable {
                                    override fun run() {
                                    }
                                })
                            return true
                        }
                    })
                }
            } else {
                view = View.inflate(parent.getContext(), R.layout.list_item_file, null)
                val fileLength = file.length()
                val fileSize: String?
                if (fileLength < 1024) {
                    fileSize = fileLength.toString() + "B"
                } else if (fileLength < 1048576) {
                    fileSize =
                        String.format("%sKB", String.format("%.2f", (file.length() / 1024.0)))
                } else if (fileLength < 1073741824) {
                    fileSize =
                        String.format("%sMB", String.format("%.2f", (file.length() / 1048576.0)))
                } else {
                    fileSize =
                        String.format("%sGB", String.format("%.2f", (file.length() / 1073741824.0)))
                }

                ((view.findViewById<View?>(R.id.ItemText)) as TextView).setText(fileSize)

                view.setOnClickListener(object : View.OnClickListener {
                    override fun onClick(v: View?) {
                        confirm(
                            view.getContext(),
                            "选定文件？",
                            file.getAbsolutePath(),
                            object : Runnable {
                                override fun run() {
                                    if (!file.exists()) {
                                        Toast.makeText(
                                            view.getContext(),
                                            "所选的文件已被删除，请重新选择！",
                                            Toast.LENGTH_SHORT
                                        ).show()
                                        return
                                    }
                                    selectedFile = file
                                    fileSelected!!.run()
                                }
                            },
                            object : Runnable {
                                override fun run() {
                                }
                            })
                    }
                })
            }
            ((view.findViewById<View?>(R.id.ItemTitle)) as TextView).setText(file.getName())
            return view
        }
    }

    companion object {
        fun FolderChooser(
            rootDir: File,
            fileSelected: Runnable,
            progressBarDialog: ProgressBarDialog
        ): AdapterFileSelector {
            val adapterFileSelector =
                AdapterFileSelector(rootDir, fileSelected, progressBarDialog, null)
            adapterFileSelector.folderChooserMode = true
            return adapterFileSelector
        }

        fun FileChooser(
            rootDir: File,
            fileSelected: Runnable,
            progressBarDialog: ProgressBarDialog,
            extension: String?
        ): AdapterFileSelector {
            val adapterFileSelector =
                AdapterFileSelector(rootDir, fileSelected, progressBarDialog, extension)
            adapterFileSelector.folderChooserMode = false
            return adapterFileSelector
        }
    }
}
