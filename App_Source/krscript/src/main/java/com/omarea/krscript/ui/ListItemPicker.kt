package wipe.cache.module.ui

import android.content.Context
import android.view.View
import android.widget.ImageView
import wipe.cache.module.R
import wipe.cache.module.model.PickerNode

class ListItemPicker(context: Context, config: PickerNode) : ListItemClickable(context, R.layout.kr_action_list_item, config) {
    private val widgetView = layout.findViewById<ImageView?>(R.id.kr_widget)

    init {
        widgetView?.visibility = View.VISIBLE
        widgetView?.setImageDrawable(context.getDrawable(R.drawable.function))
    }
}
