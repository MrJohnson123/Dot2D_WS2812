/****************************************************************************
Copyright (c) 2024      AI Assistant

https://www.example.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "SpectrumAnalyzer.h"
#include <math.h>

NS_DT_BEGIN

bool SpectrumAnalyzer::init()
{
    SpectrumAnalyzerLayer *rootLayer = SpectrumAnalyzerLayer::create();
    rootLayer->setContentSize(Size(32,8));
    this->addChild(rootLayer);
    rootLayer->initLayer();
    return true;
}

SpectrumAnalyzerLayer::~SpectrumAnalyzerLayer()
{
}

bool SpectrumAnalyzerLayer::initLayer()
{
    Size size = this->getContentSize();
    
    canvasSprite = CanvasSprite::create(size.width, size.height);
    this->addChild(canvasSprite);
    
    // 初始化频谱参数
    samplingRate = 44100.0f;
    
    for (int i = 0; i < 8; i++) {
        frequencyBands[i] = 0.0f;
        peakLevels[i] = 0.0f;
        decayRates[i] = 0.93f + (i * 0.01f);
        goertzelValues[i] = 0.0f;
        previousBands[i] = 0.0f;
    }
    
    timeCounter = 0;
    this->scheduleUpdate();
    
    return true;
}

DTRGB SpectrumAnalyzerLayer::hsvToRgb(float h, float s, float v)
{
    // HSV转RGB函数
    float r, g, b;
    
    h = fmodf(h, 360.0f);
    if (h < 0) h += 360.0f;
    
    s = fmaxf(0.0f, fminf(1.0f, s));
    v = fmaxf(0.0f, fminf(1.0f, v));
    
    int i = (int)(h / 60.0f) % 6;
    float f = (h / 60.0f) - (int)(h / 60.0f);
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    
    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: r = v; g = t; b = p;
    }
    
    return DTRGB((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255));
}

void SpectrumAnalyzerLayer::simulateAudioInput()
{
    // 模拟音频输入信号（多种频率合成）
    for (int i = 0; i < 32; i++) {
        float t = timeCounter + i * 0.01f;
        
        // 生成多频率合成信号（模拟音乐）
        float signal = 
            sinf(t * 50.0f) * 0.3f +      // 低频 50Hz
            sinf(t * 200.0f) * 0.4f +     // 中低频 200Hz
            sinf(t * 800.0f) * 0.5f +     // 中频 800Hz
            sinf(t * 2000.0f) * 0.6f +    // 中高频 2kHz
            sinf(t * 5000.0f) * 0.7f +    // 高频 5kHz
            sinf(t * 10000.0f) * 0.4f +   // 超高频 10kHz
            (random(0, 100) / 100.0f - 0.5f) * 0.2f; // 噪声
        
        // 添加节奏脉冲（模拟鼓点）
        if (fmodf(t, 0.5f) < 0.05f) {
            signal += sinf(t * 100.0f) * 0.8f;
        }
        
        audioInput[i] = signal;
    }
}

void SpectrumAnalyzerLayer::processGoertzelAlgorithm()
{
    // 简化的Goertzel算法实现（针对8个频段）
    float targetFrequencies[8] = {50, 150, 400, 800, 1600, 3200, 6400, 12800};
    
    for (int band = 0; band < 8; band++) {
        float freq = targetFrequencies[band];
        float k = (int)(0.5f + (32 * freq) / samplingRate);
        float w = 2.0f * 3.14159f * k / 32;
        float cosine = cosf(w);
        float coeff = 2.0f * cosine;
        
        float q0 = 0, q1 = 0, q2 = 0;
        
        for (int n = 0; n < 32; n++) {
            q0 = coeff * q1 - q2 + audioInput[n];
            q2 = q1;
            q1 = q0;
        }
        
        // 计算能量值
        float real = q1 - q2 * cosine;
        float imag = q2 * sinf(w);
        float magnitude = real * real + imag * imag;
        
        // 更新频段值（平滑过渡）
        frequencyBands[band] = frequencyBands[band] * 0.7f + sqrtf(magnitude) * 0.3f;
    }
}

void SpectrumAnalyzerLayer::updatePeakDetection()
{
    for (int band = 0; band < 8; band++) {
        // 峰值保持和衰减
        if (frequencyBands[band] > peakLevels[band]) {
            peakLevels[band] = frequencyBands[band];
        } else {
            peakLevels[band] *= decayRates[band]; // 缓慢衰减
        }
        
        // 防止峰值过低
        if (peakLevels[band] < 0.01f) {
            peakLevels[band] = 0.01f;
        }
    }
}

void SpectrumAnalyzerLayer::update(float dt)
{
    Size size = this->getContentSize();
    SpriteCanvas* canvas = canvasSprite->getSpriteCanvas();
    canvas->canvasReset();
    
    timeCounter += dt;
    
    // 1. 模拟音频处理流程
    simulateAudioInput();
    processGoertzelAlgorithm();
    updatePeakDetection();
    
    // 新增：平滑处理，让频谱变化更自然
    for (int band = 0; band < 8; band++) {
        // 应用低通滤波器，减少突变
        frequencyBands[band] = frequencyBands[band] * 0.7f + (previousBands[band] * 0.3f);
        previousBands[band] = frequencyBands[band];
    }
    
    // 2. 绘制频谱（紧密排列版本）
    for (int band = 0; band < 8; band++) {
        // 计算强度和峰值（归一化）
        float intensity = fminf(frequencyBands[band] / 5.0f, 1.0f);
        float peakIntensity = fminf(peakLevels[band] / 5.0f, 1.0f);
        
        // 计算频谱高度（0-8）
        int barHeight = (int)(intensity * 8);
        int peakHeight = (int)(peakIntensity * 8);
        
        // 确保在范围内
        barHeight = fmaxf(fminf(barHeight, 8), 0);
        peakHeight = fmaxf(fminf(peakHeight, 8), 0);
        
        // *** 关键修改：每个频段占3像素宽，紧密排列 ***
        int startX = band * 4; // 频段起始位置（32像素/8频段=每个4像素空间）
        int barWidth = 3;      // 频谱条宽度增加到3像素（更饱满）
        
        // 为当前频段生成颜色（HSV色彩空间）
        float hue = 210.0f + (band * 25.0f); // 蓝色基调，不同频段略有偏移
        float saturation = 0.8f + (intensity * 0.2f);
        float value = intensity;
        
        DTRGB baseColor = hsvToRgb(hue, saturation, value);
        
        // a. 绘制主频谱条（带有垂直渐变）
        for (int y = 0; y < barHeight; y++) {
            // 计算当前行的亮度因子（底部亮，顶部暗）
            float rowFactor = (float)(y + 1) / barHeight;
            
            // 创建渐变颜色
            DTRGB gradientColor = baseColor;
            gradientColor.r = (uint8_t)(gradientColor.r * rowFactor * 0.8f);
            gradientColor.g = (uint8_t)(gradientColor.g * rowFactor);
            gradientColor.b = (uint8_t)(gradientColor.b * rowFactor * 1.2f);
            
            // 绘制频谱条的每一行（3像素宽）
            for (int w = 0; w < barWidth; w++) {
                int pixelX = startX + w;
                int pixelY = 7 - y; // LED矩阵从底部开始绘制
                
                if (pixelX >= 0 && pixelX < size.width && pixelY >= 0 && pixelY < size.height) {
                    canvas->writePixel(pixelX, pixelY, gradientColor);
                }
            }
        }
        
        // b. 绘制峰值保持线（带有缓慢衰减）
        if (peakHeight > 0) {
            DTRGB peakColor = hsvToRgb(60, 1.0f, 1.0f); // 亮黄色峰值线
            
            // 峰值线在达到新高度时闪烁一下
            float blink = (sinf(timeCounter * 20.0f) + 1.0f) * 0.5f;
            peakColor.g = (uint8_t)(peakColor.g * (0.7f + 0.3f * blink));
            
            for (int w = 0; w < barWidth; w++) {
                int pixelX = startX + w;
                int peakY = 7 - peakHeight;
                
                if (pixelX >= 0 && pixelX < size.width && peakY >= 0 && peakY < size.height) {
                    canvas->writePixel(pixelX, peakY, peakColor);
                }
            }
            
            // 峰值点以上的轻微光晕效果
            if (peakHeight < 7) {
                DTRGB glowColor = peakColor;
                glowColor.r = glowColor.r / 4;
                glowColor.g = glowColor.g / 4;
                glowColor.b = glowColor.b / 4;
                
                int glowY = 7 - (peakHeight + 1);
                if (glowY >= 0 && glowY < size.height) {
                    canvas->writePixel(startX, glowY, glowColor);
                    if (barWidth > 1) {
                        canvas->writePixel(startX + 1, glowY, glowColor);
                        canvas->writePixel(startX + 2, glowY, glowColor);
                    }
                }
            }
        }
        
        // c. 底部光晕效果（增强立体感）
        if (barHeight > 0) {
            DTRGB glowColor = baseColor;
            glowColor.r = glowColor.r / 3;
            glowColor.g = glowColor.g / 3;
            glowColor.b = glowColor.b / 3;
            
            int bottomY = 7; // 最底部一行
            for (int w = 0; w < barWidth; w++) {
                int pixelX = startX + w;
                if (pixelX >= 0 && pixelX < size.width) {
                    // 基础光晕
                    canvas->writePixel(pixelX, bottomY, glowColor);
                }
            }
        }
        
        // *** 移除：频段间的分隔线代码 ***
        // 现在频谱条紧密排列，不需要绿色分隔线
    }
    
    // 3. VU表效果优化（右侧整体音量指示）
    float overallVolume = 0;
    for (int band = 0; band < 8; band++) {
        overallVolume += frequencyBands[band];
    }
    overallVolume /= 8.0f;
    overallVolume = fminf(overallVolume / 3.0f, 1.0f);
    
    int vuHeight = (int)(overallVolume * 8);
    for (int y = 0; y < vuHeight; y++) {
        int pixelX = 31;
        int pixelY = 7 - y;
        
        // VU表颜色渐变（绿-黄-红）
        DTRGB vuColor;
        float heightRatio = (float)(y + 1) / vuHeight;
        if (heightRatio < 0.6f) {
            // 绿色到黄色渐变
            float t = heightRatio / 0.6f;
            vuColor = DTRGB((uint8_t)(t * 255), 255, 0);
        } else {
            // 黄色到红色渐变
            float t = (heightRatio - 0.6f) / 0.4f;
            vuColor = DTRGB(255, (uint8_t)((1.0f - t) * 255), 0);
        }
        
        if (pixelX >= 0 && pixelX < size.width && pixelY >= 0 && pixelY < size.height) {
            canvas->writePixel(pixelX, pixelY, vuColor);
        }
    }
    
    // 4. 随机脉动效果（模拟音频动态）
    if (random(0, 100) < 20) { // 20%概率每帧添加随机脉动
        int randomBand = random(0, 8);
        float pulseIntensity = random(30, 80) / 100.0f;
        
        // 短暂增强该频段的显示
        frequencyBands[randomBand] = frequencyBands[randomBand] * (1.0f + pulseIntensity);
    }
}

NS_DT_END