package wipe.cache.common.shell

import java.io.IOException

object ShellExecutor {
    private var extraEnvPath: String = ""
    private var defaultEnvPath: String = ""

    @JvmStatic
    fun setExtraEnvPath(extraEnvPath: String) {
        this.extraEnvPath = extraEnvPath
    }

    private val envPath: String?
        get() {
            // FIXME:非root模式下，默认的 TMPDIR=/data/local/tmp 变量可能会导致某些需要写缓存的场景（例如使用source指令）脚本执行失败！
            if (extraEnvPath.isNotEmpty()) {
                if (defaultEnvPath.isEmpty()) {
                    try {
                        val process = Runtime.getRuntime().exec("sh")
                        val outputStream = process.outputStream
                        outputStream.write("echo \$PATH".toByteArray())
                        outputStream.flush()
                        outputStream.close()

                        val inputStream = process.inputStream
                        val cache = ByteArray(16384)
                        val length = inputStream.read(cache)
                        inputStream.close()
                        process.destroy()

                        val path = String(cache, 0, length).trim()
                        if (path.isNotEmpty()) {
                            defaultEnvPath = path
                        } else {
                            throw RuntimeException("Get \$PATH args error")
                        }
                    } catch (ex: Exception) {
                        defaultEnvPath = "/sbin:/system/sbin:/system/bin:/system/xbin:/odm/bin:/vendor/bin:/vendor/xbin"
                    }
                }

                return "PATH=${defaultEnvPath}:$extraEnvPath"
            }

            return null
        }

    @Throws(IOException::class)
    private fun getProcess(run: String): Process {
        val env = envPath
        val process = Runtime.getRuntime().exec(run)
        if (env != null) {
            val outputStream = process.outputStream
            outputStream.write("export ".toByteArray())
            outputStream.write(env.toByteArray())
            outputStream.write("\n".toByteArray())
            outputStream.flush()
        }
        return process
    }

    @JvmStatic
    @Throws(IOException::class)
    fun getSuperUserRuntime(): Process {
        return getProcess("su")
    }

    @Throws(IOException::class)
    fun getRuntime(): Process {
        return getProcess("sh")
    }
}