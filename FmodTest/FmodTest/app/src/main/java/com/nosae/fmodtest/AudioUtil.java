package com.nosae.fmodtest;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class    AudioUtil {
    //采用频率
    //作用：how many times per second a sound is sampled每秒钟被采样多少次
    //44100是目前的标准，但是某些设备仍然支持22050，16000，11025
    private static final int SAMPLE_RATE = 44100;
    // 声明recoordBufffer的大小字段

    private static final int BUFFER_SIZE = 2048;
    // 音频通道 单声道 录音声道
    private static final int RECORD_CHANNEL = AudioFormat.CHANNEL_IN_STEREO;
    // 音频格式：PCM编码
    //介绍PCM：PCM是把声音从模拟信号（人们听到的信号）转化为数字信号的技术
    //原理：将量化后的声音脉冲的数值连续地输出、传输、处理或记录到存储介质中
    //播放音乐时，应用程序从存储介质中读取音频数据（MP3），经过解码后，最终送到音频驱动程序中的就是PCM数据
    private static final int AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT;
    // 音频通道 单声道 播放声道
    private static final int PLAY_CHANNEL = AudioFormat.CHANNEL_OUT_STEREO;
    //路径设置
    private static String appPath = "";
    private static String pcmPath = "";

    //创建AudioRecord对象
    private static AudioRecord recorder;
    //创建WAVUtil 对象
    private static WAVUtil wavUtil;

    //创建AudioRecord对象时，AudioRecord会初始化，并和音频缓冲区连接，用来缓冲新的音频数据
    //根据构造时指定的缓冲区大小，来决定AudioRecord能够记录多长的数据。从硬件设备读取的数据，应小于整个记录缓冲区
    //bufferSizeInBytes：
    //采集数据需要的缓冲区的大小，如果不知道最小需要的大小可以在getMinBufferSize()查看。

    // 声明recoordBufffer的大小字段

    //audioRecord能接受的最小的buffer大小 -- 返回成功创建AudioRecord对象所需要的最小缓冲区大小。
    //public static int getMinBufferSize (int sampleRateInHz, int channelConfig, int audioFormat)
    //sampleRateInHz      默认采样率，单位Hz。
    //channelConfig           描述音频通道设置。
    //audioFormat             音频数据保证支持此格式
    private static int playMinBufferSize = AudioTrack.getMinBufferSize(
            SAMPLE_RATE, PLAY_CHANNEL,
            AUDIO_ENCODING
    );
    //创建AudioTrack对象
    private static AudioTrack player;

    //初始化
    public static void init(Context ctx) {
        //设置路径
        appPath = ctx.getExternalFilesDir(null).getAbsolutePath();
        pcmPath = appPath + "/pcm";

        File file = new File(pcmPath);

        //Returns:
        //true if and only if the directory was created; false otherwise
        //如果目录已创建，则该方法返回 true
        //是否多余  没有返回值 这里不应该是 报错或者再次创建
        if (!file.exists()) {
            file.mkdirs();
        }

        wavUtil = new WAVUtil();
    }


    //录音
    public static String startRecord() {
        if (recorder != null){
            return null;
        }

        //fn:fileName
        String fn = pcmPath + "/录音_" + System.currentTimeMillis() + ".pcm";
        recorder = makeRecorder();
        recorder.startRecording();

        //System.currentTimeMillis()返回的数字没有辨别性
        //我想返回20220422这样能看出来日期，但是会导致重复而崩溃
        //获取当前时间
//        SimpleDateFormat sdf = new SimpleDateFormat();// 格式化时间
//        sdf.applyPattern("yyyyMMdd");// a为am/pm的标记
//        Date date = new Date();// 获取当前时间

        //Returns the current time in milliseconds
        //设置本次录音文件的详细地址：pcmpath/录音/当前时间 System.currentTimeMillis()
        //fn:fileName
//        String fn = pcmPath + "/录音_" + sdf.format(date);

        //recorder变量初始化
        recorder = makeRecorder();
        //开始录制
        recorder.startRecording();
        // 根据是否stop自动结束录音
        //多线程 复习一下竟然看不懂了
        new Thread(() -> doRecord(fn)).start();
        return fn;
    }

    public static void pauseRecord() {
        //若recorder未初始化，录音已经停止 则不需要执行停止操作
        //recorder.getRecordingState() 得到当前状态  Returns the recording state of the AudioRecord instance.
        if (recorder == null || recorder.getRecordingState() == AudioRecord.RECORDSTATE_STOPPED){
            return;
        }
        recorder.stop();
    }

    public static void resumeRecord() {
        //继续录制
        if (recorder == null || recorder.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING){
            return;
        }
        recorder.startRecording();
    }

    public static void stopRecord() {
        if (recorder == null){
            return;
        }
        recorder.stop();
        //回收
        recorder.release();
        recorder = null;
    }

    private static AudioRecord makeRecorder() {
        //AudioRecord实例化
        //(int audioSource, int sampleRateInHz, int channelConfig, int audioFormat, int bufferSizeInBytes)
        return new AudioRecord(
                // 音频源：音频输入-麦克风
                MediaRecorder.AudioSource.MIC,
                SAMPLE_RATE,
                RECORD_CHANNEL,
                AUDIO_ENCODING,
                BUFFER_SIZE
        );
    }

    private static void doRecord(String fn) {
        try {
            //音频数据存放
            byte[] buf = new byte[BUFFER_SIZE];
            //输出流
            FileOutputStream fos = new FileOutputStream(fn);
            while (true) {
                if (recorder == null){
                    break;
                }
                if (recorder.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
                    //使用方法：public int read (byte[] audioData, int offsetInBytes, int sizeInBytes)
                    //从音频硬件录制缓冲区读取数据。
                    // audioData        写入的音频录制数据。
                    // offsetInBytes    audioData的起始偏移值，单位byte
                    // sizeInBytes      读取的最大字节数 -- 缓冲区大小
                    //返回值：读入缓冲区的总byte数，如果对象属性没有初始化，则返回ERROR_INVALID_OPERATION
                    recorder.read(buf, 0, BUFFER_SIZE);
                    fos.write(buf, 0, BUFFER_SIZE);
                }
            }
            fos.close();
            //更改地址 pcm结尾变为wav结尾
            String outPath = fn.replace(".pcm", ".wav");
            //调用WAVUtil工具类实现数字信号到播放音频的转换
            wavUtil.pcmToWav(fn, outPath);
        } catch (IOException e) {
            Log.d("AudioUtil", "录音失败", e);
        }
    }

    //播放
    public static void play(String path) {
        if (player != null)
            return;

        player = makePlayer();
        player.play();
        new Thread(() -> {
            doPlay(path);
        }).start();
    }

    public static void pause() {
        player.pause();
    }

    public static void resume() {
        player.play();
    }

    public static void stop() {
        if (player == null)
            return;
        player.stop();
    }

    private static void doPlay(String path) {
        try {
            FileInputStream fis = new FileInputStream(path);
            byte[] buf = new byte[playMinBufferSize];
            while (fis.available() > 0) {
                if (player == null || player.getPlayState() == AudioTrack.PLAYSTATE_STOPPED) {
                    break;
                } else if (player.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
                    int read = fis.read(buf);
                    player.write(buf, 0, read);
                }
            }
            fis.close();
            player.stop();
            player.release();
            player = null;
        } catch (IOException e) {
            Log.d("AudioUtil", "播放失败", e);
        }
    }

    private static AudioTrack makePlayer() {
        return new AudioTrack(
                new AudioAttributes.Builder()
                        .setLegacyStreamType(AudioManager.STREAM_MUSIC)
                        .build(),
                new AudioFormat.Builder()
                        .setSampleRate(SAMPLE_RATE)
                        .setEncoding(AUDIO_ENCODING)
                        .setChannelMask(PLAY_CHANNEL)
                        .build(),
                playMinBufferSize, AudioTrack.MODE_STREAM, AudioManager.AUDIO_SESSION_ID_GENERATE
        );
    }
}