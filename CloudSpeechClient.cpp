#include "CloudSpeechClient.h"
#include <ArduinoJson.h>
#include <M5Unified.h>

// --- AmiVoice API設定 ---
namespace {
constexpr char API_HOST[] = "acp-api.amivoice.com";
constexpr int API_PORT = 443;
constexpr char API_PATH[] = "/v1/recognize";
const char* ENGINE_GRAMMAR = "-a-general -l-LSB16K";
}

// コンストラクタ
CloudSpeechClient::CloudSpeechClient(const char* root_ca, const char* api_key) : client(), key(api_key) {
  client.setCACert(root_ca);
  client.setTimeout(15000); // タイムアウトを15秒に設定
}

// デストラクタ
CloudSpeechClient::~CloudSpeechClient() {
  client.stop();
}

// 音声認識処理
String CloudSpeechClient::Transcribe(Audio* audio) {
  if (!client.connect(API_HOST, API_PORT)) {
    M5.Displays(0).clear();
    M5.Displays(0).setCursor(0, 0);
    M5.Displays(0).print("Connection Failed!");
    delay(4000);
    return "";
  }

  String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";

  // 音声データより前の部分 (パート u, d と パート a のヘッダー)
  String body_prefix = "--" + boundary + "\r\n" +
                     "Content-Disposition: form-data; name=\"u\"\r\n\r\n" +
                     key + "\r\n" +
                     "--" + boundary + "\r\n" +
                     "Content-Disposition: form-data; name=\"d\"\r\n\r\n" +
                     ENGINE_GRAMMAR + "\r\n" +
                     "--" + boundary + "\r\n" +
                     "Content-Disposition: form-data; name=\"a\"; filename=\"audio.raw\"\r\n" +
                     "Content-Type: application/octet-stream\r\n\r\n";

  // 音声データより後の部分 (終端バウンダリ)
  String body_suffix = "\r\n--" + boundary + "--\r\n";

  long contentLength = body_prefix.length() + audio->wavDataSize + body_suffix.length();


  // --- HTTPヘッダーを送信 ---
  client.println(String("POST ") + API_PATH + " HTTP/1.1");
  client.println(String("Host: ") + API_HOST);
  client.println("Connection: close");
  client.println("Content-Length: " + String(contentLength));
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println();
  client.flush(); // ヘッダーを確実に送信

  // --- ボディパートを送信 ---
  // プレフィックス部を送信
  client.print(body_prefix);
  client.flush();

  uint8_t* audio_ptr = (uint8_t*)audio->wavData;
  size_t remaining_size = audio->wavDataSize;
  size_t chunk_size = 1024; // 一度に送信するサイズ (調整可能)

  while (remaining_size > 0) {
    size_t size_to_send = (remaining_size < chunk_size) ? remaining_size : chunk_size;
    client.write(audio_ptr, size_to_send);
    client.flush(); // 分割したデータを確実に送信
    
    audio_ptr += size_to_send;
    remaining_size -= size_to_send;
  }

  // サフィックス部を送信
  client.print(body_suffix);
  client.flush();


  unsigned long startTime = millis();
  while (client.connected() && !client.available()) {
    if (millis() - startTime > 10000) { // タイムアウトを10秒に短縮
      M5.Displays(0).clear();
      M5.Displays(0).setCursor(0, 0);
      M5.Displays(0).print("Client Timeout!");
      delay(4000);
      client.stop();
      return "";
    }
    delay(100);
  }

  // HTTPレスポンスヘッダーを読み飛ばす
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  // JSONボディをパース
  String result = "";
  if (client.available()) {
    StaticJsonDocument<4096> jsonBuffer;//STTしきれない長文はここの数値を大きくとる
    DeserializationError error = deserializeJson(jsonBuffer, client);

    if (error) {
    //  M5.Displays(0).clear();
    //  M5.Displays(0).setCursor(0, 0);
    //  M5.Displays(0).println("JSON Parse Err");
    //  M5.Displays(0).println(error.c_str());
      
      // 生のレスポンスを表示してみる（デバッグ用）
      // String raw_response = client.readString();
      // M5.Displays(0).println(raw_response);

    //  delay(5000);
    } else {
      const char* text = jsonBuffer["text"];
      if (text && strlen(text) > 0) {
        result = String(text);
    //    M5.Displays(0).clear();
    //    M5.Displays(0).setCursor(0, 0);
    //    M5.Displays(0).println("Success! Result:");
    //    M5.Displays(0).print(result);
      } else {
        // APIからエラーメッセージが返された場合
    //    M5.Displays(0).clear();
    //    M5.Displays(0).setCursor(0, 0);
    //    M5.Displays(0).println("AmiVoice Result:");
    //    M5.Displays(0).print(jsonBuffer["message"] | "Recog failed");
      }
    //  delay(5000);
    delay(10);
    }
  }
  
  client.stop();
  return result;
}