package wipe.cache.krscript.executor

import android.content.Context
import android.os.Build
import android.os.Environment
import wipe.cache.common.shared.FileWrite.getPrivateFileDir
import wipe.cache.common.shared.FileWrite.getPrivateFilePath
import wipe.cache.common.shared.FileWrite.writePrivateFile
import wipe.cache.common.shared.FileWrite.writePrivateShellFile
import wipe.cache.common.shell.KeepShell
import wipe.cache.common.shell.KeepShellPublic.checkRoot
import wipe.cache.common.shell.KeepShellPublic.getDefaultInstance
import wipe.cache.common.shell.ShellTranslation
import wipe.cache.krscript.FileOwner
import wipe.cache.krscript.model.NodeInfoBase
import java.io.DataOutputStream
import java.io.File
import java.nio.charset.Charset
import java.security.MessageDigest
import java.security.NoSuchAlgorithmException

object ScriptEnvironmen {
    private const val ASSETS_FILE = "file:///android_asset/"
    var isInited: Boolean = false
        private set
    private var environmentPath = ""

    // 此目录将添加到PATH尾部，作为应用程序提供的拓展程序库目录，如有需要则需要在初始化executor.sh之前为该变量赋值
    private var TOOKIT_DIR: String? = ""
    private var rooted = false
    private var privateShell: KeepShell? = null
    private var shellTranslation: ShellTranslation? = null

    private fun init(context: Context): Boolean {
        val configSpf = context.getSharedPreferences("kr-script-config", Context.MODE_PRIVATE)

        return ScriptEnvironmen.init(
            context,
            configSpf.getString("executor", "kr-script/executor.sh")!!,
            configSpf.getString("toolkitDir", "kr-script/toolkit")
        )
    }

    /**
     * 初始化执行器
     * 
     * @param context  Context
     * @param executor 执行器在Assets中的位置
     * @return 是否初始化成功
     */
    fun init(context: Context, executor: String, toolkitDir: String?): Boolean {
        if (isInited) {
            return true
        }

        shellTranslation = ShellTranslation(context.getApplicationContext())
        rooted = checkRoot()

        try {
            if (toolkitDir != null && !toolkitDir.isEmpty()) {
                TOOKIT_DIR = ExtractAssets(context).extractResources(toolkitDir)
            }

            var fileName = executor
            if (fileName.startsWith(ASSETS_FILE)) {
                fileName = fileName.substring(ASSETS_FILE.length)
            }

            val inputStream = context.getAssets().open(fileName)
            val bytes = ByteArray(inputStream.available())
            val length = inputStream.read(bytes, 0, bytes.size).toLong()
            var envShell = String(bytes, Charset.defaultCharset()).replace("\r".toRegex(), "")

            val environment = getEnvironment(context)
            for (key in environment.keys) {
                var value = environment.get(key)
                if (value == null) {
                    value = ""
                }
                envShell = envShell.replace("$({" + key + "})", value)
            }
            val outputPathAbs = getPrivateFilePath(context, fileName)
            envShell = envShell.replace("$({EXECUTOR_PATH})", outputPathAbs)


            isInited =
                writePrivateFile(envShell.toByteArray(Charset.defaultCharset()), fileName, context)
            if (isInited) {
                environmentPath = outputPathAbs
            }

            val configSpf =
                context.getSharedPreferences("kr-script-config", Context.MODE_PRIVATE).edit()
            configSpf.putString("executor", executor)
            configSpf.putString("toolkitDir", toolkitDir)
            configSpf.apply()

            privateShell = if (rooted) getDefaultInstance() else KeepShell(rooted)

            return isInited
        } catch (ex: Exception) {
            return false
        }
    }

    private fun md5(string: String): String {
        if (string.isEmpty()) {
            return ""
        }

        var md5: MessageDigest? = null
        try {
            md5 = MessageDigest.getInstance("MD5")
            val bytes = md5.digest(string.toByteArray())
            val result = StringBuilder()
            for (b in bytes) {
                var temp = Integer.toHexString(b.toInt() and 0xff)
                if (temp.length == 1) {
                    temp = "0" + temp
                }
                result.append(temp)
            }
            return result.toString()
        } catch (e: NoSuchAlgorithmException) {
            e.printStackTrace()
        }

        return ""
    }

    /**
     * 写入缓存（脚本代码存入脚本文件）
     * 
     * @param context
     * @param script
     * @return
     */
    private fun createShellCache(context: Context, script: String): String {
        val md5 = md5(script)
        val outputPath = "kr-script/cache/" + md5 + ".sh"
        if (File(outputPath).exists()) {
            return outputPath
        }

        val bytes = ("#!/system/bin/sh\n\n" + script)
            .replace("\r\n".toRegex(), "\n")
            .replace("\r\t".toRegex(), "\t")
            .replace("\r".toRegex(), "\n")
            .toByteArray()
        if (writePrivateFile(bytes, outputPath, context)) {
            return getPrivateFilePath(context, outputPath)
        }
        return ""
    }

    /**
     * 执行脚本
     * 
     * @param context
     * @param fileName
     * @return
     */
    private fun extractScript(context: Context, fileName: String): String? {
        var fileName = fileName
        if (fileName.startsWith(ASSETS_FILE)) {
            fileName = fileName.substring(ASSETS_FILE.length)
        }
        return writePrivateShellFile(fileName, fileName, context)
    }

    @JvmStatic
    fun executeResultRoot(context: Context, script: String?, nodeInfoBase: NodeInfoBase?): String {
        if (!isInited) {
            init(context)
        }

        if (script == null || script.isEmpty()) {
            return ""
        }

        val script2 = script.trim { it <= ' ' }
        var path: String? = ""
        if (script2.startsWith(ASSETS_FILE)) {
            path = extractScript(context, script2)
        } else {
            path = createShellCache(context, script)
        }

        if (!isInited) {
            init(context)
        }

        val stringBuilder = StringBuilder()

        stringBuilder.append("\n")
        if (nodeInfoBase != null && !nodeInfoBase.currentPageConfigPath.isEmpty()) {
            val parentPageConfigDir = nodeInfoBase.pageConfigDir
            val currentPageConfigPath = nodeInfoBase.currentPageConfigPath
            stringBuilder.append("export PAGE_CONFIG_DIR='").append(parentPageConfigDir)
                .append("'\n")
            stringBuilder.append("export PAGE_CONFIG_FILE='").append(currentPageConfigPath)
                .append("'\n")

            if (currentPageConfigPath.startsWith("file:///android_asset/")) {
                stringBuilder.append("export PAGE_WORK_DIR='")
                    .append(ExtractAssets(context).getExtractPath(parentPageConfigDir))
                    .append("'\n")
                stringBuilder.append("export PAGE_WORK_FILE='")
                    .append(ExtractAssets(context).getExtractPath(currentPageConfigPath))
                    .append("'\n")
            } else {
                stringBuilder.append("export PAGE_WORK_DIR='").append(parentPageConfigDir)
                    .append("'\n")
                stringBuilder.append("export PAGE_WORK_FILE='").append(currentPageConfigPath)
                    .append("'\n")
            }
        } else {
            stringBuilder.append("export PAGE_CONFIG_DIR=''\n")
            stringBuilder.append("export PAGE_CONFIG_FILE=''\n")
            stringBuilder.append("export PAGE_WORK_DIR=''\n")
            stringBuilder.append("export PAGE_WORK_DIR=''\n")
        }

        stringBuilder.append("\n\n")
        stringBuilder.append(environmentPath + " \"" + path + "\"")
        if (shellTranslation != null) {
            return shellTranslation!!.resolveRow(
                privateShell!!.doCmdSync(stringBuilder.toString())
            )
        } else {
            return privateShell!!.doCmdSync(stringBuilder.toString())
        }
    }

    private fun getStartPath(context: Context): String {
        val dir = getPrivateFileDir(context)
        if (dir.endsWith("/")) {
            return dir.substring(0, dir.length - 1)
        }
        return dir
    }

    /**
     * 获取框架的环境变量
     * 
     * @param context
     * @return
     */
    private fun getEnvironment(context: Context): HashMap<String?, String?> {
        val params = HashMap<String?, String?>()

        params.put("TOOLKIT", TOOKIT_DIR)
        params.put("START_DIR", getStartPath(context))
        // params.put("EXECUTOR_PATH", environmentPath);
        params.put("TEMP_DIR", context.getCacheDir().getAbsolutePath())

        val fileOwner = FileOwner(context)
        val androidUid = fileOwner.getUserId()
        params.put("ANDROID_UID", "" + androidUid)

        try {
            // @ https://blog.csdn.net/Gaugamela/article/details/78689580
            params.put("APP_USER_ID", fileOwner.getFileOwner())
            // params.put("APP_UID", "" + android.os.Process.myPid());
            // params.put("APP_PID", "" + android.os.Process.myPid());
            // params.put("APP_TID", "" + android.os.Process.myTid());
        } catch (ignored: Exception) {
        }

        params.put("ANDROID_SDK", "" + Build.VERSION.SDK_INT)
        // params.put("ROOT_PERMISSION", rooted ? "granted" : "denied");
        params.put("ROOT_PERMISSION", if (rooted) "true" else "false")
        params.put("SDCARD_PATH", Environment.getExternalStorageDirectory().getAbsolutePath())
        val busyboxPath = getPrivateFilePath(context, "busybox")
        if (File(getPrivateFilePath(context, "busybox")).exists()) {
            params.put("BUSYBOX", busyboxPath)
        } else {
            params.put("BUSYBOX", "busybox")
        }
        try {
            val packageInfo =
                context.getPackageManager().getPackageInfo(context.getPackageName(), 0)
            params.put("PACKAGE_NAME", context.getPackageName())
            params.put("PACKAGE_VERSION_NAME", packageInfo.versionName)
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                params.put("PACKAGE_VERSION_CODE", "" + packageInfo.getLongVersionCode())
            } else {
                params.put("PACKAGE_VERSION_CODE", "" + packageInfo.versionCode)
            }
        } catch (ex: Exception) {
        }

        return params
    }

    /**
     * @param params
     * @return
     */
    private fun getVariables(params: HashMap<String?, String?>?): ArrayList<String?> {
        val envp = ArrayList<String?>()

        if (params != null) {
            for (key in params.keys) {
                var value = params.get(key)
                if (value == null) {
                    value = ""
                }
                envp.add(key + "='" + value.replace("'".toRegex(), "'\\\\''") + "'")
            }
        }

        return envp
    }

    private fun getExecuteScript(context: Context, script: String?, tag: String?): String {
        if (!isInited) {
            init(context)
        }

        if (script == null || script.isEmpty()) {
            return ""
        }

        val script2 = script.trim { it <= ' ' }
        var cachePath: String? = ""
        if (script2.startsWith(ASSETS_FILE)) {
            cachePath = extractScript(context, script2)
            if (cachePath == null) {
                cachePath = script
                // String error = context.getString(R.string.script_losted) + setState;
                // Toast.makeText(context, error, Toast.LENGTH_LONG).show();
            }
        } else {
            cachePath = createShellCache(context, script)
        }


        return environmentPath + " \"" + cachePath + "\" \"" + tag + "\""
    }

    @JvmStatic
    val runtime: Process?
        get() {
            try {
                if (rooted) {
                    return Runtime.getRuntime().exec("su")
                } else {
                    return Runtime.getRuntime().exec("sh")
                }
            } catch (ex: Exception) {
                return null
            }
        }

    /**
     * 使用执行器运行脚本
     * 
     * @param context          Context
     * @param dataOutputStream Runtime进程的输出流
     * @param cmds             要执行的脚本
     * @param params           参数类别
     */
    @JvmStatic
    fun executeShell(
        context: Context,
        dataOutputStream: DataOutputStream,
        cmds: String?,
        params: HashMap<String?, String?>?,
        nodeInfo: NodeInfoBase?,
        tag: String?
    ) {
        var params = params
        if (params == null) {
            params = HashMap<String?, String?>()
        }

        // 页面配置文件路径
        if (nodeInfo != null) {
            val parentPageConfigDir = nodeInfo.pageConfigDir
            val currentPageConfigPath = nodeInfo.currentPageConfigPath
            params.put("PAGE_CONFIG_DIR", parentPageConfigDir)
            params.put("PAGE_CONFIG_FILE", currentPageConfigPath)
            if (currentPageConfigPath.startsWith("file:///android_asset/")) {
                params.put(
                    "PAGE_WORK_DIR",
                    ExtractAssets(context).getExtractPath(parentPageConfigDir)
                )
                params.put(
                    "PAGE_WORK_FILE",
                    ExtractAssets(context).getExtractPath(currentPageConfigPath)
                )
            } else {
                params.put("PAGE_WORK_DIR", parentPageConfigDir)
                params.put("PAGE_WORK_FILE", currentPageConfigPath)
            }
        } else {
            params.put("PAGE_CONFIG_DIR", "")
            params.put("PAGE_CONFIG_FILE", "")
            params.put("PAGE_WORK_DIR", "")
            params.put("PAGE_WORK_FILE", "")
        }

        val envp = getVariables(params)
        val envpCmds = StringBuilder()
        if (envp.size > 0) {
            for (param in envp) {
                envpCmds.append("export ").append(param).append("\n")
            }
        }
        try {
            dataOutputStream.write(envpCmds.toString().toByteArray(charset("UTF-8")))

            dataOutputStream.write(
                getExecuteScript(
                    context,
                    cmds,
                    tag
                ).toByteArray(charset("UTF-8"))
            )

            dataOutputStream.writeBytes("\n\n")
            dataOutputStream.writeBytes("sleep 0.2;\n")
            dataOutputStream.writeBytes("exit\n")
            dataOutputStream.writeBytes("exit\n")
            dataOutputStream.flush()
        } catch (ignored: Exception) {
        }
    }
}

