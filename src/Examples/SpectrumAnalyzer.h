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

#ifndef __SpectrumAnalyzer_H__
#define __SpectrumAnalyzer_H__

#include "Dot2D/dot2d.h"

NS_DT_BEGIN

class SpectrumAnalyzer : public Scene
{
protected:
    bool init() override;

public:
    STATIC_CREATE(SpectrumAnalyzer);
};

class SpectrumAnalyzerLayer : public Layer
{
private:
    CanvasSprite* canvasSprite;
    float timeCounter;
    float frequencyBands[8];        // 8个频段对应8行LED
    float peakLevels[8];           // 峰值保持
    float decayRates[8];           // 各频段衰减率
    float audioInput[32];          // 模拟音频输入缓冲区
    float previousBands[8];        // 上一帧的频段值（用于平滑）
    
    // Goertzel算法参数
    float goertzelValues[8];
    float samplingRate;
    
    void simulateAudioInput();
    void processGoertzelAlgorithm();
    void updatePeakDetection();
    DTRGB getFrequencyColor(int band, float intensity);
    DTRGB hsvToRgb(float h, float s, float v);
    
public:
    virtual ~SpectrumAnalyzerLayer();
    virtual bool initLayer();
    virtual void update(float dt) override;

    STATIC_CREATE(SpectrumAnalyzerLayer);
};

NS_DT_END

#endif //__SpectrumAnalyzer_H__