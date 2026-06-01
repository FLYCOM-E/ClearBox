package wipe.cache.krscript

import android.annotation.SuppressLint
import android.app.Activity
import android.content.Context
import android.webkit.JavascriptInterface
import android.webkit.ValueCallback
import android.webkit.WebView
import android.widget.Toast
import org.json.JSONObject
import wipe.cache.common.shell.KeepShellPublic.checkRoot
import wipe.cache.common.shell.ShellExecutor.getSuperUserRuntime
import wipe.cache.krscript.executor.ExtractAssets
import wipe.cache.krscript.executor.ScriptEnvironmen.executeResultRoot
import wipe.cache.krscript.executor.ScriptEnvironmen.executeShell
import wipe.cache.krscript.model.NodeInfoBase
import wipe.cache.krscript.model.ShellHandlerBase
import wipe.cache.krscript.ui.ParamsFileChooserRender.FileChooserInterface
import wipe.cache.krscript.ui.ParamsFileChooserRender.FileSelectedInterface
import wipe.cache.krscript.ui.ParamsFileChooserRender.FileSelectedInterface.Companion.TYPE_FILE
import java.io.BufferedReader
import java.io.DataOutputStream
import java.io.IOException
import java.io.InputStreamReader

class WebViewInjector @SuppressLint("SetJavaScriptEnabled") constructor(
    webView: WebView,
    fileChooser: FileChooserInterface?
) {
    private val webView: WebView?
    private val context: Context
    private val fileChooser: FileChooserInterface?

    init {
        this.webView = webView
        this.context = webView.getContext()
        this.fileChooser = fileChooser
    }

    @SuppressLint("JavascriptInterface", "SetJavaScriptEnabled")
    fun inject(activity: Activity?, credible: Boolean) {
        if (webView != null) {
            val webSettings = webView.getSettings()
            webSettings.setJavaScriptEnabled(true)
            webSettings.setAllowFileAccess(credible)
            webSettings.setAllowUniversalAccessFromFileURLs(credible)
            webSettings.setAllowFileAccessFromFileURLs(credible)
            webSettings.setAllowContentAccess(true)
            webSettings.setUseWideViewPort(true)

            webView.addJavascriptInterface(
                KrScriptEngine(context),
                "KrScriptCore" // 由于类名会被混淆，写死吧... KrScriptEngine.class.getSimpleName()
            )
        }
    }

    private inner class KrScriptEngine(private val context: Context) {
        private val virtualRootNode = NodeInfoBase("")

        /**
         * 检查是否具有ROOT权限
         * 
         * @return
         */
        @JavascriptInterface
        fun rootCheck(): Boolean {
            return checkRoot()
        }

        /**
         * 同步执行shell脚本 并返回结果（不包含错误信息）
         * 
         * @param script 脚本内容
         * @return 执行过程中的输出内容
         */
        @JavascriptInterface
        fun executeShell(script: String?): String {
            if (script != null && !script.isEmpty()) {
                return executeResultRoot(context, script, virtualRootNode)
            }
            return ""
        }

        /**
         * @param script
         * @param callbackFunction
         */
        @JavascriptInterface
        fun executeShellAsync(script: String?, callbackFunction: String?, env: String?): Boolean {
            val params = HashMap<String?, String?>()
            var process: Process? = null
            try {
                if (env != null && !env.isEmpty()) {
                    val paramsObject = JSONObject(env)
                    val it = paramsObject.keys()
                    while (it.hasNext()) {
                        val key = it.next()
                        params.put(key, paramsObject.getString(key))
                    }
                }
                process = getSuperUserRuntime()
            } catch (ex: Exception) {
                Toast.makeText(context, ex.message, Toast.LENGTH_SHORT).show()
            }

            if (process != null) {
                val outputStream = process.getOutputStream()
                val dataOutputStream = DataOutputStream(outputStream)

                setHandler(process, callbackFunction, object : Runnable {
                    override fun run() {
                    }
                })

                executeShell(context, dataOutputStream, script, params, null, null)
                return true
            } else {
                return false
            }
        }

        @JavascriptInterface
        fun fileChooser(callbackFunction: String?): Boolean {
            if (fileChooser != null) {
                return fileChooser.openFileChooser(object : FileSelectedInterface {
                    override fun type(): Int {
                        return TYPE_FILE
                    }

                    override fun suffix(): String? {
                        return null
                    }

                    override fun mimeType(): String {
                        return "*/*"
                    }

                    override fun onFileSelected(path: String?) {
                        try {
                            val message = JSONObject()
                            if (path == null || path.isEmpty()) {
                                message.put("absPath", null)
                            } else {
                                message.put("absPath", path)
                            }
                            webView!!.post(object : Runnable {
                                override fun run() {
                                    webView.evaluateJavascript(
                                        callbackFunction + "(" + message.toString() + ")",
                                        object : ValueCallback<String?> {
                                            override fun onReceiveValue(value: String?) {
                                            }
                                        })
                                }
                            })
                        } catch (ex: Exception) {
                        }
                    }
                })
            }
            return false
        }

        fun setHandler(process: Process, callbackFunction: String?, onExit: Runnable?) {
            val inputStream = process.getInputStream()
            val errorStream = process.getErrorStream()
            val reader = Thread(object : Runnable {
                override fun run() {
                    var line: String?
                    try {
                        val bufferedReader = BufferedReader(InputStreamReader(inputStream, "UTF-8"))
                        while ((bufferedReader.readLine().also { line = it }) != null) {
                            try {
                                val message = JSONObject()
                                message.put("type", ShellHandlerBase.EVENT_REDE)
                                message.put("message", line + "\n")
                                webView!!.post(object : Runnable {
                                    override fun run() {
                                        webView.evaluateJavascript(
                                            callbackFunction + "(" + message.toString() + ")",
                                            object : ValueCallback<String?> {
                                                override fun onReceiveValue(value: String?) {
                                                }
                                            })
                                    }
                                })
                            } catch (ex: Exception) {
                            }
                        }
                    } catch (e: IOException) {
                        e.printStackTrace()
                    }
                }
            })
            val readerError = Thread(object : Runnable {
                override fun run() {
                    var line: String?
                    try {
                        val bufferedReader = BufferedReader(InputStreamReader(errorStream, "UTF-8"))
                        while ((bufferedReader.readLine().also { line = it }) != null) {
                            try {
                                val message = JSONObject()
                                message.put("type", ShellHandlerBase.EVENT_READ_ERROR)
                                message.put("message", line + "\n")
                                webView!!.post(object : Runnable {
                                    override fun run() {
                                        webView.evaluateJavascript(
                                            callbackFunction + "(" + message.toString() + ")",
                                            object : ValueCallback<String?> {
                                                override fun onReceiveValue(value: String?) {
                                                }
                                            })
                                    }
                                })
                            } catch (ex: Exception) {
                            }
                        }
                    } catch (e: IOException) {
                        e.printStackTrace()
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
                        try {
                            val message = JSONObject()
                            message.put("type", ShellHandlerBase.EVENT_EXIT)
                            message.put("message", "" + status)
                            webView!!.post(object : Runnable {
                                override fun run() {
                                    webView.evaluateJavascript(
                                        callbackFunction + "(" + message.toString() + ")",
                                        object : ValueCallback<String?> {
                                            override fun onReceiveValue(value: String?) {
                                            }
                                        })
                                }
                            })
                        } catch (ex: Exception) {
                        }

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
}
