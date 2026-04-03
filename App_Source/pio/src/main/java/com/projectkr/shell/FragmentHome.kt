package com.projectkr.shell

import android.annotation.SuppressLint
import android.app.ActivityManager
import android.content.Context.ACTIVITY_SERVICE
import android.content.SharedPreferences
import android.os.Bundle
import android.os.Handler
import com.google.android.material.snackbar.Snackbar
import androidx.fragment.app.Fragment
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.GridView
import android.widget.TextView
import android.widget.Toast
import com.omarea.common.shell.KeepShellPublic
import com.projectkr.shell.ui.AdapterCpuCores
import com.projectkr.shell.ui.CpuChartView
import com.projectkr.shell.ui.RamChatView
import com.projectkr.shell.utils.CpuFrequencyUtils
import com.projectkr.shell.utils.GpuUtils
import java.math.BigDecimal
import java.math.RoundingMode
import java.util.*
import kotlin.collections.HashMap


class FragmentHome : androidx.fragment.app.Fragment() {
    private lateinit var homeClearRam: View
    private lateinit var homeClearSwap: View
    private lateinit var homeRaminfoText: TextView
    private lateinit var homeZramsizeText: TextView
    private lateinit var homeRaminfo: RamChatView
    private lateinit var homeSwapstateChat: RamChatView
    private lateinit var cpuCoreCount: TextView
    private lateinit var homeGpuFreq: TextView
    private lateinit var homeGpuLoad: TextView
    private lateinit var homeGpuChat: View
    private lateinit var cpuCoreTotalLoad: TextView
    private lateinit var homeCpuChat: View
    private lateinit var cpuCoreList: GridView

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,
                              savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.fragment_home, container, false)
    }

    private lateinit var globalSPF: SharedPreferences
    private var timer: Timer? = null
    private fun showMsg(msg: String) {
        this.view?.let { Snackbar.make(it, msg, Snackbar.LENGTH_LONG).show() }
    }

    private lateinit var spf: SharedPreferences
    private var myHandler = Handler()

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        homeClearRam = view.findViewById(R.id.home_clear_ram)
        homeClearSwap = view.findViewById(R.id.home_clear_swap)
        homeRaminfoText = view.findViewById(R.id.home_raminfo_text)
        homeZramsizeText = view.findViewById(R.id.home_zramsize_text)
        homeRaminfo = view.findViewById(R.id.home_raminfo)
        homeSwapstateChat = view.findViewById(R.id.home_swapstate_chat)
        cpuCoreCount = view.findViewById(R.id.cpu_core_count)
        homeGpuFreq = view.findViewById(R.id.home_gpu_freq)
        homeGpuLoad = view.findViewById(R.id.home_gpu_load)
        homeGpuChat = view.findViewById(R.id.home_gpu_chat)
        cpuCoreTotalLoad = view.findViewById(R.id.cpu_core_total_load)
        homeCpuChat = view.findViewById(R.id.home_cpu_chat)
        cpuCoreList = view.findViewById(R.id.cpu_core_list)

        homeClearRam.setOnClickListener {
            homeRaminfoText.text = getString(R.string.please_wait)
            Thread(Runnable {
                KeepShellPublic.doCmdSync("sync\n" + "echo 3 > /proc/sys/vm/drop_caches\n" + "echo 1 > /proc/sys/vm/compact_memory")
                myHandler.postDelayed({
                    try {
                        updateRamInfo()
                        Toast.makeText(context, getString(R.string.monitor_cache_cleared), Toast.LENGTH_SHORT).show()
                    } catch (ex: java.lang.Exception) {
                    }
                }, 600)
            }).start()
        }
        homeClearSwap.setOnClickListener {
            homeZramsizeText.text = getString(R.string.please_wait)
            Thread(Runnable {
                KeepShellPublic.doCmdSync("sync\n" + "echo 1 > /proc/sys/vm/compact_memory")
                myHandler.postDelayed({
                    try {
                        updateRamInfo()
                        Toast.makeText(context, getString(R.string.monitor_ram_cleared), Toast.LENGTH_SHORT).show()
                    } catch (ex: java.lang.Exception) {
                    }
                }, 600)
            }).start()
        }
    }

    @SuppressLint("SetTextI18n")
    override fun onResume() {
        super.onResume()
        if (isDetached) return
        maxFreqs.clear()
        minFreqs.clear()
        stopTimer()
        timer = Timer()
        timer!!.schedule(object : TimerTask() {
            override fun run() { updateInfo() }
        }, 0, 1000)
        updateRamInfo()
    }

    private var coreCount = -1
    private var activityManager: ActivityManager? = null
    private var minFreqs = HashMap<Int, String>()
    private var maxFreqs = HashMap<Int, String>()

    fun format1(value: Double): String {
        var bd = BigDecimal(value)
        bd = bd.setScale(1, RoundingMode.HALF_UP)
        return bd.toString()
    }

    @SuppressLint("SetTextI18n")
    private fun updateRamInfo() {
        try {
            val info = ActivityManager.MemoryInfo()
            if (activityManager == null) {
                activityManager = context!!.getSystemService(ACTIVITY_SERVICE) as ActivityManager
            }
            activityManager!!.getMemoryInfo(info)
            val totalMem = (info.totalMem / 1024 / 1024f).toInt()
            val availMem = (info.availMem / 1024 / 1024f).toInt()
            homeRaminfoText.text = "${((totalMem - availMem) * 100 / totalMem)}% (${totalMem / 1024 + 1}GB)"
            homeRaminfo.setData(totalMem.toFloat(), availMem.toFloat())
            val swapInfo = KeepShellPublic.doCmdSync("free -m | grep Swap")
            if (swapInfo.contains("Swap")) {
                try {
                    val swapInfos = swapInfo.substring(swapInfo.indexOf(" "), swapInfo.lastIndexOf(" ")).trim()
                    if (Regex("[\\d]{1,}[\\s]{1,}[\\d]{1,}").matches(swapInfos)) {
                        val total = swapInfos.substring(0, swapInfos.indexOf(" ")).trim().toInt()
                        val use = swapInfos.substring(swapInfos.indexOf(" ")).trim().toInt()
                        val free = total - use
                        homeSwapstateChat.setData(total.toFloat(), free.toFloat())
                        if (total > 99) {
                            homeZramsizeText.text = "${(use * 100.0 / total).toInt()}% (${format1(total / 1024.0)}GB)"
                        } else {
                            homeZramsizeText.text = "${(use * 100.0 / total).toInt()}% (${total}MB)"
                        }
                    }
                } catch (ex: java.lang.Exception) {
                }
            }
        } catch (ex: Exception) {
        }
    }

    private var updateTick = 0

    @SuppressLint("SetTextI18n")
    private fun updateInfo() {
        if (coreCount < 1) {
            coreCount = CpuFrequencyUtils.getCoreCount()
        }
        val cores = ArrayList<CpuCoreInfo>()
        val loads = CpuFrequencyUtils.getCpuLoad()
        for (coreIndex in 0 until coreCount) {
            val core = CpuCoreInfo()
            core.currentFreq = CpuFrequencyUtils.getCurrentFrequency("cpu$coreIndex")
            if (!maxFreqs.containsKey(coreIndex) || (core.currentFreq != "" && maxFreqs[coreIndex].isNullOrEmpty())) {
                maxFreqs[coreIndex] = CpuFrequencyUtils.getCurrentMaxFrequency("cpu$coreIndex")
            }
            core.maxFreq = maxFreqs[coreIndex]
            if (!minFreqs.containsKey(coreIndex) || (core.currentFreq != "" && minFreqs[coreIndex].isNullOrEmpty())) {
                minFreqs[coreIndex] = CpuFrequencyUtils.getCurrentMinFrequency("cpu$coreIndex")
            }
            core.minFreq = minFreqs[coreIndex]
            if (loads.containsKey(coreIndex)) {
                core.loadRatio = loads[coreIndex]!!
            }
            cores.add(core)
        }
        val gpuFreq = GpuUtils.getGpuFreq() + "Mhz"
        val gpuLoad = GpuUtils.getGpuLoad()
        myHandler.post {
            try {
                cpuCoreCount.text = String.format(getString(R.string.monitor_core_count), coreCount)
                homeGpuFreq.text = gpuFreq
                homeGpuLoad.text = String.format(getString(R.string.monitor_laod), gpuLoad)
                if (gpuLoad > -1) {
                    (homeGpuChat as? com.projectkr.shell.ui.RamChatView)?.setData(100.toFloat(), (100 - gpuLoad).toFloat())
                }
                if (loads.containsKey(-1)) {
                    val totalLoad = loads[-1]!!.toInt()
                    cpuCoreTotalLoad.text = String.format(getString(R.string.monitor_laod), totalLoad)
                    (homeCpuChat as? com.projectkr.shell.ui.RamChatView)?.setData(100.toFloat(), (100 - totalLoad).toFloat())
                }
                if (cpuCoreList.adapter == null) {
                    if (cores.size < 6) cpuCoreList.numColumns = 2
                    cpuCoreList.adapter = AdapterCpuCores(context!!, cores)
                } else {
                    (cpuCoreList.adapter as AdapterCpuCores).setData(cores)
                }
            } catch (ex: Exception) {
                Log.e("Exception", ex.message ?: "")
            }
        }
        updateTick++
        if (updateTick > 5) {
            updateTick = 0
            minFreqs.clear()
            maxFreqs.clear()
        }
    }

    private fun stopTimer() {
        if (this.timer != null) {
            timer!!.cancel()
            timer = null
        }
    }

    override fun onPause() {
        stopTimer()
        super.onPause()
    }
}
