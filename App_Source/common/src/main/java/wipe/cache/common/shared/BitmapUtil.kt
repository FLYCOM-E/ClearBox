package wipe.cache.common.shared

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Canvas
import android.graphics.Matrix
import android.graphics.PixelFormat
import android.graphics.drawable.BitmapDrawable
import android.graphics.drawable.Drawable
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.InputStream
import java.io.OutputStream

class BitmapUtil {
    var temp: Bitmap? = null

    /**
     * 根据指定的高度进行缩放（source是bitmap）
     */
    fun bitmapZoomByHeight(srcBitmap: Bitmap, newHeight: Float): Bitmap {
        val scale = newHeight / ((srcBitmap.getHeight().toFloat()))
        return bitmapZoomByScale(srcBitmap, scale, scale)
    }

    /**
     * 根据指定的高度进行缩放（source是drawable）
     */
    fun bitmapZoomByHeight(drawable: Drawable, newHeight: Float): Bitmap {
        val bitmap = drawableToBitmap(drawable)
        val scale = newHeight / ((bitmap.getHeight().toFloat()))
        return bitmapZoomByScale(bitmap, scale, scale)
    }

    /**
     * 根据指定的宽度比例值和高度比例值进行缩放
     */
    fun bitmapZoomByScale(srcBitmap: Bitmap, scaleWidth: Float, scaleHeight: Float): Bitmap {
        val width = srcBitmap.getWidth()
        val height = srcBitmap.getHeight()
        val matrix = Matrix()
        matrix.postScale(scaleWidth, scaleHeight)
        val bitmap = Bitmap.createBitmap(srcBitmap, 0, 0, width, height, matrix, true)
        if (bitmap != null) {
            return bitmap
        } else {
            return srcBitmap
        }
    }

    /**
     * 将drawable对象转成bitmap对象
     */
    fun drawableToBitmap(drawable: Drawable): Bitmap {
        val width = drawable.getIntrinsicWidth()
        val height = drawable.getIntrinsicHeight()
        val config =
            if (drawable.getOpacity() != PixelFormat.OPAQUE) Bitmap.Config.ARGB_8888 else Bitmap.Config.RGB_565
        val bitmap = Bitmap.createBitmap(width, height, config)
        val canvas = Canvas(bitmap)
        drawable.setBounds(0, 0, width, height)
        drawable.draw(canvas)
        return bitmap
    }

    /**
     * 将drawable对象转成bitmap对象
     */
    fun drawableToBitmap2(drawable: Drawable?): Bitmap? {
        val bd = drawable as BitmapDrawable
        val bm = bd.getBitmap()
        return bm
    }

    /**
     * 将bitmap对象保存成图片到sd卡中
     */
    fun saveBitmapToSDCard(bitmap: Bitmap, path: String) {
        val file = File(path)
        if (file.exists()) {
            file.delete()
        }
        try {
            val fileOutputStream = FileOutputStream(file)
            bitmap.compress(
                Bitmap.CompressFormat.PNG,
                100,
                (fileOutputStream as OutputStream)
            ) //设置PNG的话，透明区域不会变成黑色

            fileOutputStream.close()
            println("----------save success-------------------")
        } catch (v0: Exception) {
            v0.printStackTrace()
        }
    }

    /**
     * 从sd卡中获取图片的bitmap对象
     */
    fun getBitmapFromSDCard(path: String?): Bitmap? {
        var bitmap: Bitmap? = null
        try {
            val fileInputStream = FileInputStream(path)
            if (fileInputStream != null) {
                val options = BitmapFactory.Options()
                options.inSampleSize = 2 //当图片资源太大的适合，会出现内存溢出。图片宽高都为原来的二分之一，即图片为原来的四分一
                bitmap = BitmapFactory.decodeStream((fileInputStream as InputStream), null, options)
            }
        } catch (e: Exception) {
            return null
        }

        return bitmap
    }
}