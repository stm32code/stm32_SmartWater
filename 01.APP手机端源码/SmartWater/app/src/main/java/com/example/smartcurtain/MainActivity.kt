package com.example.smartcurtain

import android.annotation.SuppressLint
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.Gravity
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.ArrayAdapter
import android.widget.SeekBar
import com.blankj.utilcode.util.LogUtils
import com.example.Smartwater.R
import com.example.Smartwater.databinding.ActivityMainBinding

import com.example.smartcurtain.bean.DataDTO
import com.example.smartcurtain.bean.Receive
import com.example.smartcurtain.bean.Send
import com.example.smartcurtain.utils.BeatingAnimation
import com.example.smartcurtain.utils.Common
import com.example.smartcurtain.utils.Common.PushTopic
import com.example.smartcurtain.utils.MToast
import com.example.smartcurtain.utils.TimeCycle


import com.google.gson.Gson
import com.gyf.immersionbar.ImmersionBar
import com.itfitness.mqttlibrary.MQTTHelper
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken
import org.eclipse.paho.client.mqttv3.MqttCallback
import org.eclipse.paho.client.mqttv3.MqttMessage
import java.util.Objects

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    private var isDebugView = false //是否显示debug界面
    private var arrayList = ArrayList<String>()// debug消息数据
    private var adapter: ArrayAdapter<String>? = null
    private var onlineFlag = false //是否在线标识
    private var TAG = this.toString()
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        mqttServer()
        initView()
        isOnline()
    }


    /**
     * @brief 界面的初始化
     */
    private fun initView() {
        setSupportActionBar(findViewById(R.id.toolbar))
        binding.toolbarLayout.title = title
        ImmersionBar.with(this).init()
        debugView()
        eventManager()
    }

    /**
     * 事件处理
     */
    private fun eventManager() {

        binding.modeButton.setOnClickListener {
            sendMessage(1, if (binding.modeButton.isChecked) "0" else "1")
        }
        // 阈值
        binding.waterSeekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(p0: SeekBar?, p1: Int, p2: Boolean) {
                if (Common.mqttHelper?.connected == true) {
                    binding.waterSeekText.text = p1.toString()
                } else {
                    MToast.mToast(this@MainActivity, "请先建立连接")
                }
            }

            override fun onStartTrackingTouch(p0: SeekBar?) {

            }

            override fun onStopTrackingTouch(p0: SeekBar?) {
                if (p0 != null) {
                    if (Common.mqttHelper?.connected == true) {
                        val p1 = p0.progress.toString()
                        binding.waterSeekText.text = p1
                        sendMessage(2, p1)
                    } else {
                        MToast.mToast(this@MainActivity, "请先建立连接")
                    }
                }
            }

        })
        binding.waterBtn.setOnClickListener {
            binding.waterBtn.isSelected = !binding.waterBtn.isSelected
            sendMessage(3, if (binding.waterBtn.isSelected) "1" else "0")
        }
    }

    /**
     * 接收消息处理
     */
    @SuppressLint("ResourceAsColor")
    private fun analysisOfData(data: Receive?) {
        try {
            if (data != null) {
                // 温度
                if (data.temp != null) {
                    binding.tempText.text = String.format("%.2f", data.temp!!.toFloat())
                }

                // 温度阈值
                if (data.temp_v != null) {
                    binding.waterSeekBar.progress = data.temp_v!!.toInt()
                    binding.waterSeekText.text = String.format("%s", data.temp_v)
                }
                // 模式
                if (data.flage != null) {
                    binding.modeButton.isChecked = data.flage == "0"
                   if (data.flage == "1"){
                       binding.modetext.text = "恒温模式"
                   }else
                       binding.modetext.text = "正常模式"
                }
                // 模式
                if (data.wight != null) {
                    if (data.wight!!.toFloat()<50f){
                        binding.stateText.setTextColor(resources.getColor(R.color.red))
                        binding.stateText.text = "空烧警告！"
                    }else{
                        binding.stateText.setTextColor(resources.getColor(R.color.limegreen))
                        if (data.hot == "0"){
                            binding.stateText.text = "加热中~~"
                        }else
                            binding.stateText.text = "状态正常"
                    }

                }
                if (data.hot != null) {
                    binding.waterBtn.isSelected = data.hot == "1"
                }
                if (data.waning != null) {
                    if (data.waning!!.toInt() in 2..14){
                        warringLayout(true, "水烧好了！！")
                    }else
                        warringLayout(false, "水烧好了！！")
                }
            } else {
                MToast.mToast(this, "数据解析失败:解析为空")
            }

        } catch (e: Exception) {
            e.printStackTrace()
            MToast.mToast(this, "数据解析失败")
        }
    }

    /**
     * @brief 再次封装MQTT发送
     * @param message 需要发送的消息
     */
    private fun sendMessage(cmd: Int, message: String) {
        if (Common.mqttHelper != null && Common.mqttHelper!!.subscription) {
            var str = ""
            when (cmd) {
                1 -> {
                    str = Gson().toJson(
                        Send(
                            cmd, DataDTO(
                                flage = message.toInt()
                            )
                        )
                    )
                }
                2 -> {
                    str = Gson().toJson(
                        Send(
                            cmd, DataDTO(
                                temp_v = binding.waterSeekBar.progress,

                            )
                        )
                    )
                }
                3 -> {
                str = Gson().toJson(
                    Send(
                        cmd, DataDTO(
                            hot = message.toInt()
                        )
                    )
                )
            }
            }
            Thread {
                Common.mqttHelper!!.publish(
                    PushTopic, str, 1
                )
            }.start()

            debugViewData(1, str)
        }
    }

    private fun mqttServer() {
        Common.mqttHelper = MQTTHelper(
            this, Common.Sever, Common.DriveID, Common.DriveName, Common.DrivePassword, true, 30, 60
        )
        Common.mqttHelper!!.connect(Common.ReceiveTopic, 1, true, object : MqttCallback {
            override fun connectionLost(cause: Throwable?) {
                MToast.mToast(this@MainActivity, "MQTT连接断开")
            }

            override fun messageArrived(topic: String?, message: MqttMessage?) {
                //收到消息
                val data = Gson().fromJson(message.toString(), Receive::class.java)
                LogUtils.eTag("接收到消息", message?.payload?.let { String(it) })
                onlineFlag = true
                binding.online.text = "在线"
                debugViewData(2, message?.payload?.let { String(it) }!!)
                println(data)
                analysisOfData(data)
            }

            override fun deliveryComplete(token: IMqttDeliveryToken?) {

            }
        })

    }

    /***
     * 判断硬件是否在线
     */
    private fun isOnline() {
        Thread {
            var i = 0
            while (true) {//开启轮询，每18s重置标识
                if (i > 3) {
                    i = 0
                    runOnUiThread {
                        binding.online.text = if (onlineFlag) "在线" else "离线"
                    }
                    onlineFlag = false
                }
                i++
                Thread.sleep(6000)
//                Thread.sleep(2500)
            }
        }.start()
    }


    /**
     * @brief debug界面的初始化
     */
    private fun debugView() {
        adapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, arrayList)
        binding.debugView.adapter = adapter
    }

    /**
     * @brief debug界面数据添加
     * @param str 如果为 1 添加发送数据到界面   为 2 添加接受消息到界面
     */
    private fun debugViewData(str: Int, data: String) {
        if (arrayList.size >= 255) arrayList.clear()
        runOnUiThread {
            when (str) {
                1 -> { //发送的消息
                    arrayList.add("目标主题:${Common.ReceiveTopic} \n时间:${TimeCycle.getDateTime()}\n发送消息:$data ")
                }

                2 -> {
                    arrayList.add("来自主题:${Common.ReceiveTopic} \n时间:${TimeCycle.getDateTime()}\n接到消息:$data ")
                }
            }
            // 在添加新数据之后调用以下方法，滚动到列表底部
            binding.debugView.post {
                binding.debugView.setSelection(adapter?.count?.minus(1)!!)
            }
            adapter?.notifyDataSetChanged()
        }

    }


    /**
     * @brief 显示警告弹窗和设置弹窗内容
     * @param visibility 是否显示
     * @param str 显示内容
     */
    private fun warringLayout(visibility: Boolean, str: String) {
        if (visibility) {
            binding.warringLayout.visibility = View.VISIBLE
            binding.warringText.text = str
            BeatingAnimation().onAnimation(binding.warringImage)
        } else {
            binding.warringLayout.visibility = View.GONE
        }
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_scrolling, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        return when (item.itemId) {
            R.id.setDebugView -> { // set debug view is enabled or disabled
                isDebugView = !isDebugView
                binding.debugView.visibility = if (isDebugView) View.VISIBLE else View.GONE
                true
            }

            else -> super.onOptionsItemSelected(item)
        }
    }

}