package wipe.cache.common.ui

import android.annotation.SuppressLint
import android.app.Activity
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.view.LayoutInflater
import android.widget.TextView
import android.widget.Toast
import wipe.cache.common.R
import wipe.cache.common.shell.AsyncSuShellUnit

open class ProgressBarDialog(private var context: Activity, private var uniqueId: String? = null) {
    private var alert: DialogHelper.DialogWrap? = null
    private var textView: TextView? = null

    companion object {
        private val dialogs = LinkedHashMap<String, DialogHelper.DialogWrap>()
    }

    init {
        hideDialog()
    }

    class DefaultHandler(private var alertDialog: DialogHelper.DialogWrap?) : Handler(Looper.myLooper()!!) {
        override fun handleMessage(msg: Message) {
            super.handleMessage(msg)

            try {
                if (alertDialog == null) {
                    return
                }
                if (msg.what == 10) {
                    alertDialog!!.dismiss()
                    alertDialog!!.hide()
                    if (msg.obj == true) {
                        Toast.makeText(alertDialog!!.context, R.string.execute_success, Toast.LENGTH_SHORT).show()
                    } else {
                        Toast.makeText(alertDialog!!.context, R.string.execute_fail, Toast.LENGTH_LONG).show()
                    }
                } else if (msg.what == -1) {
                    Toast.makeText(alertDialog!!.context, R.string.execute_fail, Toast.LENGTH_LONG).show()
                } else if (msg.what == 0 && msg.obj == false) {
                    alertDialog!!.dismiss()
                    alertDialog!!.hide()
                    Toast.makeText(alertDialog!!.context, R.string.execute_fail, Toast.LENGTH_LONG).show()
                }
            } catch (_: Exception) {
            }
        }
    }

    @SuppressLint("InflateParams")
    fun execShell(cmd: String, handler: Handler? = null) {
        hideDialog()

        val layoutInflater = LayoutInflater.from(context)
        val dialog = layoutInflater.inflate(R.layout.dialog_loading, null)
        val textView: TextView = (dialog.findViewById(R.id.dialog_text))
        textView.text = context.getString(R.string.execute_wait)
        alert = DialogHelper.loadingDialog(context, dialog)
        if (handler == null) {
            AsyncSuShellUnit(DefaultHandler(alert)).exec(cmd).waitFor()
        } else {
            AsyncSuShellUnit(handler).exec(cmd).waitFor()
        }
    }

    fun execShell(sb: StringBuilder, handler: Handler? = null) {
        execShell(sb.toString(), handler)
    }

    fun hideDialog() {
        try {
            if (alert != null) {
                alert!!.dismiss()
                alert!!.hide()
                alert = null
            }
        } catch (_: Exception) {
        }
        textView = null

        uniqueId?.run {
            if (dialogs.containsKey(this)) {
                dialogs.remove(this)
            }
        }
    }

    @SuppressLint("InflateParams")
    fun showDialog(text: String = "..."): ProgressBarDialog {
        if (textView != null && alert != null) {
            textView!!.text = text
        } else {
            hideDialog()
            val layoutInflater = LayoutInflater.from(context)
            val dialog = layoutInflater.inflate(R.layout.dialog_loading, null)
            textView = (dialog.findViewById(R.id.dialog_text)!!)
            textView!!.text = text
            alert = DialogHelper.loadingDialog(context, dialog)
        }

        uniqueId?.run {
            if (dialogs.containsKey(this)) {
                dialogs.remove(this)
            }
            if (alert != null) {
                dialogs[this] = alert!!
            }
        }

        return this
    }
}
