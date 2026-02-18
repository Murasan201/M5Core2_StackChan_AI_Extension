# Raspberry Pi 5 → M5Stack Core2 Development Guide

## 1. この環境の概要
- ホストは **Raspberry Pi 5 (OpenClaw workspace)** で、`project/M5Core2_StackChan_AI_Extension` にこのスタックちゃん AI 拡張のソースが置かれています。
- Core2 本体は USB ケーブルで直接 Pi に接続されており、現在の設定でソースコードの編集・ビルド・書き込みをこの Pi から完結させることを目指します。
- 開発は CLI（`arduino-cli`）ベースで進める前提で手順を書いています。

## 2. 接続と通信確認
1. `lsusb` の出力で `10c4:ea60 Silicon Labs CP210x UART Bridge`（Bus 003 Device 009）が確認でき、Pi 上には `CP210x` ドライバが読み込まれています。これは Core2 の USB-Serial インターフェースに相当します。
2. デバイスノードは `/dev/ttyUSB0` です（`ls /dev/ttyUSB*` で見えており、`ttyACM*` は存在しません）。このポートが Pi → Core2 の通信経路になります。
3. `arduino-cli board list` で `/dev/ttyUSB0` が「Serial Port」として認識されていて、`board name` は `Unknown` ですが、ESP32/ M5Stack のボード定義をインストールすれば書き込み可能です。
4. `screen /dev/ttyUSB0 115200` や `minicom -D /dev/ttyUSB0 -b 115200` などで Core2 のシリアル出力（ログやデバッグメッセージ）を確認できます。終了時は `Ctrl-A k` などで detach してください。

## 3. 開発ツールと依存関係
1. **arduino-cli** がこの Pi にインストール済み（`arduino-cli version` で確認できます）。
2. ESP32/ M5Stack 用のコア定義を追加しておきます。
   ```sh
   arduino-cli core update-index
   arduino-cli core install esp32:esp32
   ```
3. 必要なライブラリ（M5Unified、M5Stack-Avatar、ServoEasing、ESP32Servo、ESP8266Audio）は `.ino` ファイル内で `#include` されており、`arduino-cli lib install` で取得できます：
   ```sh
   arduino-cli lib install "M5Unified"
   arduino-cli lib install "M5Stack-Avatar"
   arduino-cli lib install "ServoEasing"
   arduino-cli lib install "ESP32Servo"
   arduino-cli lib install "ESP8266Audio"
   ```
   ※ すでにボード内蔵の `M5Stack` ライブラリが使われている場合は追加不要です。必要に応じて `arduino-cli lib list` で確認してください。
4. `M5Core2_StackChan_AI_Extension/M5Core2_SG90_StackChan_VoiceText_Ataru/M5Core2_SG90_StackChan_VoiceText_Ataru.ino` が main sketch です。VoiceText を使う場合は `#define USE_VOICE_TEXT` を `//` で切り替えてください。

## 4. ビルド手順（Pi 5 上）
1. リポジトリのルートから：
   ```sh
   cd /home/pi/.openclaw/workspace/project/M5Core2_StackChan_AI_Extension
   ```
2. コンパイル対象は `.ino` ファイル全体です。典型的にはボード FQBN（例：`esp32:esp32:m5stack-core2` など）を使います。
   ```sh
   arduino-cli compile --fqbn esp32:esp32:m5stack-core2 M5Core2_SG90_StackChan_VoiceText_Ataru/M5Core2_SG90_StackChan_VoiceText_Ataru.ino
   ```
   - お使いの `arduino-cli` バージョンやコア定義によって `esp32:esp32:esp32` のように変わる場合があります。`arduino-cli board listall | grep Core2` で候補を確認してください。
3. コンパイルが通ったら、ポート指定を `--port /dev/ttyUSB0` で明示して書き込みます：
   ```sh
   arduino-cli upload --fqbn esp32:esp32:m5stack-core2 --port /dev/ttyUSB0 M5Core2_SG90_StackChan_VoiceText_Ataru/M5Core2_SG90_StackChan_VoiceText_Ataru.ino
   ```
   - Core2 の Boot モードが必要なら「BOOT」ボタンを押しながら書き込みを開始する、または `esptool.py --before default_reset --after hard_reset write_flash ...` を試すこともできます。
4. 成功するとシリアルモニタや本機の画面でスタックちゃんが反応します。`M5.Speaker` などの動きが必要な場合は `USE_VOICE_TEXT` の有効化と `VoiceText` API の設定が別途必要です。

## 5. ランタイム/書き込み後の確認
- `screen /dev/ttyUSB0 115200` でリアルタイムログを見ると、`Serial.println` の出力や `VoiceText_tts` の開始ログが流れます。
- 必要に応じて `M5.BtnA/B/C` を押して、ボタンでの挙動と吹き出し（AI Extension では日本語テキスト）を確認します。
- サーボ制御ループはデフォルトで `return` しているので、Servo は動きません。必要なら `servoloop` 内の `return` を外し、タスク登録を再度有効化してください。

## 6. 開発ルールと次のステップ
- この作業を含めて、`project/README-index.md` に今回のガイドや新規リポジトリの概要（場所、目的、README path）を追記し続けること。
- 以後、新しいリポジトリを `project/` にクローンした際は、このドキュメントおよび索引を更新しておくこと。
- 変更はこのリポジトリ内で完結させ、`git status` → `git add` → `git commit -m "docs: add dev guide"` の流れで履歴を残してください。
