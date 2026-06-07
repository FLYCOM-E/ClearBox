package wipe.cache.krscript

import android.content.Context
import android.os.Process
import android.os.UserManager

class FileOwner(private val context: Context) {
    val userId: Int
        get() {
            val um =
                context.getSystemService(Context.USER_SERVICE) as UserManager
            val userHandle = Process.myUserHandle()

            var value = 0
            try {
                value = um.getSerialNumberForUser(userHandle).toInt()
            } catch (ignored: Exception) {
            }
            return value
        }

    val fileOwner: String
        get() {
            val androidUid = this.userId
            return "u" + androidUid + "_a" + ((Process.myUid() % 100000) - Process.FIRST_APPLICATION_UID) // 100000 => UserHandle.PER_USER_RANGE
        }
}
