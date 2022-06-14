package com.nosae.fmodtest;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        //add-library中生成的库.so，在这里引用
        // 查找加载 引用apk lib/Pingtai/libvoicechangeku.so
        System.loadLibrary("native-lib");
        System.loadLibrary("fmod");
        System.loadLibrary("fmodL");
    }

    //多种播放模式
    public static final int TYPE_NORMAL = 0;    // 普通
    public static final int TYPE_LOLITA = 1;      // 萝莉
    public static final int TYPE_UNCLE = 2;       // 大叔
    public static final int TYPE_THRILLER = 3;    // 惊悚
    public static final int TYPE_FUNNY = 4;       // 搞怪
    public static final int TYPE_ETHEREAL = 5;    // 空灵
    public static final int TYPE_CHORUS = 6;    //合唱团
    public static final int TYPE_TREMOLO = 7;    //颤音
    //更多声音
    public static final int TYPE_TOTALLOSS =8;//全损音
    public static final int TYPE_3D=9;//3D立体环绕TYPE_BROADCAST
    public static final int TYPE_BROADCAST=10;
    public static final int TYPE_DELAY=11;
    public static final int TYPE_FOGGY=12;
    public static final int TYPE_DEVIL=13;
    public static final int TYPE_PHONECALL=14;

    private String path = "";

    //需要申请的运行时权限
    private String[] permissions = new String[]{
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };


    ////请求用户授权几个权限，调用后系统会显示一个请求用户授权的提示对话框，App不能配置和修改这个对话框，
    // 如果需要提示用户这个权限相关的信息或说明，需要在调用 requestPermissions() 之前处理
    //int requestCode: 会在回调onRequestPermissionsResult()时返回，用来判断是哪个授权申请的回调。
    //个人理解：requestCode 相当于ID，当申请多个权限的时候，区分是那个权限---这里似乎没有达到区分的目的：）
    private static final int MY_PERMISSIONS_REQUEST = 1001;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //请求获得权限~
        ActivityCompat.requestPermissions(this, permissions, MY_PERMISSIONS_REQUEST);

        //录音、播放、变声初始化
        AudioUtil.init(this);

        //点击事件
        findViewById(R.id.btn1).setOnClickListener(v -> path = AudioUtil.startRecord());
        findViewById(R.id.btn2).setOnClickListener(v -> AudioUtil.pauseRecord());
        findViewById(R.id.btn3).setOnClickListener(v -> AudioUtil.resumeRecord());
        findViewById(R.id.btn4).setOnClickListener(v -> AudioUtil.stopRecord());
        findViewById(R.id.btn5).setOnClickListener(v -> AudioUtil.play(path));
        findViewById(R.id.btn6).setOnClickListener(v -> AudioUtil.pause());
        findViewById(R.id.btn7).setOnClickListener(v -> AudioUtil.resume());
        findViewById(R.id.btn8).setOnClickListener(v -> AudioUtil.stop());

        findViewById(R.id.btn9).setOnClickListener(v -> play(TYPE_NORMAL));
        findViewById(R.id.btn10).setOnClickListener(v -> play(TYPE_LOLITA));
        findViewById(R.id.btn11).setOnClickListener(v -> play(TYPE_UNCLE));
        findViewById(R.id.btn12).setOnClickListener(v -> play(TYPE_THRILLER));
        findViewById(R.id.btn13).setOnClickListener(v -> play(TYPE_FUNNY));
        findViewById(R.id.btn14).setOnClickListener(v -> play(TYPE_ETHEREAL));
        findViewById(R.id.btn15).setOnClickListener(v -> play(TYPE_CHORUS));
        findViewById(R.id.btn16).setOnClickListener(v -> play(TYPE_TREMOLO));
        findViewById(R.id.btn17).setOnClickListener(v -> play(TYPE_TOTALLOSS));
        findViewById(R.id.btn18).setOnClickListener(v -> play(TYPE_3D));
        findViewById(R.id.btn19).setOnClickListener(v -> play(TYPE_BROADCAST));
        findViewById(R.id.btn20).setOnClickListener(v -> play(TYPE_DELAY));
        findViewById(R.id.btn21).setOnClickListener(v -> play(TYPE_FOGGY));
        findViewById(R.id.btn22).setOnClickListener(v -> play(TYPE_DEVIL));
        findViewById(R.id.btn23).setOnClickListener(v -> play(TYPE_PHONECALL));
    }

    public void play(int mode) {
        new Thread(() -> {
            play(path.replace(".pcm", ".wav"), mode);
        }).start();


    }

    public native String stringFromJNI();
    //native函数
    //提供了fmod的jar包，意味着做了封装，希望我们调用jar包来初始化FMOD
    //方法一：ALT + 回车 创建对应的JNI函数
    //方法二（具体生成的原理）：javah com.example.voicechange.MainActivity
    //此时 全盘检查，遇到常量会自动生成C语言的“宏” （对标Java的常量） ； 检测到native函数，对应生成JNI函数
    //JNIEXPORT void JNICALL Java_com_example_voicechange_MainActivity_voiceChangeNative
    //  (JNIEnv *, jobject, jint, jstring);
    public native void play(String path, int mode);
}