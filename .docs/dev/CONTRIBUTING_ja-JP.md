# ClearBox へのコントリビューション

ご協力ありがとうございます！ClearBox は主に一人でメンテナンスされているプロジェクトです（少なくとも今のところは :-) ）。どんな助けでも——設定ファイルの 1 行でも——開発者は感謝します。
ご自身の得意分野に合わせて、以下の参加方法を選んでください。**最初の 2 つはコードを書く必要はありません。**



## コントリビューションの方法

| 方向 | 作業内容 | 最終的な行き先 |
|---|---|---|
| 設定 | クリーンアップ/整理/自動化ルールを書いて共有 | 内蔵設定またはコミュニティのルールセット |
| 翻訳 | 多言語の翻訳文字列を改善 | `.docs/README_*.md` またはアプリ / TUI 内のテキスト |
| テスト | お使いの端末/ROM でビルドし、バグを報告 | Issues または Telegram グループ |
| コード | C コア、Kr-Script アプリ、ビルドスクリプトの開発 | Pull Requests & Push |



### 設定（ルールの作成とデバッグ）

以下はすべてプレーンテキストです——`.txt` を編集できれば、あなたにも貢献できます：

プリセット設定：
- `AppSource/clearbox/src/main/assets/usr/Timed_xml/` — 自動化タスク
- `FileConfigs/` — ファイルのクリーンアップ / 分類ルール
- `AppConfigs/` — アプリのプライベートストレージルール

カスタム設定：
- `/data/adb/wipe_cache/CleanConfigs` — 乱れたディレクトリのクリーンアップルール
- `/data/adb/wipe_cache/BindConfigs` — ストレージリダイレクトルール

構文の参考：README の各セクションの説明をご覧ください。守るべきルール：

- PR 1 件（またはファイル 1 つ）= ルールファイル 1 つ。クリーンアップ対象の名前で命名してください
- 一部のルールは相対パスを使用します。ルート `/xxx` で始まるパスを**ハードコードしないで**ください
- `#` コメントはできるだけ行を丸ごと占有させてください（行末の `#ext` インラインコメントが唯一の例外です）
- ご自身の端末でルールをテストし、PR にテスト端末 / ROM / Android バージョンを添付してください。
- プリセット設定は上記のプリセットディレクトリにコミットし、**PR を開いて**ください
- カスタム設定はコミュニティや Telegram グループで共有することをおすすめします



### 翻訳（文字列の改善）

タイポ、不自然な表現、未翻訳の文字列を見つけましたか？PR 大歓迎です。
レビューを速くするため、PR ごとに**1 言語のみ**の変更をお願いします。
プロジェクトには現在以下の言語があります。得意な言語を選んでください：
- **`en-US / zh-CN / zh-TW / ru-RU / ko-KR / ja-JP`**
（あれ、見つからない :-)？では、その時はお願いするしかありませんね）



### テスト（機能改善とバグ報告）

問題を報告する際は、以下を含めてください：

- 端末と Android バージョン：
- ROM（MIUI / HyperOS / AOSP...）
- Root 方式：Magisk / APatch / KernelSU + バージョン
- ClearBox のバージョン（Version Code）
- ファイルシステム（分かれば）：f2fs / ext4 / sdcardfs
- 実際に起きたこと vs 期待した動作
- ターミナル出力またはログ（あれば）



### コード（機能追加 / バグ修正）

- **NDK** を設定し、環境変数 **ANDROID_NDK_HOME** に **NDK** のルートディレクトリパスを設定してください
- **App** のコード開発とデバッグに深く関わる場合は、**Android SDK** 環境も設定してください
- 以下は基本的な環境設定の例です：

基本パッケージツール

``` shell
# Debian、Ubuntu 系
sudo apt install zip make build-essential

# Arch 系
sudo pacman -S zip make base-devel

# Alpine 系
sudo apk add zip make build-base

# Fedora 系
sudo dnf install zip make
sudo dnf groupinstall "Development Tools"
```

プロジェクトの取得（参考用——PR するなら自分で Fork してください）

``` shell
git clone https://github.com/FLYCOM-E/ClearBox
cd ClearBox/ && chmod +x *

export TARGET_API=23         # 必要に応じてターゲット API バージョンを調整
export TARGET_ABI=aarch64   # 選択可能: aarch64、armv7a、x86_64、riscv64
```

コアをコンパイル

``` shell
./build.sh       # APK をコンパイルする場合: --build-apk
```

コミット規約：既存のスタイルを維持してください: `Fix: ...` / `Update: ...` / `Add: ...`。コミット 1 件につき関連する変更 1 つにし、複数の変更がある場合は改行を入れてください。
PR 要件：PR 1 件 = 機能 1 つ、または修正 1 つ。何を変えたか、そして**なぜ**変えたのかを説明してください（必要な場合。実際に遭遇した失敗事例が最良の「なぜ」です）。



## 🚫 おやめください

- 上記の端末情報なしに「動かない」という issue は提出しないでください。情報がなければ、メンテナーは再現すらできないかもしれません
- 機能アイデア：大量のコードを書く前に、まず Issue を作成して議論してください。時間の無駄を防ぐためです
- フォーマットの調整とロジックの変更を同じコミットに混ぜないでください
- テストしていない「正規表現の推測」ルールをコミットしないでください



## ✍️ ディスカッションの場

- Issues：**バグ** と機能の追跡用
- Telegram（最も活発）：[**Group**](https://t.me/clearbox_group)



## ❤️ 謝辞

このプロジェクトは [GPL-3.0](https://github.com/FLYCOM-E/ClearBox/blob/main/LICENSE) でライセンスされています ~ 貢献すると、あなたの成果が同じライセンスで配布されることに同意したことになります。
貢献者はここに記載されます。あなたの名前がこのリストの先頭に来るかもしれません。



### 以下の貢献者に感謝します：

[**Amktiao**](https://www.coolapk.com/u/925348): F2FS-GC の原理を間接的に提供
[**Drsexo**](https://github.com/Drsexo): Github Action の設定を追加
[**Creeper005bili**](https://github.com/Creeper005bili): モジュールの初期テストを支援
