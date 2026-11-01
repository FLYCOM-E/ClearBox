package wipe.cache.main

import android.Manifest
import android.app.Activity
import android.app.UiModeManager
import android.app.WallpaperManager
import android.content.Context
import android.os.Build
import android.util.Log
import android.view.WindowManager
import androidx.annotation.RequiresPermission
import androidx.core.content.PermissionChecker
import androidx.core.view.WindowInsetsControllerCompat
import com.google.android.material.color.DynamicColors
import wipe.cache.common.ui.ThemeMode

object ThemeModeState {
    private var themeMode: ThemeMode = ThemeMode()
    
    private fun checkPermission(context: Context, permission: String): Boolean = 
        PermissionChecker.checkSelfPermission(context, permission) == PermissionChecker.PERMISSION_GRANTED

    @RequiresPermission(anyOf = ["android.permission.READ_WALLPAPER_INTERNAL", Manifest.permission.MANAGE_EXTERNAL_STORAGE])
    fun switchTheme(activity: Activity? = null): ThemeMode {
        if (activity != null) {
            val uiModeManager = activity.applicationContext.getSystemService(Context.UI_MODE_SERVICE) as UiModeManager
            val nightMode = (uiModeManager.nightMode == UiModeManager.MODE_NIGHT_YES)

            val useWallpaper = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                ThemeConfig(activity).getAllowTransparentUI()
            } else {
                ThemeConfig(activity).getAllowTransparentUI() &&
                        checkPermission(activity, Manifest.permission.READ_EXTERNAL_STORAGE)
            }
            
            if (useWallpaper) {
                if (nightMode) {
                    activity.setTheme(R.style.AppThemeWallpaper)
                } else {
                    activity.setTheme(R.style.AppThemeWallpaperLight)
                }
            } else {
                activity.setTheme(R.style.Theme_KrScript)
            }
            DynamicColors.applyToActivityIfAvailable(activity)
            
            themeMode.isDarkMode = nightMode

            if (useWallpaper) {
                applyWallpaperBackground(activity, nightMode)
            } else {
                applySystemBars(activity, nightMode)
            }
        }
        return themeMode
    }

    @RequiresPermission(anyOf = ["android.permission.READ_WALLPAPER_INTERNAL", Manifest.permission.MANAGE_EXTERNAL_STORAGE])
    private fun applyWallpaperBackground(activity: Activity, nightMode: Boolean) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                activity.window.addFlags(WindowManager.LayoutParams.FLAG_SHOW_WALLPAPER)
                activity.window.setBackgroundDrawable(null)
                Log.d("Wallpaper", "Android 13+: FLAG_SHOW_WALLPAPER")
            } else {
                val wallpaper = WallpaperManager.getInstance(activity)
                val wallpaperInfo = wallpaper.wallpaperInfo

                if (wallpaperInfo?.packageName != null) {
                    activity.window.addFlags(WindowManager.LayoutParams.FLAG_SHOW_WALLPAPER)
                    activity.window.setBackgroundDrawable(null)
                } else {
                    val wallpaperDrawable = wallpaper.drawable
                    if (wallpaperDrawable != null) {
                        activity.window.setBackgroundDrawable(wallpaperDrawable)
                    } else {
                        activity.window.addFlags(WindowManager.LayoutParams.FLAG_SHOW_WALLPAPER)
                        activity.window.setBackgroundDrawable(null)
                    }
                }
            }
            
            activity.window.run {
                clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
                addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
            
                val controller = WindowInsetsControllerCompat(this, decorView)
                controller.isAppearanceLightStatusBars = !nightMode
                controller.isAppearanceLightNavigationBars = !nightMode
            }
        } catch (e: Exception) {
            applySystemBars(activity, nightMode)
        }
    }
    
    private fun applySystemBars(activity: Activity, nightMode: Boolean) {
        activity.window.run {
            clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
            addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
            
            val controller = WindowInsetsControllerCompat(this, decorView)
            controller.isAppearanceLightStatusBars = !nightMode
            controller.isAppearanceLightNavigationBars = !nightMode
        }
    }

    fun getThemeMode(): ThemeMode = themeMode
}
