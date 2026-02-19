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
- [x] Fixed `WiFiClient`/`DrawContext` build errors by gating VoiceText and refactoring the Ataru/Ram helpers.
- [x] Confirmed CLI handshake recovery via `control_stackchan.py` (longer delay, flush input, no forced DTR) and USB serial logging.

## 4. 今後の手順（Next steps）
1. **Verify display behavior**：`control_stackchan.py` の `--expression`/`--duration` コマンドで Core2 の顔と吹き出しが期待どおり変化しているかを確認し、起動直後の `Serial` 出力と合わせて記録。
2. **Document bubble timing**：吹き出しの自動消去や `duration` の反応にばらつきがあれば `docs/troubleshooting.md` へ記録し、必要なら `control_stackchan.py` の `--delay` やスクリプト側の再送タイミングを調整。
3. **Hands-off share**：このガイド/`docs/interface-requirements.md`/`docs/tests.md` に手順・コマンド・確認事項をまとめ、将来の担当者が同じ順序で再現できるようにする。

## 5. Tips & Notes
- Use `python -m pip install pyserial` inside the Pi environment if `control_stackchan.py` needs dependencies.
- If the serial command returns `ERR`, check `Serial` log output via `screen /dev/ttyUSB0 115200` before clearing the terminal.
- Keep `docs/tests.md` current: add new rows for additional command sequences or failure conditions.

以上のガイドを教材として、必要に応じて他の開発者にも共有してください。修正点は Git コミットと `docs/journal.md` への記録を忘れずに。