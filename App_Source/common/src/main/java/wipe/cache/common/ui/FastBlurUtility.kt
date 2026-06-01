package wipe.cache.common.ui

import android.app.Activity
import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.ColorMatrix
import android.graphics.ColorMatrixColorFilter
import android.graphics.Matrix
import android.graphics.Paint
import android.renderscript.Allocation
import android.renderscript.Element
import android.renderscript.RenderScript
import android.renderscript.ScriptIntrinsicBlur
import kotlin.math.abs
import kotlin.math.max
import kotlin.math.min

object FastBlurUtility {
    /**
     * 获得模糊化的背景图片
     * 
     * @param activity 获取模糊化的背景activity
     * @return 模糊化的背景图片
     */
    fun getBlurBackgroundDrawer(activity: Activity): Bitmap? {
        val bmp = takeScreenShot(activity)
        return startBlurBackground(bmp)
    }


    // 实测RenderScript做模糊性能反而更低...
    private fun blur(bitmap: Bitmap?, context: Context?): Bitmap? {
        if (bitmap == null) {
            return bitmap
        }

        //使用RenderScript对图片进行高斯模糊处理
        val output = Bitmap.createBitmap(bitmap) // 创建输出图片
        val rs = RenderScript.create(context) // 构建一个RenderScript对象
        val gaussianBlue = ScriptIntrinsicBlur.create(rs, Element.U8_4(rs)) //
        // 创建高斯模糊脚本
        val allIn = Allocation.createFromBitmap(rs, bitmap) // 开辟输入内存
        val allOut = Allocation.createFromBitmap(rs, output) // 开辟输出内存
        val radius = 10f //设置模糊半径
        gaussianBlue.setRadius(radius) // 设置模糊半径，范围0f<radius<=25f
        gaussianBlue.setInput(allIn) // 设置输入内存
        gaussianBlue.forEach(allOut) // 模糊编码，并将内存填入输出内存
        allOut.copyTo(output) // 将输出内存编码为Bitmap，图片大小必须注意
        rs.destroy()
        //rs.releaseAllContexts(); // 关闭RenderScript对象，API>=23则使用rs.releaseAllContexts()
        return output
    }

    /**
     * 截屏
     * 
     * @param activity 截屏的activity
     * @return 截屏图片
     */
    private fun takeScreenShot(activity: Activity): Bitmap? {
        val view = activity.getWindow().getDecorView()
        view.setDrawingCacheEnabled(true)
        view.buildDrawingCache()
        val b1 = view.getDrawingCache()

        if (b1 != null) {
            // 获取屏幕长和高
            val width = activity.getResources().getDisplayMetrics().widthPixels
            val height = activity.getResources().getDisplayMetrics().heightPixels

            try {
                val bmp = Bitmap.createBitmap(b1, 0, 0, width, height)
                view.destroyDrawingCache()
                return bmp
            } catch (ignored: Exception) {
            }
        }
        return null
    }

    // 使图片变暗
    fun getDimmedBitmap(bitmap: Bitmap?): Bitmap? {
        if (bitmap == null) {
            return null
        }

        val width = bitmap.getWidth()
        val height = bitmap.getHeight()
        val bmpGrayscale = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        val c = Canvas(bmpGrayscale)
        val paint = Paint()
        paint.setAntiAlias(true)
        val cm = ColorMatrix()
        val contrast = 0.95f
        cm.set(
            floatArrayOf(
                contrast, 0f, 0f, 0f, 0f,
                0f, contrast, 0f, 0f, 0f,
                0f, 0f, contrast, 0f, 0f,
                0f, 0f, 0f, 1f, 0f
            )
        )
        val f = ColorMatrixColorFilter(cm)
        paint.setColorFilter(f)
        c.drawBitmap(bitmap, 0f, 0f, paint)
        return bmpGrayscale
    }

    private fun startBlurBackground(bkg: Bitmap?): Bitmap? {
        if (bkg == null) {
            return null
        }
        val radius = 8f //模糊程度

        val overlay = fastBlur(small(bkg), radius.toInt())
        val dimmed = getDimmedBitmap(overlay)
        return if (dimmed != null) big(dimmed) else null
    }

    /**
     * 放大图片
     * 
     * @param bitmap 需要放大的图片
     * @return 放大的图片
     */
    private fun big(bitmap: Bitmap): Bitmap {
        val matrix = Matrix()
        matrix.postScale(4f, 4f)
        return Bitmap.createBitmap(
            bitmap,
            0,
            0,
            bitmap.getWidth(),
            bitmap.getHeight(),
            matrix,
            true
        )
    }

    /**
     * 缩小图片
     * 
     * @param bitmap 需要缩小的图片
     * @return 缩小的图片
     */
    private fun small(bitmap: Bitmap): Bitmap {
        val matrix = Matrix()
        matrix.postScale(0.10f, 0.10f)
        return Bitmap.createBitmap(
            bitmap,
            0,
            0,
            bitmap.getWidth(),
            bitmap.getHeight(),
            matrix,
            true
        )
    }

    /**
     * 将图片模糊化
     * 
     * @param sentBitmap 需要模糊的图片
     * @param radius     模糊程度
     * @return 模糊后的图片
     */
    private fun fastBlur(sentBitmap: Bitmap, radius: Int): Bitmap? {
        val bitmap = sentBitmap.copy(sentBitmap.getConfig()!!, true)

        if (radius < 1) {
            return (null)
        }

        val w = bitmap.getWidth()
        val h = bitmap.getHeight()

        val pix = IntArray(w * h)
        bitmap.getPixels(pix, 0, w, 0, 0, w, h)

        val wm = w - 1
        val hm = h - 1
        val wh = w * h
        val div = radius + radius + 1

        val r = IntArray(wh)
        val g = IntArray(wh)
        val b = IntArray(wh)
        var rsum: Int
        var gsum: Int
        var bsum: Int
        var x: Int
        var y: Int
        var i: Int
        var p: Int
        var yp: Int
        var yi: Int
        var yw: Int
        val vmin = IntArray(max(w, h))

        var divsum = (div + 1) shr 1
        divsum *= divsum
        val dv = IntArray(256 * divsum)
        i = 0
        while (i < 256 * divsum) {
            dv[i] = (i / divsum)
            i++
        }

        yi = 0
        yw = yi

        val stack = Array(div) { IntArray(3) }
        var stackpointer: Int
        var stackstart: Int
        var sir: IntArray
        var rbs: Int
        val r1 = radius + 1
        var routsum: Int
        var goutsum: Int
        var boutsum: Int
        var rinsum: Int
        var ginsum: Int
        var binsum: Int

        y = 0
        while (y < h) {
            bsum = 0
            gsum = bsum
            rsum = gsum
            boutsum = rsum
            goutsum = boutsum
            routsum = goutsum
            binsum = routsum
            ginsum = binsum
            rinsum = ginsum
            i = -radius
            while (i <= radius) {
                p = pix[yi + min(wm, max(i, 0))]
                sir = stack[i + radius]
                sir[0] = (p and 0xff0000) shr 16
                sir[1] = (p and 0x00ff00) shr 8
                sir[2] = (p and 0x0000ff)
                rbs = r1 - abs(i)
                rsum += sir[0] * rbs
                gsum += sir[1] * rbs        // cm.setSaturation(0);
                bsum += sir[2] * rbs
                if (i > 0) {
                    rinsum += sir[0]
                    ginsum += sir[1]
                    binsum += sir[2]
                } else {
                    routsum += sir[0]
                    goutsum += sir[1]
                    boutsum += sir[2]
                }
                i++
            }
            stackpointer = radius

            x = 0
            while (x < w) {
                r[yi] = dv[rsum]
                g[yi] = dv[gsum]
                b[yi] = dv[bsum]

                rsum -= routsum
                gsum -= goutsum
                bsum -= boutsum

                stackstart = stackpointer - radius + div
                sir = stack[stackstart % div]

                routsum -= sir[0]
                goutsum -= sir[1]
                boutsum -= sir[2]

                if (y == 0) {
                    vmin[x] = min(x + radius + 1, wm)
                }
                p = pix[yw + vmin[x]]

                sir[0] = (p and 0xff0000) shr 16
                sir[1] = (p and 0x00ff00) shr 8
                sir[2] = (p and 0x0000ff)

                rinsum += sir[0]
                ginsum += sir[1]
                binsum += sir[2]

                rsum += rinsum
                gsum += ginsum
                bsum += binsum

                stackpointer = (stackpointer + 1) % div
                sir = stack[(stackpointer) % div]

                routsum += sir[0]
                goutsum += sir[1]
                boutsum += sir[2]

                rinsum -= sir[0]
                ginsum -= sir[1]
                binsum -= sir[2]

                yi++
                x++
            }
            yw += w
            y++
        }
        x = 0
        while (x < w) {
            bsum = 0
            gsum = bsum
            rsum = gsum
            boutsum = rsum
            goutsum = boutsum
            routsum = goutsum
            binsum = routsum
            ginsum = binsum
            rinsum = ginsum
            yp = -radius * w
            i = -radius
            while (i <= radius) {
                yi = max(0, yp) + x

                sir = stack[i + radius]

                sir[0] = r[yi]
                sir[1] = g[yi]
                sir[2] = b[yi]

                rbs = r1 - abs(i)

                rsum += r[yi] * rbs
                gsum += g[yi] * rbs
                bsum += b[yi] * rbs

                if (i > 0) {
                    rinsum += sir[0]
                    ginsum += sir[1]
                    binsum += sir[2]
                } else {
                    routsum += sir[0]
                    goutsum += sir[1]
                    boutsum += sir[2]
                }

                if (i < hm) {
                    yp += w
                }
                i++
            }
            yi = x
            stackpointer = radius
            y = 0
            while (y < h) {
                pix[yi] =
                    (-0x1000000 and pix[yi]) or (dv[rsum] shl 16) or (dv[gsum] shl 8) or dv[bsum]

                rsum -= routsum
                gsum -= goutsum
                bsum -= boutsum

                stackstart = stackpointer - radius + div
                sir = stack[stackstart % div]

                routsum -= sir[0]
                goutsum -= sir[1]
                boutsum -= sir[2]

                if (x == 0) {
                    vmin[y] = min(y + r1, hm) * w
                }
                p = x + vmin[y]

                sir[0] = r[p]
                sir[1] = g[p]
                sir[2] = b[p]

                rinsum += sir[0]
                ginsum += sir[1]
                binsum += sir[2]

                rsum += rinsum
                gsum += ginsum
                bsum += binsum

                stackpointer = (stackpointer + 1) % div
                sir = stack[stackpointer]

                routsum += sir[0]
                goutsum += sir[1]
                boutsum += sir[2]

                rinsum -= sir[0]
                ginsum -= sir[1]
                binsum -= sir[2]

                yi += w
                y++
            }
            x++
        }

        bitmap.setPixels(pix, 0, w, 0, 0, w, h)
        return (bitmap)
    }
}
