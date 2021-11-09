//
// Created by Jonathan on 09.11.2021.
//

#include "win32_audio.h"
#include <cmath>

#define M_PI 3.14159265358979323846

i16 sineWave(double time, double freq, double amplitude) {
    i16 result;
    double tpc = 44100 / freq;
    double cycles = time / tpc;
    double rad = M_PI * 2 * cycles;
    i16 _amp = 32767 * amplitude;
    result = _amp  * sin(rad);
    return result;
}
void fillBuffer(i16* buffer, size_t len, int sampleRate, int freq) {
    for(int i = 0; i<len - 1; i+=2) {
        //This should give us a middle C4 (for reference: https://www.youtube.com/watch?v=t8ZE1Mlkg2s)
        i16 currentSample = sineWave(i, 261.6, 0.9);
        buffer[i] = currentSample;
        buffer[i+1] = currentSample;
    }
}

void sineWavePlaybackExample() {
    size_t bufferLen = 44100 * 2 * 8;
    i16* buffer = static_cast<i16 *>(malloc(bufferLen * sizeof(i16)));
    fillBuffer(buffer, bufferLen, 44100, 10000);

    PlatformWin32::AudioPlaybackContext context {0};
    PlatformWin32::setupAudioPlayback(true, nullptr, &context);
    PlatformWin32::PCMAudioBufferInfo bufferInfo {0};
    bufferInfo.bufferSize = bufferLen;
    bufferInfo.rawDataBuffer = reinterpret_cast<u8 *>(buffer);

    WAVEFORMATEX wf {0};
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nSamplesPerSec = 44100;
    wf.wBitsPerSample = 16;
    wf.nChannels = 2;
    wf.nBlockAlign = (wf.nChannels * wf.wBitsPerSample) / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    wf.cbSize = 0;

    bufferInfo.waveformat = wf;
    PlatformWin32::AudioHandle audioHandle {0};

    PlatformWin32::submitSoundBuffer(&context, &bufferInfo, &audioHandle);
    PlatformWin32::playAudioBuffer(&context, &audioHandle, true);
}

void opusPlaybackExample() {
    PlatformWin32::AudioPlaybackContext context {0};
    PlatformWin32::setupAudioPlayback(true, nullptr, &context);

    PlatformWin32::VorbisDecoderFileApi opusAPI;
    PlatformWin32::PCMAudioBufferInfo bufferInfo {0};
    PlatformWin32::decodeVorbisFile(&opusAPI, L"allTheTime.ogg", &bufferInfo);

    PlatformWin32::AudioHandle audioHandle {0};
    PlatformWin32::submitSoundBuffer(&context, &bufferInfo, &audioHandle);

    PlatformWin32::playAudioBuffer(&context, &audioHandle, true);

}
int WINAPI WinMain(
        _In_ HINSTANCE  hinstance,
        _In_ HINSTANCE  hPrevInstance,
        _In_ LPSTR      lpCmdLine,
        _In_ int        nShowCmd
        ) {

    OutputDebugStringW(L"Moin!\n");
    //Still kinda broken but who cares
    //sineWavePlaybackExample();
    opusPlaybackExample();

    for(;;) {
        Sleep(100);
    }
}