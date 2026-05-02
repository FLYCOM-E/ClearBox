package com.projectkr.shell

import android.Manifest
import android.app.Activity
import android.app.UiModeManager
import android.app.WallpaperManager
import android.content.Context
import android.graphics.Color
import android.graphics.drawable.BitmapDrawable
import android.graphics.drawable.Drawable
import android.os.Build
import android.view.View
import android.view.WindowManager
import androidx.core.content.PermissionChecker
import com.omarea.common.ui.ThemeMode
import com.google.android.material.color.MaterialColors

object ThemeModeState {
    private var themeMode: ThemeMode = ThemeMode()
    
    private fun checkPermission(context: Context, permission: String): Boolean = 
        PermissionChecker.checkSelfPermission(context, permission) == PermissionChecker.PERMISSION_GRANTED

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
                if (nightMode) {
                    activity.setTheme(R.style.AppThemeDark)
                } else {
                    activity.setTheme(R.style.AppTheme)
                }
            }
            
            themeMode.isDarkMode = nightMode

            if (useWallpaper) {
                applyWallpaperBackground(activity, nightMode)
            } else {
                applySystemBars(activity, nightMode)
            }
        }
        return themeMode
    }

    private fun applyWallpaperBackground(activity: Activity, nightMode: Boolean) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                activity.window.addFlags(WindowManager.LayoutParams.FLAG_SHOW_WALLPAPER)
                activity.window.setBackgroundDrawable(null)
                android.util.Log.d("Wallpaper", "Android 13+: FLAG_SHOW_WALLPAPER")
            } else {
                val wallpaper = WallpaperManager.getInstance(activity)
                val wallpaperInfo = wallpaper.wallpaperInfo

                if (wallpaperInfo != null && wallpaperInfo.packageName != null) {
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

                if (nightMode) {
                    statusBarColor = Color.argb(128, 0, 0, 0)
                    navigationBarColor = Color.argb(128, 0, 0, 0)
                } else {
                    statusBarColor = Color.argb(128, 255, 255, 255)
                    navigationBarColor = Color.argb(128, 255, 255, 255)
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                        decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or
                                View.SYSTEM_UI_FLAG_LAYOUT_STABLE or
                                View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR or
                                View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR
                    } else {
                        decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or
                                View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
                    }
                }
            }

        } catch (e: Exception) {
            applySystemBars(activity, nightMode)
        }
    }
    
    private fun applySystemBars(activity: Activity, nightMode: Boolean) {
        val colorSurface = MaterialColors.getColor(activity, com.google.android.material.R.attr.colorSurface, Color.WHITE)
        
        activity.window.run {
            clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
            addFlags(WindowManager.LayoutParams.FLAG_DRAWS_System_BAR_BACKGROUNDS)
            decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
    
            statusBarColor = colorSurface
            navigationBarColor = colorSurface
    
            if (!nightMode) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    decorView.systemUiVisibility = decorView.systemUiVisibility or
                            View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR or
                            View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR
                } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    decorView.systemUiVisibility = decorView.systemUiVisibility or
                            View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
                }
            }
        }
    }
    
    @Suppress("unused")
    private fun isDarkColor(wallPaper: Drawable): Boolean {
        val bitmap = (wallPaper as BitmapDrawable).bitmap
        val h = bitmap.height - 1
        val w = bitmap.width - 1

        var darkPoint = 0
        var lightPoint = 0
        val pointCount = if (h > 24 && w > 24) 24 else 1

        for (i in 0..pointCount) {
            val y = h / pointCount * i
            val x = w / pointCount * i
            val pixel = bitmap.getPixel(x, y)

            val redValue = Color.red(pixel)
            val blueValue = Color.blue(pixel)
            val greenValue = Color.green(pixel)

            if (redValue > 150 && blueValue > 150 && greenValue > 150) {
                lightPoint += 1
            } else {
                darkPoint += 1
            }
        }
        return darkPoint > lightPoint
    }

    fun getThemeMode(): ThemeMode = themeMode
}
