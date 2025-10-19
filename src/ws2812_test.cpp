// #include <Arduino.h>
// #include "Dot2D/dot2d.h"
// #include "Button.h"
// #include <Adafruit_NeoMatrix.h>
// #include <Adafruit_NeoPixel.h>
// #include "Examples/Matrix/Matrix.h"
// #include "Examples/TripleClick/TripleClick.h"
// #include "Examples/Snake/Snake.h"


// #define MATRIX_WIDTH          32
// #define MATRIX_HEIGHT         8
// #define MATRIX_LED_PIN        37    // ✅ 改为 15，37 无法输出信号
// #define MATRIX_MAX_BRIGHTNESS 120   // ✅ 提高亮度



// dot2d::Director* director = nullptr;

// // ✅ 全局 matrix 对象
// Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
//   MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_LED_PIN,
//   NEO_MATRIX_TOP +      
//   NEO_MATRIX_LEFT +       
//   NEO_MATRIX_COLUMNS +  
//   NEO_MATRIX_PROGRESSIVE,
//   NEO_GRB + NEO_KHZ800
// );

// void click(void *btn)
// {
//     Button *button = (Button *)btn;
//     dot2d::EventButton event(button->type(), dot2d::EventButton::ButtonEventCode::BEC_CLICK);
//     auto dispatcher = dot2d::Director::getInstance()->getEventDispatcher();
//     dispatcher->dispatchEvent(&event);
// }

// //------------------------------------------
// class MainDelegate : public dot2d::DirectorDelegate
// {
//   void render() override {
//       static uint32_t lastMillis = 0;
//       if (millis() - lastMillis > 1000) {
//           Serial.println("render called");
//           lastMillis = millis();
//       }

//       for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
//           for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
//               auto c = _data[y * MATRIX_WIDTH + x];
//               matrix.drawPixel(x, y, matrix.Color(c.r, c.g, c.b));
//           }
//       }
//       matrix.show();
//   }

//   size_t write(uint8_t c) override
//   {
//       return Serial.write(c);
//   }

//   void initMatrix(dot2d::DTRGB *data) override {
//       _data = data;
//       matrix.begin();
//       matrix.setBrightness(MATRIX_MAX_BRIGHTNESS);
//       matrix.fillScreen(0);
//       Serial.println("Matrix initialized, data ptr:");
//       Serial.println((uintptr_t)_data, HEX);
//   }


//   uint32_t dotOrder(uint16_t x, uint16_t y) override
//   {
//       return y * MATRIX_WIDTH + x;
//   }

// private:
//   dot2d::DTRGB* _data = nullptr;
// };

// void setup() {
//   Serial.begin(9600);



//   // ✅ Dot2D 初始化
//   director = dot2d::Director::getInstance();
//   director->setDelegate(new MainDelegate());
//   director->setFramesPerSecond(30);
//   director->initDotCanvas(MATRIX_WIDTH, MATRIX_HEIGHT);

//   // ✅ 运行场景
//    director->runWithScene(dot2d::Matrix::create());
//   // director->runWithScene(dot2d::TripleClick::create());
//   //  director->runWithScene(dot2d::Snake::create());

//   Serial.println("Dot2D started.");
// }

// void loop() {
//   delay(10); // ✅ 改成 delay()


//   director->mainLoop();

// }
