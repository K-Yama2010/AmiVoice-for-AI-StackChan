# AmiVoice-for-AI-StackChan


AI StackChanでの音声認識で、 AmiVoiceAPI を利用するときのサンプルプログラムです。




<br>


Audio.cpp　、CloudSpeechClient.cpp　、CloudSpeechClient.h　は書き換え、rootCAamivoice.h　は追加してください。



<br>


また、下記画像のようにmain.cpp内を書き換えてください。


（以上の変更は、あくまでサンプルとしてですので最小限の変更にて動作するような変更です。googleSTTと併用される場合は、CloudSpeechClient.cpp　、CloudSpeechClient.h　の名前は変更し、main.cppもその名前に対応した変更を行ってください。）


<br>


![](images/main1.jpg)




<br>


![](images/main2.jpg)




<br>

補足ですが、AmiVoice API( https://acp.amivoice.com/ ) でAPI（APP）キーを作る際は下記画像のようにしてください。

<br>



![](images/Ami-API-entry.jpg)
