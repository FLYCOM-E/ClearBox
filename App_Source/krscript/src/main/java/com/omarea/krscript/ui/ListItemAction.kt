package wipe.cache.module.ui

import android.content.Context
import android.view.View
import android.widget.ImageView
import com.omarea.common.R as CommonR
import wipe.cache.module.model.ActionNode

class ListItemAction(context: Context, config: ActionNode) : ListItemClickable(context, R.layout.kr_action_list_item, config) {
    private val widgetView = layout.findViewById<ImageView?>(R.id.kr_widget)

    init {
        widgetView?.visibility = View.VISIBLE
        if (config.params != null && config.params!!.size > 0) {
            widgetView?.setImageDrawable(context.getDrawable(CommonR.drawable.menu))
        } else {
            widgetView?.setImageDrawable(context.getDrawable(CommonR.drawable.run))
        }
    }
}
