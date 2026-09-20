package wipe.cache.main

import android.app.Activity
import android.content.Intent
import android.os.Handler
import android.os.Looper
import android.widget.Toast
import wipe.cache.common.ui.ProgressBarDialog
import wipe.cache.krscript.model.PageNode

class OpenPageHelper(private var activity: Activity) {
    private var progressBarDialog: ProgressBarDialog? = null
    private var handler = Handler(Looper.getMainLooper())

    private val dialog: ProgressBarDialog
        get() {
            if (progressBarDialog == null) {
                progressBarDialog = ProgressBarDialog(activity)
            }
            return progressBarDialog!!
        }

    private fun showDialog(msg: String) {
        handler.post {
            dialog.showDialog(msg)
        }
    }

    private fun hideDialog() {
        handler.post {
            dialog.hideDialog()
        }
    }

    fun openPage(pageNode: PageNode) {
        try {
            var intent: Intent? = null
            if (!pageNode.onlineHtmlPage.isEmpty()) {
                intent = Intent(activity, ActionPageOnline::class.java)
                intent.putExtra("config", pageNode.onlineHtmlPage)
            }

            if (!pageNode.pageConfigSh.isEmpty()) {
                if (intent == null) {
                    intent = Intent(activity, ActionPage::class.java)
                }
            }

            if (!pageNode.pageConfigPath.isEmpty()) {
                if (intent == null) {
                    intent = Intent(activity, ActionPage::class.java)
                }
            }

            intent?.run {
                putExtra("page", pageNode)
                PageTransition.start(activity, this)
            }
        } catch (ex: Exception) {
            Toast.makeText(activity, "" + ex.message, Toast.LENGTH_SHORT).show()
        }
    }
}
