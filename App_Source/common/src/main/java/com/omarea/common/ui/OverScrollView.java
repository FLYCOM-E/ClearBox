package com.omarea.common.ui;

import android.annotation.TargetApi;
import android.content.Context;
import android.os.Build;
import android.util.AttributeSet;
import android.widget.ScrollView;

public class OverScrollView extends ScrollView {
    public OverScrollView(Context context) {
        super(context);
    }
    
    public OverScrollView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }
    
    public OverScrollView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }
    
    public OverScrollView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }
}
