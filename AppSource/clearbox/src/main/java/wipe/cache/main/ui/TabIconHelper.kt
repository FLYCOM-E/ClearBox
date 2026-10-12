package wipe.cache.main.ui

import android.app.Activity
import android.graphics.drawable.Drawable
import android.view.View
import android.widget.ImageView
import android.widget.TabHost
import android.widget.TextView
import androidx.core.content.ContextCompat
import wipe.cache.main.R

class TabIconHelper(private var tabHost: TabHost, private var activity: Activity) {
    private var views = ArrayList<View>()

    fun newTabSpec(drawable: Drawable, content: Int): String {
        return newTabSpec("", drawable, content)
    }

    fun newTabSpec(text: String, drawable: Drawable, content: Int): String {
        val layout = View.inflate(activity, R.layout.list_item_tab, null)
        val imageView = layout.findViewById<ImageView>(R.id.ItemIcon)
        val textView = layout.findViewById<TextView>(R.id.ItemTitle)
        val tabId = "tab_" + views.size

        textView.text = text

        if (views.size != 0) {
            layout.alpha = 0.3f
        }
        imageView.setImageDrawable(drawable)
        views.add(layout)
        tabHost.addTab(tabHost.newTabSpec(tabId).setContent(content).setIndicator(layout))

        return tabId
    }
}
