package wipe.cache.common.model

class SelectItem {
    var title: String? = null
    var value: String? = null
    var selected: Boolean = false

    override fun toString(): String {
        return if (!title.isNullOrEmpty()) {
            title!!
        } else if (!value.isNullOrEmpty()) {
            value!!
        } else {
            ""
        }
    }
}
