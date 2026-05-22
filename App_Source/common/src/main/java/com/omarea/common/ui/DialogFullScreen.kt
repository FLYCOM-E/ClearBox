package com.omarea.common.ui

import android.app.Dialog
import android.os.Build
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import com.omarea.common.R

open class DialogFullScreen(private val layout: Int, private val darkMode: Boolean) : androidx.fragment.app.DialogFragment() {
    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        currentView = inflater.inflate(layout, container)
        return currentView
    }
    
    private var themeResId: Int = 0
    private lateinit var currentView: View
    
    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            return Dialog(activity!!, if (themeResId != 0) themeResId else R.style.dialog_full_screen_light)
        } else {
            return Dialog(activity!!, -1)
        }
    }
    
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val activity = this.activity
        if (activity != null) {
            dialog?.window?.run {
                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
                    setWindowAnimations(android.R.style.Animation_Translucent)
                }

                DialogHelper.setWindowBlurBg(this, activity)
            }
        }
    }
    
    public fun closeView() {
        try {
            dismiss()
        } catch (ex: java.lang.Exception) {
        }
    }
    
    init {
        themeResId = if (darkMode) R.style.dialog_full_screen_dark else R.style.dialog_full_screen_light
    }
}
