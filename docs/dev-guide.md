# Raspberry Pi → M5Stack Core2 Development Guide

この資料は、Raspberry Pi 5 を使って `M5Core2_StackChan_AI_Extension` の Core2 アプリケーションを開発・ビルド・テストするための教材です。これまでの手順も含め、環境構築と試行錯誤の記録を整理しました。

## 1. 環境と前提
- **開発ホスト**：Raspberry Pi 5（OpenClaw workspace `/home/pi/.openclaw/workspace`）。
- **Core2 接続**：USB-Serial（Silicon Labs CP210x）で `/dev/ttyUSB0` として認識。`lsusb` で `10c4:ea60` を確認し、`arduino-cli board list` でポートが出るかチェックします。
- **リポジトリ**：作業ディレクトリは `/home/pi/.openclaw/workspace/project/M5Core2_StackChan_AI_Extension`。
- **ルール**：`docs/tests.md` に TDDテストを書き、`docs/troubleshooting.md` にエラーと対処を記録。`AGENTS.md` と `RULES.md` に従う。

## 2. 必要ツール（これまで実行したコマンド）
1. `arduino-cli core update-index` → インデックスを取得。
2. `arduino-cli core install esp32:esp32@3.3.7` → ESP32コアをインストール。
3. ライブラリインストール（プロジェクトの`#include`に対応）：
   - `arduino-cli lib install "M5Unified"`
   - `arduino-cli lib install "ServoEasing"`
   - `arduino-cli lib install "ESP32Servo"`
   - `arduino-cli lib install "ESP8266Audio"`
   - `arduino-cli lib install "ArduinoJson"`
   - `m5stack-avatar` はソースを `~/Arduino/libraries/M5Stack-Avatar` にコピー（`/tmp/m5stack-avatar` を rsync ）。
4. `control_stackchan.py` を作成（pyserial CLI）および `docs/interface-requirements.md` で仕様整備。
5. `docs/tests.md`, `docs/journal.md`, `docs/troubleshooting.md` で記録・テスト管理。

## 3. ここまでの開発手順（チェックリスト）
- [x] Cloned the repository under `/project/M5Core2_StackChan_AI_Extension`. Validate workspace symlinks.
- [x] Recorded environment, Canvas interactions, and rules in `docs/development-environment.md`, `AGENTS.md`, `RULES.md`.
- [x] Implemented the serial JSON parser in the sketch and created `control_stackchan.py`.
- [x] Added TDD guidance and interim tests in `docs/tests.md`.
- [x] Logged troubleshooting notes about patch/compile issues and built the `arduino-cli` toolchain.
- [ ] Fix `WiFiClient`/`DrawContext` build errors (current blocking issue).

## 4. 今後の手順（Next steps）
1. **Build fix**：`AudioFileSourceVoiceTextStream` に `<WiFi.h>`, `Ataru*` ソースに `DrawContext` 互換処理を追加または `Avatar` のメソッドを使う。エラーを `docs/troubleshooting.md` に追記しつつ進める。
2. **Rebuild**：`arduino-cli compile --fqbn esp32:esp32:m5stack_core2 ...` でビルド、続けて `arduino-cli upload ...` で Core2 に書き込み。
3. **Run tests**：`control_stackchan.py` で commands を送り、`docs/tests.md` に結果を記入（pass/fail）。失敗があれば詳細をトラブルシュートに記録。
4. **Verification**：画面で表情/吹き出しの変化を確認し、記録を `docs/journal.md` にまとめる。

## 5. Tips & Notes
- Use `python -m pip install pyserial` inside the Pi environment if `control_stackchan.py` needs dependencies.
- If the serial command returns `ERR`, check `Serial` log output via `screen /dev/ttyUSB0 115200` before clearing the terminal.
- Keep `docs/tests.md` current: add new rows for additional command sequences or failure conditions.

以上のガイドを教材として、必要に応じて他の開発者にも共有してください。修正点は Git コミットと `docs/journal.md` への記録を忘れずに。