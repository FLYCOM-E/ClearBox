package wipe.cache.krscript

import android.annotation.SuppressLint
import android.webkit.WebView

class WebViewInjector @SuppressLint("SetJavaScriptEnabled") constructor(
    private val webView: WebView
) {
    @SuppressLint("JavascriptInterface", "SetJavaScriptEnabled")
    fun inject(credible: Boolean) {
        val webSettings = webView.settings
        webSettings.javaScriptEnabled = true
        webSettings.allowFileAccess = credible
        webSettings.setAllowUniversalAccessFromFileURLs(credible)
        webSettings.setAllowFileAccessFromFileURLs(credible)
        webSettings.allowContentAccess = true
        webSettings.useWideViewPort = true

        webView.addJavascriptInterface(
            KrScriptEngine(),
            "KrScriptCore" // 由于类名会被混淆，写死吧... KrScriptEngine.class.getSimpleName()
        )
    }

    private class KrScriptEngine
}
