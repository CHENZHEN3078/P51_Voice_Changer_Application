#include <jni.h>
#include <string>
#include "inc/fmod.hpp"
#include <unistd.h>
#include <android/log.h>

using namespace FMOD;//使用秘密控件

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"fmodSound",FORMAT,##__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"fmodSound",FORMAT,##__VA_ARGS__)

#define TYPE_NORMAL  0
#define TYPE_LOLITA  1
#define TYPE_UNCLE   2
#define TYPE_THRILLER  3
#define TYPE_FUNNY  4
#define TYPE_ETHEREAL  5
#define TYPE_CHORUS  6
#define TYPE_TREMOLO  7
#define TYPE_TOTALLOSS  8
#define TYPE_3D 9
#define TYPE_BROADCAST  10
#define TYPE_DELAY 11
#define TYPE_FOGGY 12
#define TYPE_DEVIL 13
#define TYPE_PHONECALL 14

//Channel *channel;

//不用管 另一个自动JNI函数罢了
extern "C" JNIEXPORT jstring JNICALL Java_com_nosae_fmodtest_MainActivity_stringFromJNI
( JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//native函数声明后的实现
//extern "c" 作用：必须采用C的标准 --- C语音不允许函数进行重载   若出现函数重载就报错
//若不使用↑ 则采用C++的标准，允许函数重载 -- 产生混乱
//JNIEnv 的原理 全部采用C的标准，所以必须使用C的标准！
//JNIEXPORT 作用：对外暴露JNI
//JNICALL ：JNI函数标记
extern "C" JNIEXPORT void JNICALL Java_com_nosae_fmodtest_MainActivity_play
(JNIEnv *env, jobject thiz, jstring path, jint mode) {
    //  上面一行解释： 在MainActivity中 传入的mode path记录  jstring path需要转换成char*
    //告诉上层 是否播放完毕
    //content_是字符串
    //java层：string  -- JNI层：jstring -- C层： char*

    //所有的声音都在音轨上播放，而在音轨之上必须要有音频的引擎系统 --- 引擎系统中又音轨，音轨中有声音
    //所以需要三个环境的构建
    System * system = 0;//fmod音效引擎系统
    Sound * sound = 0; //fmod声音
    Channel * channel = 0; //通道 音轨
    DSP * dsp = 0; //digital signal process == 数字信号处理


    float frequency;//频率
    bool isPlaying = true;
    FMOD_VECTOR listenerpos={10.0f,10.0f,-5.0f};
    FMOD_RESULT res;

    //创建系统
    System_Create(&system);//& 取出该指针所对应的地址 ， 此时便能操作引擎系统
    //系统的初始化
    system->init(32, FMOD_INIT_NORMAL, nullptr);


    const char *path_cstr = env->GetStringUTFChars(path, nullptr);

    try {
        auto *ex_info = new FMOD_CREATESOUNDEXINFO{};
        ex_info->defaultfrequency = 44100;
        ex_info->numchannels = 2;
        ex_info->format = FMOD_SOUND_FORMAT_PCM16;
//        res = system->createSound(path_cstr, FMOD_OPENRAW, ex_info, &sound);
        LOGI("%s", path_cstr);
        res = system->createSound(path_cstr, FMOD_DEFAULT, nullptr, &sound);
        if (res != FMOD_OK) {
            LOGE("创建声音失败 %d %d", res, FMOD_ERR_INVALID_PARAM);
//            return;
        }

        switch (mode) {
            case TYPE_NORMAL:  // 普通
                //直接播放
                system->playSound(sound, nullptr, false, &channel);
                break;
            case TYPE_LOLITA:  // 萝莉
                //音调高 萝莉
                //1.创建DSP类型的pitch音调调节  默认正常1.0
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);    // 可改变音调
                //2.设置Pitch音调调节为：8.0  音调很高是萝莉 高八个度 很夸张
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 8.0);
                //3.将调节好的萝莉音添加到通道中去播放
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;

            case TYPE_UNCLE:  // 大叔
                //音调低 大叔
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.8);
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;

            case TYPE_THRILLER:   // 惊悚

                // 惊悚音效：特点： 很多声音的拼接

                // 音调低
                // 1.创建DSP类型的Pitch 音调条件
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
                // 2.设置Pitch音调调节2.0
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.7f);
                // 3.添加音效进去 音轨
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp); // 第一个音轨

                // 回音 ECHO
                system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
                dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 200); // 回音 延时    to 5000.  Default = 500.
                dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 5); // 回音 衰减度 Default = 50   0 完全衰减了
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(1, dsp); // 第二个音轨

                // 颤抖
                // Tremolo 颤抖音 正常5    非常颤抖  20
                system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &dsp);
                dsp->setParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, 50); // 非常颤抖
                dsp->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 5); // ？？？
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(2, dsp); // 第三个音轨
                // 惊悚音效：特点： 很多声音的拼接

                // 音调低
                // 1.创建DSP类型的Pitch 音调条件
//                system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &dsp);       //可改变颤1音
//                dsp->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 5);           // 时间偏移低频振荡周期
//                system->playSound(sound, nullptr, false, &channel);
//                channel->addDSP(0, dsp);
                break;
            case TYPE_FUNNY:  // 搞怪
                system->createDSPByType(FMOD_DSP_TYPE_NORMALIZE, &dsp);    //放大声音
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);

                //特点：频率很快
                //从通道里拿出来频率，原始频率
                //在原来的频率上修改频率 变成小黄人的声音~
                channel->getFrequency(&frequency);
                frequency = frequency * 1.8;                                  //频率*1.5
                channel->setFrequency(frequency);
                break;
            case TYPE_ETHEREAL: // 空灵
                //空灵的本质：有回音 像是大山里说话
                //回音 ECHO
                // 回音的延时时间  ； 回音的减弱度
                system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);          // 控制回声
                dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 300);           // 延时
                dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 20);         // 回波衰减的延迟

                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;
            case TYPE_CHORUS://和声
                system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
                dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 100);
                dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 50);
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;
            case TYPE_TREMOLO://颤音
                system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &dsp);
                dsp->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 0.8);
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;
            case TYPE_TOTALLOSS:  // 全损音
                //改变 MULTIBAND_EQ中的参数
                system->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &dsp);
                //2.设置MULTIBAND_EQ音频调节
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_A_FREQUENCY, 3410);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_A_Q, 4.26);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_B_FREQUENCY, 4020);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_B_Q, 7.23);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_C_FREQUENCY, 2200);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_C_Q, 0.10);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_D_FREQUENCY, 2300);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_D_Q, 10.0);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_E_FREQUENCY, 3480);
                dsp->setParameterFloat(FMOD_DSP_MULTIBAND_EQ_E_Q, 0.10);

                //3.将调节好的全损音添加到通道中去播放
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;

            case TYPE_3D:
                system->set3DListenerAttributes(0,&listenerpos,0,0,0);
                system->createSound(path_cstr,FMOD_3D,NULL,&sound);
                system->playSound(sound, nullptr, false,&channel);
                channel->addDSP(0, dsp);
                break;

            case TYPE_BROADCAST:  // 广播音
                system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);          // 控制回声
                dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 500);           // 延时
                dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 20);         // 回波衰减的延迟
                dsp->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL, 500);
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;
            case TYPE_DELAY:  // 延迟音
                //改变Delay中的参数
                system->createDSPByType(FMOD_DSP_TYPE_DELAY, &dsp);
                //2.设置DELAY音频调节
                dsp->setParameterFloat(FMOD_DSP_DELAY_CH0, 940);
                dsp->setParameterFloat(FMOD_DSP_DELAY_CH1, -8.50);
                dsp->setParameterFloat(FMOD_DSP_DELAY_CH2, 0.69);
                dsp->setParameterFloat(FMOD_DSP_DELAY_CH3, -6.00);
            case TYPE_FOGGY:  // 雾蒙蒙
                //音调不改变，改变3-EQ内两排的参数
                //1.创建DSP类型的3-EQ音调调节
                system->createDSPByType(FMOD_DSP_TYPE_THREE_EQ, &dsp);
                //2.设置3-EQ音调调节
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_LOWGAIN, 10.0);
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_MIDGAIN, -1000000.0);
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_HIGHGAIN, 10.0);
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_LOWCROSSOVER, 440.0);
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_HIGHCROSSOVER, 9000.0);

                //3.将调节好的雾蒙蒙音添加到通道中去播放
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;
            case TYPE_DEVIL:  // 魔鬼音
                //改变音调（两行共四个都要调）
                //1.创建DSP类型的pitch音调调节
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
                //2.设置Pitch音调调节
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.5);
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_OVERLAP, 18.6);
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 4100.0);
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_MAXCHANNELS,6.90);

                //3.将调节好的魔鬼音添加到通道中去播放
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;

            case TYPE_PHONECALL:  // 打电话音
                //音调不改变，改变3-EQ内两排的参数
                //1.创建DSP类型的3-EQ音调调节
                system->createDSPByType(FMOD_DSP_TYPE_THREE_EQ, &dsp);
                //2.设置3-EQ音调调节
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_LOWGAIN, -100000000000000.0);
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_MIDGAIN, 10.0);
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_HIGHGAIN, -12.0);
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_LOWCROSSOVER, 2000.0);
                dsp->setParameterFloat(FMOD_DSP_THREE_EQ_HIGHCROSSOVER, 6500.0);

                //3.将调节好的打电话音添加到通道中去播放
                system->playSound(sound, nullptr, false, &channel);
                channel->addDSP(0, dsp);
                break;







            default:
                LOGE("模式(%d)不存在", mode);
                break;
        }
    } catch (...) {
        LOGE("%s", "catch exception...");
        goto end;
    }

//    system->update();

    // 每隔一秒检测是否播放结束
    while (isPlaying) {
        channel->isPlaying(&isPlaying);    //如果通道播放完成，会自动去修改isPlay地址的值，修改为false后跳出循环
        usleep(1000*1000);  //睡眠一秒钟
    }

    goto end;

    //释放资源
    //写C代码时，要做到实时回收   此时就不能播放了-- 一播放 就会被回收
    end:
    env->ReleaseStringUTFChars(path, path_cstr);
    sound->release();
    system->close();
    system->release();
}