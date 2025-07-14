#ifndef CLOUDSPEECHCLIENT_H_
#define CLOUDSPEECHCLIENT_H_

#include <WiFiClientSecure.h>
#include "Audio.h" 

class CloudSpeechClient {
 public:
  // コンストラクタ: ルート証明書とAPIキーを受け取る
  CloudSpeechClient(const char* root_ca, const char* api_key);
  ~CloudSpeechClient();
  String Transcribe(Audio* audio);

 private:
  WiFiClientSecure client;
  String key; // APIキーを保持するメンバー変数をString型に変更
  //const char* key; // APIキーを保持するメンバー変数
};

#endif  // CLOUDSPEECHCLIENT_H_
