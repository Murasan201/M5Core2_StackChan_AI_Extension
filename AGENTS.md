# AGENTS.md — Guidance for helpers

## リポジトリ概要
- このリポジトリは [M5Core2_SG90_StackChan_VoiceText_Ataru](https://github.com/robo8080/M5Core2_SG90_StackChan_VoiceText_Ataru) をベースに Murasan201 氏がフォークしたスタックちゃん（M5Stack Core2）向けファームウェアです。
- `docs/interface-requirements.md` に現在進行中の「Pi から JSON コマンドで表情と吹き出しを操作するインタフェース」仕様をまとめています。
- `docs/development-environment.md` にはこの Raspberry Pi 5 環境でのビルド/書き込み手順とシリアルポート構成 (`/dev/ttyUSB0`, CP210x) を記載しています。

## 開発前提
- 開発ホスト：Raspberry Pi 5（OpenClaw workspace）。USB 経由で M5Stack Core2 と接続され、`lsusb` では `10c4:ea60 Silicon Labs CP210x UART Bridge` が `/dev/ttyUSB0` にマッピングされています。
- 開発ツール：`arduino-cli`（ESP32 コアを `esp32:esp32` のようにインストール済み）を使った CLI ビルド。ライブラリは `M5Unified`, `M5Stack-Avatar`, `ServoEasing`, `ESP32Servo`, `ESP8266Audio` など。

## 進行中の仕事と方針
1. Phase1〜2：要件/計画整備（完了）。要件書と計画ファイルに記録済み。\
2. 今後：Core2 側で JSON パーサを実装し、Pi 側で `control_stackchan.py` などの CLI を作る予定。\
3. 現バージョンではボタン (M5.BtnA/B/C) による挙動は不要。
4. シリアル/Avatar API とのやり取りを中心に進めてください。`Avatar` は `m5avatar::Expression` + `setSpeechText()` + `setFace()` などで制御できます。

## 追加メモ
- 構想の中心は newline-delimited JSON コマンド（`expression`, `speech`, `face`, `palette`, `duration`, `clear`）を Pi から送り、Core2 が `OK`/`ERR` で応答すること。詳細は `docs/interface-requirements.md`。
- 変更を加えたら `task_plan.md`/`progress.md`/`findings.md` に加え、`docs/journal.md`（作業ログ）と `docs/troubleshooting.md`（エラー/対処）も随時更新してください。トラブルシューティングは重要なナレッジなので漏れなく記録すること。
