package wipe.cache.common.ui

import android.view.View
import androidx.dynamicanimation.animation.DynamicAnimation
import androidx.dynamicanimation.animation.SpringAnimation
import androidx.dynamicanimation.animation.SpringForce

object SpringUtils {

    private const val STIFFNESS_MEDIUM = 400f
    private const val STIFFNESS_LOW = 200f
    private const val DAMPING_RATIO_MEDIUM = 0.7f
    private const val DAMPING_RATIO_LOW = 0.5f

    fun addPressSpring(view: View) {
        addPressSpring(view, STIFFNESS_MEDIUM, DAMPING_RATIO_MEDIUM)
    }

    fun addPressSpring(view: View, stiffness: Float, dampingRatio: Float) {
        val scaleX = SpringAnimation(view, DynamicAnimation.SCALE_X, 1f).apply {
            spring = SpringForce(1f).apply {
                this.stiffness = stiffness
                this.dampingRatio = dampingRatio
            }
        }
        val scaleY = SpringAnimation(view, DynamicAnimation.SCALE_Y, 1f).apply {
            spring = SpringForce(1f).apply {
                this.stiffness = stiffness
                this.dampingRatio = dampingRatio
            }
        }

        view.setOnTouchListener { v, event ->
            when (event.action) {
                android.view.MotionEvent.ACTION_DOWN -> {
                    scaleX.animateToFinalPosition(0.92f)
                    scaleY.animateToFinalPosition(0.92f)
                }
                android.view.MotionEvent.ACTION_UP, android.view.MotionEvent.ACTION_CANCEL -> {
                    scaleX.animateToFinalPosition(1f)
                    scaleY.animateToFinalPosition(1f)
                }
            }
            false
        }
    }

    fun addClickSpring(view: View) {
        addClickSpring(view, STIFFNESS_MEDIUM, DAMPING_RATIO_MEDIUM)
    }

    fun addClickSpring(view: View, stiffness: Float, dampingRatio: Float) {
        val scaleX = SpringAnimation(view, DynamicAnimation.SCALE_X, 1f).apply {
            spring = SpringForce(1f).apply {
                this.stiffness = stiffness
                this.dampingRatio = dampingRatio
            }
        }
        val scaleY = SpringAnimation(view, DynamicAnimation.SCALE_Y, 1f).apply {
            spring = SpringForce(1f).apply {
                this.stiffness = stiffness
                this.dampingRatio = dampingRatio
            }
        }

        view.setOnClickListener { v ->
            scaleX.animateToFinalPosition(0.88f)
            scaleY.animateToFinalPosition(0.88f)
            view.postOnAnimationDelayed({
                scaleX.animateToFinalPosition(1f)
                scaleY.animateToFinalPosition(1f)
            }, 50)
            view.performClick()
        }
    }

    fun springBounceY(view: View, from: Float, to: Float) {
        SpringAnimation(view, DynamicAnimation.TRANSLATION_Y, to).apply {
            spring = SpringForce(to).apply {
                stiffness = STIFFNESS_MEDIUM
                dampingRatio = DAMPING_RATIO_MEDIUM
            }
            setStartVelocity(from - to)
            start()
        }
    }

    fun springEntrance(view: View) {
        view.alpha = 0f
        view.translationY = 80f

        SpringAnimation(view, DynamicAnimation.TRANSLATION_Y, 0f).apply {
            spring = SpringForce(0f).apply {
                stiffness = STIFFNESS_LOW
                dampingRatio = DAMPING_RATIO_LOW
            }
            start()
        }

        SpringAnimation(view, DynamicAnimation.ALPHA, 1f).apply {
            spring = SpringForce(1f).apply {
                stiffness = STIFFNESS_LOW
                dampingRatio = 1f
            }
            start()
        }
    }

    fun springScaleEntrance(view: View, delay: Long = 0) {
        view.scaleX = 0f
        view.scaleY = 0f

        view.postDelayed({
            SpringAnimation(view, DynamicAnimation.SCALE_X, 1f).apply {
                spring = SpringForce(1f).apply {
                    stiffness = STIFFNESS_MEDIUM
                    dampingRatio = DAMPING_RATIO_LOW
                }
                start()
            }
            SpringAnimation(view, DynamicAnimation.SCALE_Y, 1f).apply {
                spring = SpringForce(1f).apply {
                    stiffness = STIFFNESS_MEDIUM
                    dampingRatio = DAMPING_RATIO_LOW
                }
                start()
            }
        }, delay)
    }
}
