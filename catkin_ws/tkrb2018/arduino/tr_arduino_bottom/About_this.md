## `tr_arduino_bottom`について
かなり雑です。
本戦終わり次第書き足します。
`tr_arduino_bottom`はロボットの下方についているarduino megaに書き込まれるべきスケッチです

### 主要な関数の一覧
`int* linesensorRawValue`  
ラインセンサの生の値  

`bool* linesensorValue`

`void linesensorMain()`  
loopの中に詰め込むやつ  

`void initLinesensor()`  
setupの中に詰め込むやつ

`void initDebug()`  
## `tr_arduino_bottom`について

`tr_arduino_bottom`はロボットの下方についているarduino megaに書き込まれるべきスケッチです

### 主要な関数の一覧
#### `linesensor.ino`の中身
`int* linesensorRawValue`  
ラインセンサの生の値  
`bool* linesensorValue`  

`void linesensorMain()`  
loopの中に詰め込むやつ  

`void initLinesensor()`  
setupの中に詰め込むやつ  

`void initDebug()`  
`void sendRawLinesensorValue()`
