package wipe.cache.main

import android.app.Activity
import android.content.Intent
import android.widget.Toast
import wipe.cache.krscript.model.PageNode

class OpenPageHelper(private var activity: Activity) {
    fun openPage(pageNode: PageNode) {
        try {
            var intent: Intent? = null
            if (pageNode.onlineHtmlPage.isNotEmpty()) {
                intent = Intent(activity, ActionPageOnline::class.java)
                intent.putExtra("config", pageNode.onlineHtmlPage)
            }

            if (pageNode.pageConfigSh.isNotEmpty()) {
                if (intent == null) {
                    intent = Intent(activity, ActionPage::class.java)
                }
            }

            if (pageNode.pageConfigPath.isNotEmpty()) {
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
