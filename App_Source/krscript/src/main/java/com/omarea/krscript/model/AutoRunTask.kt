package wipe.cache.module.model

interface AutoRunTask {
    fun onCompleted(result: Boolean?)
    val key: String?
}
