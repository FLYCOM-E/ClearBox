package wipe.cache.main

import android.app.Activity
import android.content.Intent
import android.os.Build

// AI

object PageTransition {
    fun start(activity: Activity, intent: Intent) {
        intent.flags = intent.flags and Intent.FLAG_ACTIVITY_NEW_TASK.inv()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            activity.overrideActivityTransition(
                Activity.OVERRIDE_TRANSITION_OPEN,
                R.anim.activity_open_enter,
                R.anim.activity_open_exit
            )
            activity.startActivity(intent)
        } else {
            activity.startActivity(intent)
            @Suppress("DEPRECATION")
            activity.overridePendingTransition(
                R.anim.activity_open_enter,
                R.anim.activity_open_exit
            )
        }
    }

    fun prepareOpen(activity: Activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            activity.overrideActivityTransition(
                Activity.OVERRIDE_TRANSITION_OPEN,
                R.anim.activity_open_enter,
                R.anim.activity_open_exit
            )
        }
    }

    fun finish(activity: Activity, superFinish: () -> Unit) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
            activity.overrideActivityTransition(
                Activity.OVERRIDE_TRANSITION_CLOSE,
                R.anim.activity_close_enter,
                R.anim.activity_close_exit
            )
            superFinish()
        } else {
            superFinish()
            @Suppress("DEPRECATION")
            activity.overridePendingTransition(
                R.anim.activity_close_enter,
                R.anim.activity_close_exit
            )
        }
    }
}
