package wipe.cache.krscript.model

import android.graphics.Color
import android.os.Handler
import android.os.Message
import android.text.Spannable
import android.text.SpannableString
import android.text.style.ForegroundColorSpan
import java.util.regex.Pattern

/**
 * Created by Hello on 2018/04/01.
 */
abstract class ShellHandlerBase : Handler() {
    protected abstract fun onProgress(current: Int, total: Int)

    protected abstract fun onStart(msg: Any?)

    abstract fun onStart(forceStop: Runnable?)

    protected abstract fun onExit(msg: Any?)

    /**
     * 输出格式化内容
     * 
     * @param msg
     */
    protected abstract fun updateLog(msg: SpannableString?)

    override fun handleMessage(msg: Message) {
        super.handleMessage(msg)
        when (msg.what) {
            EVENT_EXIT -> onExit(msg.obj)
            EVENT_START -> {
                onStart(msg.obj)
            }

            EVENT_REDE -> onReaderMsg(msg.obj)
            EVENT_READ_ERROR -> onError(msg.obj)
            EVENT_WRITE -> {
                onWrite(msg.obj)
            }
        }
    }

    protected fun onReaderMsg(msg: Any?) {
        if (msg != null) {
            val log = msg.toString().trim { it <= ' ' }
            if (Pattern.matches("^progress:\\[[\\-0-9\\\\]{1,}/[0-9\\\\]{1,}]$", log)) {
                val values: Array<String?> =
                    log.substring("progress:[".length, log.indexOf("]")).split("/".toRegex())
                        .dropLastWhile { it.isEmpty() }.toTypedArray()
                val start = values[0]!!.toInt()
                val total = values[1]!!.toInt()
                onProgress(start, total)
            } else {
                onReader(msg)
            }
        }
    }

    protected open fun onReader(msg: Any?) {
        updateLog(msg, "#00cc55")
    }

    protected open fun onWrite(msg: Any?) {
        updateLog(msg, "#808080")
    }

    protected open fun onError(msg: Any?) {
        updateLog(msg, "#ff0000")
    }

    /**
     * 输出指定颜色的内容
     * 
     * @param msg
     * @param color
     */
    protected fun updateLog(msg: Any?, color: String?) {
        if (msg != null) {
            val msgStr = msg.toString()
            val spannableString = SpannableString(msgStr)
            spannableString.setSpan(
                ForegroundColorSpan(Color.parseColor(color)),
                0,
                msgStr.length,
                Spannable.SPAN_EXCLUSIVE_EXCLUSIVE
            )
            updateLog(spannableString)
        }
    }

    protected fun updateLog(msg: Any?, color: Int) {
        if (msg != null) {
            val msgStr = msg.toString()
            val spannableString = SpannableString(msgStr)
            spannableString.setSpan(
                ForegroundColorSpan(color),
                0,
                msgStr.length,
                Spannable.SPAN_EXCLUSIVE_EXCLUSIVE
            )
            updateLog(spannableString)
        }
    }

    companion object {
        /**
         * 处理启动信息
         */
        const val EVENT_START: Int = 0

        /**
         * 命令行输出内容
         */
        const val EVENT_REDE: Int = 2

        /**
         * 命令行错误输出
         */
        const val EVENT_READ_ERROR: Int = 4

        /**
         * 脚本写入日志
         */
        const val EVENT_WRITE: Int = 6

        /**
         * 处理Exitvalue
         */
        val EVENT_EXIT: Int = -2
    }
}
