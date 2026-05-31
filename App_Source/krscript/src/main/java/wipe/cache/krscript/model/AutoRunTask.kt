package wipe.cache.krscript.model

interface AutoRunTask {
    fun onCompleted(result: Boolean?)
    val key: String?
}
