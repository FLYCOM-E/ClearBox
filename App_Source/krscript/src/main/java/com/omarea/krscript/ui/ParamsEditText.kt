package wipe.cache.module.ui

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.widget.EditText
import wipe.cache.module.R
import wipe.cache.module.model.ActionParamInfo
import wipe.cache.module.model.ParamInfoFilter

class ParamsEditText(private var actionParamInfo: ActionParamInfo, private var context: Context) {
    fun render(): View {
        val layout = LayoutInflater.from(context).inflate(R.layout.kr_param_text, null)


        layout.findViewById<EditText>(R.id.kr_param_text).run {
            tag = actionParamInfo.name
            if (actionParamInfo.valueFromShell != null)
                setText(actionParamInfo.valueFromShell)
            else if (actionParamInfo.value != null) {
                setText(actionParamInfo.value)
            }
            filters = arrayOf(ParamInfoFilter(actionParamInfo))
            isEnabled = !actionParamInfo.readonly
            if (actionParamInfo.placeholder.isNotEmpty()) {
                hint = actionParamInfo.placeholder
            } else if (
                    (actionParamInfo.type == "int" || actionParamInfo.type == "number")
                    &&
                    (actionParamInfo.min != Int.MIN_VALUE || actionParamInfo.max != Int.MAX_VALUE)
            ) {
                hint = "${actionParamInfo.min} ~ ${actionParamInfo.max}"
            }
        }

        return layout
    }
}
