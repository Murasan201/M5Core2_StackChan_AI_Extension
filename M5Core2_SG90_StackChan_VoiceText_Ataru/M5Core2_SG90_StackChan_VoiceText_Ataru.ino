#include <Arduino.h>
#include <M5Unified.h>

// サーボモーターの接続ピン番号
#define SERVO_PIN_X 13
#define SERVO_PIN_Y 14

#include <Avatar.h>         // M5Stack用のアバターライブラリ：https://github.com/meganetaaan/m5stack-avatar
#include <ServoEasing.hpp>   // サーボのイージング動作用ライブラリ：https://github.com/ArminJo/ServoEasing
#include "AtaruFace.h"       // あたるの顔データ
#include "RamFace.h"         // ラムちゃんの顔データ

// 音声合成機能（VoiceText）の使用を有効化
//#define USE_VOICE_TEXT //for M5STACK_Core2 Only

#ifdef USE_VOICE_TEXT
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SLipSync.h"
#include "AudioFileSourceVoiceTextStream.h"

// Wi-Fi接続用の情報（実際のSSIDとPASSWORDに置き換えてください）
const char *SSID = "YOUR_WIFI_SSID";
const char *PASSWORD = "YOUR_WIFI_PASSWORD";


// 音声再生用オブジェクトの宣言
AudioGeneratorMP3 *mp3;
AudioFileSourceVoiceTextStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2SLipSync *out;
const int preallocateBufferSize = 40 * 1024;  // 音声バッファサイズの定義
uint8_t *preallocateBuffer;
#endif

using namespace m5avatar;

// アバターオブジェクトと各顔、カラーパレットのインスタンス
Avatar avatar;
Face* faces[3];
ColorPalette* cps[3];

// サーボの初期角度設定（X軸, Y軸）
#define START_DEGREE_VALUE_X 90
#define START_DEGREE_VALUE_Y 90

// サーボイージングを使用してなめらかに動作させるためのオブジェクト
ServoEasing servo_x;
ServoEasing servo_y;

// ----------------------------------------------
// タスク関数：アバターの行動制御（主に口の動きの調整）
// ----------------------------------------------
void behavior(void *args)
{
  float gazeX, gazeY;
  DriveContext *ctx = (DriveContext *)args;
  Avatar *avatar = ctx->getAvatar();
  for (;;)
  {
#ifdef USE_VOICE_TEXT
    // スピーカー出力のレベルを取得し、音量に応じた口の開閉具合を計算
    int level = out->getLevel();
    level = abs(level);
    if(level > 10000)
    {
      level = 10000;
    }
    float open = (float)level / 10000.0;
    // 口の開き具合を設定（0.0～1.0の値）
    avatar->setMouthOpenRatio(open);
#endif
    // タスクループ内で短時間待機
    vTaskDelay(1 / portTICK_PERIOD_MS);
    // delay(50);  // 以前は50ms待機していたが、現在は1msに変更
  }
}

// ----------------------------------------------
// タスク関数：サーボ制御（アバターの視線に合わせてサーボ動作を実現）
// ----------------------------------------------
void servoloop(void *args)
{
  //仮でサーボ停止
  return;
  float gazeX, gazeY;
  DriveContext *ctx = (DriveContext *)args;
  for (;;)
  {
    Avatar *avatar = ctx->getAvatar();
    // アバターの視線(gaze)を取得。gazeXとgazeYは通常-1～1の範囲を取る
    avatar->getGaze(&gazeY, &gazeX);
    // X軸サーボの角度を視線に合わせて調整（+-20度の範囲）
    servo_x.setEaseTo(START_DEGREE_VALUE_X + (int)(20.0 * gazeX));
    // Y軸サーボは視線が下方向の場合は+-20度、上方向の場合は+-10度で調整
    if (gazeY < 0) {
      servo_y.setEaseTo(START_DEGREE_VALUE_Y + (int)(20.0 * gazeY));
    } else {
      servo_y.setEaseTo(START_DEGREE_VALUE_Y + (int)(10.0 * gazeY));
    }
    // 全サーボの動作を同期し、全動作完了まで待機
    synchronizeAllServosStartAndWaitForAllServosToStop();
    // 約33ms待機（約30fpsの更新レート）
    vTaskDelay(33 / portTICK_PERIOD_MS);
  }
}

// ----------------------------------------------
// setup関数：初期化処理
// ----------------------------------------------
void setup() {
#ifdef USE_VOICE_TEXT
  // 音声再生用バッファの確保
  preallocateBuffer = (uint8_t*)ps_malloc(preallocateBufferSize);
#endif
  // M5Stackの基本設定の取得と初期化
  auto cfg = M5.config();
  M5.begin(cfg);
  
  // スピーカー設定（サンプルレートとモノラル設定）
  auto spk_config = M5.Speaker.config();
  spk_config.sample_rate = 88200;
  spk_config.stereo = false;
  M5.Speaker.config(spk_config);
  // M5.Speaker.begin();  // 必要に応じてアンコメント

  // サーボの初期設定：指定ピン、初期角度、PWMパルス幅の設定
  if (servo_x.attach(SERVO_PIN_X, START_DEGREE_VALUE_X, DEFAULT_MICROSECONDS_FOR_0_DEGREE, DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo x");
  }
  if (servo_y.attach(SERVO_PIN_Y, START_DEGREE_VALUE_Y, DEFAULT_MICROSECONDS_FOR_0_DEGREE, DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo y");
  }
  // イージングタイプの設定（滑らかな動作のための補間）
  servo_x.setEasingType(EASE_QUADRATIC_IN_OUT);
  servo_y.setEasingType(EASE_QUADRATIC_IN_OUT);
  // サーボ動作速度の設定
  setSpeedForAllServos(60);

  //ふきだしに関する表示設定
  avatar.setSpeechFont(&fonts::lgfxJapanGothic_12);

#ifdef USE_VOICE_TEXT
  // ディスプレイの初期設定：明るさ、クリア、文字サイズ設定
  M5.Lcd.setBrightness(100);
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);
  delay(1000);

  // Wi-Fi接続処理
  Serial.println("Connecting to WiFi");
  M5.Lcd.print("Connecting to WiFi");
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  // Wi-Fiが接続されるまで待機
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    M5.Lcd.print(".");
  }
  Serial.println("\nConnected");
  M5.Lcd.println("\nConnected");
  // 接続確認後、スピーカーでトーン再生
  M5.Speaker.tone(2000, 500);
  delay(500);
  M5.Speaker.tone(1000, 500);
  delay(1000);
  
  // 音声再生用の初期設定
  audioLogger = &Serial;
  out = new AudioOutputI2SLipSync(0, 0);
  out->SetPinout(12, 0, 2);  // I2Sのピン設定（BCK, LRCK, DATA）
  out->SetOutputModeMono(false);
  mp3 = new AudioGeneratorMP3();
#endif

  // 顔データとカラーパレットの初期化
  faces[0] = new AtaruFace();
  faces[1] = new RamFace();
  faces[2] = avatar.getFace();
  cps[0] = new ColorPalette();
  cps[1] = new ColorPalette();
  cps[2] = new ColorPalette();

  // カラーパレットの設定（各顔の配色）
  cps[0]->set(COLOR_PRIMARY, TFT_BLACK);
  cps[0]->set(COLOR_BACKGROUND, TFT_WHITE);
  cps[0]->set(COLOR_SECONDARY, TFT_WHITE);
  
  cps[1]->set(COLOR_PRIMARY, TFT_BLACK);
  cps[1]->set(COLOR_BACKGROUND, TFT_WHITE);
  cps[1]->set(COLOR_SECONDARY, TFT_WHITE);
  
  cps[2]->set(COLOR_PRIMARY, TFT_WHITE);
  cps[2]->set(COLOR_BACKGROUND, TFT_BLACK);
  cps[2]->set(COLOR_SECONDARY, TFT_WHITE);

  // アバターの初期化と初期設定（顔とパレットの割り当て）
  avatar.init(8);
  avatar.setFace(faces[2]);
  avatar.setColorPalette(*cps[2]);
  // タスクとして行動制御とサーボ制御をアバターへ追加
  avatar.addTask(behavior, "behavior");
  //avatar.addTask(servoloop, "servoloop");
}

#ifdef USE_VOICE_TEXT
// 各種テキストおよびTTS用パラメータの定義
char *text1 = "こんにちは、僕の名前はあたるです。よろしくね！";
char *text2 = "こんにちは、私の名前はラムちゃんです。よろしくね！";
char *text3 = "こんにちは、私の名前はスタックちゃんです。よろしくね！";
char *tts_parms1 = "&emotion_level=2&emotion=happiness&format=mp3&speaker=takeru&volume=200&speed=100&pitch=130";
char *tts_parms2 = "&emotion_level=2&emotion=happiness&format=mp3&speaker=hikari&volume=200&speed=120&pitch=130";
char *tts_parms3 = "&emotion_level=4&emotion=anger&format=mp3&speaker=bear&volume=200&speed=120&pitch=100";

// 音声合成（Text-to-Speech）を開始するための関数
void VoiceText_tts(char *text, char *tts_parms) {
    // 指定されたテキストとパラメータから音声ストリームを生成
    file = new AudioFileSourceVoiceTextStream(text, tts_parms);
    // 生成したストリームをバッファリング
    buff = new AudioFileSourceBuffer(file, preallocateBuffer, preallocateBufferSize);
    delay(100);
    // mp3再生を開始
    mp3->begin(buff, out);
}
#endif

//表情に応じてメッセージを生成
char* GenText(int expression){

}

// ----------------------------------------------
// メインループ：ユーザ入力に応じた処理を実行
// ----------------------------------------------
void loop() {
  // ボタンやタッチなどの状態更新
  M5.update();
#ifdef USE_VOICE_TEXT
  static int lastms = 0;
  // ボタンAが押された場合の処理
  if (M5.BtnA.wasPressed())
  {
    // スピーカーでトーンを鳴らしてフィードバック
    M5.Speaker.tone(2000, 500);
    // 顔とカラーパレットを顔0（あたる）に切り替え
    avatar.setFace(faces[0]);
    avatar.setColorPalette(*cps[0]);
    delay(1000);
    // 表情をHappyに変更してからTTSでメッセージ再生
    avatar.setExpression(Expression::Happy);
    VoiceText_tts(text1, tts_parms1);
    // 再生後、表情をNeutralに戻す
    avatar.setExpression(Expression::Neutral);
    Serial.println("mp3 begin");
  }
  // ボタンBが押された場合の処理
  if (M5.BtnB.wasPressed())
  {
    // フィードバックトーン
    M5.Speaker.tone(2000, 500);
  
    // 吹き出し（セリフ）を画面に表示（背景は白、文字は黒）
    displayDialogue("ゆいちゃん、こんにちは♪");

    // 5秒間待機
    delay(5000);

    // セリフ領域をクリアして消去
    clearDialogue();
    /*
    // ボタンBに対応したフィードバックトーン
    M5.Speaker.tone(2000, 500);
    // 顔とカラーパレットを顔1（ラムちゃん）に切り替え
    avatar.setFace(faces[1]);
    avatar.setColorPalette(*cps[1]);
    delay(1000);
    // 表情をHappyにしてTTSでラムちゃんのメッセージを再生
    avatar.setExpression(Expression::Happy);
    VoiceText_tts(text2, tts_parms2);
    // 再生完了後、表情をNeutralに戻す
    avatar.setExpression(Expression::Neutral);
    Serial.println("mp3 begin");
    */
    
  }
  // ボタンCが押された場合の処理
  if (M5.BtnC.wasPressed())
  {
    // ボタンCに対応したフィードバックトーン
    M5.Speaker.tone(2000, 500);
    // 顔とカラーパレットを顔2（スタックちゃん）に切り替え
    avatar.setFace(faces[2]);
    avatar.setColorPalette(*cps[2]);
    delay(1000);
    // 表情をHappyに変更してTTSでスタックちゃんのメッセージを再生
    avatar.setExpression(Expression::Happy);
    VoiceText_tts(text3, tts_parms3);
    // 再生完了後、表情をNeutralに戻す
    avatar.setExpression(Expression::Neutral);
    Serial.println("mp3 begin");
  }
  // mp3が再生中の場合の処理
  if (mp3->isRunning()) {
    // 1秒ごとに再生時間を出力（デバッグ用）
    if (millis() - lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
    }
    // mp3の再生ループ。再生が終了した場合は停止処理を実施
    if (!mp3->loop()) {
      mp3->stop();
      out->setLevel(0);
      delete file;
      delete buff;
      Serial.println("mp3 stop");
    }
  }
#else
  // USE_VOICE_TEXTが無効の場合：ボタン押下により顔とカラーパレットのみ切替
  if (M5.BtnA.wasPressed())
  {
    // フィードバックトーン
    M5.Speaker.tone(2000, 500);

    //ふきだしの表示
    avatar.setSpeechText("ゆいちゃん、こんにちは");
    avatar.setExpression(Expression::Happy);

    avatar.setMouthOpenRatio(0.7);
    delay(3000);
    avatar.setMouthOpenRatio(0);

    avatar.setSpeechText("");
    avatar.setExpression(Expression::Neutral);
  }
  if (M5.BtnB.wasPressed())
  {
    // フィードバックトーン
    M5.Speaker.tone(2000, 500);

    //ふきだしの表示
    avatar.setSpeechText("ゆいちゃん、こんにちは");
    avatar.setExpression(Expression::Happy);

    avatar.setMouthOpenRatio(0.7);
    delay(3000);
    avatar.setMouthOpenRatio(0);

    avatar.setSpeechText("");
    avatar.setExpression(Expression::Neutral);
    /*
    // ボタンBが押された場合の処理（顔1に切替）
    avatar.setFace(faces[1]);
    avatar.setColorPalette(*cps[1]);
    */
  }
  if (M5.BtnC.wasPressed())
  {
    // ボタンCが押された場合の処理（顔2に切替）
    avatar.setFace(faces[2]);
    avatar.setColorPalette(*cps[2]);
  }
#endif
}
