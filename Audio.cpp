#include <M5Unified.h>
#include "Audio.h"

Audio::Audio() {
//  wavData = (typeof(wavData))heap_caps_malloc(record_size * sizeof(int16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT); //core2,coreS3等PSRAM有りのときはこちら
  wavData = (typeof(wavData))heap_caps_malloc(record_size * sizeof(int16_t),  MALLOC_CAP_8BIT); // capusule,AtomS3等PSRAM無しのときはこちら
  memset(wavData, 0 , record_size * sizeof(int16_t));
}

Audio::~Audio() {
   delete wavData;
}



void Audio::Record() {
  
  M5.Mic.begin();
  for (int rec_record_idx = 0; rec_record_idx < record_number; rec_record_idx++) {
    auto data = &wavData[rec_record_idx * record_length];
    M5.Mic.record(data, record_length, record_samplerate);
  }
  M5.Mic.end();

}

