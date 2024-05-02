#include <chrono>
#include "raylib.h"
#include <cstdint>
#include <limits>
#include <atomic>
#include <mutex>
#include <AudioFile.h>
#include "engine/synth.h"
#include "utli/Keyboard.hpp"
#include "layout/SynthLayout.h"

static mana::Synth synth_;
static mana::KeyBoard keyboard_;
static mana::SynthLayout synth_layout_{ synth_ };
static struct spinlock {
    std::atomic<bool> lock_ = { 0 };

    void lock() noexcept {
        for (;;) {
            // Optimistically assume the lock is free on the first try
            if (!lock_.exchange(true, std::memory_order_acquire)) {
                return;
            }
            // Wait for lock to be released without generating cache misses
            while (lock_.load(std::memory_order_relaxed)) {
                // Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
                // hyper-threads
                //__builtin_ia32_pause();
            }
        }
    }

    bool try_lock() noexcept {
        // First do a relaxed load to check if lock is free in order to prevent
        // unnecessary cache misses if someone does while(!try_lock())
        return !lock_.load(std::memory_order_relaxed) &&
            !lock_.exchange(true, std::memory_order_acquire);
    }

    void unlock() noexcept {
        lock_.store(false, std::memory_order_release);
    }
} audio_lock_;

static void ThisAudioCallback(void* buffer, unsigned int frames) {
    std::scoped_lock _{ audio_lock_ };

    synth_.update_state(frames);
    synth_.Render(frames);
    std::ranges::copy(synth_.getBuffer(), static_cast<float*>(buffer));
    synth_layout_.GetOscilloscope().PushBuffer(synth_.getBuffer());
}

int main(void) {
    InitWindow(800, 600, "additive synth");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(480);
    AudioStream stream = LoadAudioStream(48000, 32, 1);
    SetAudioStreamCallback(stream, ThisAudioCallback);

    PlayAudioStream(stream);        // Start processing stream buffer (no data loaded currently)
    SetTargetFPS(30);               // Set our game to run at 30 frames-per-second

    // init synth
    synth_.Init(480, 48000.0f);
    synth_layout_.SetBounds(0, 0, 800, 600);
    synth_.CreateResynthsisFrames(AudioFile<float>{R"(C:\Users\Kawai\Desktop\o.wav)"}.samples.front());

    // link keyboard and synth
    keyboard_.onNoteOn = [](int n) {synth_.NoteOn(n, 1.0f); };
    keyboard_.onNoteOff = [](int n) {synth_.NoteOff(n, 1.0f); };

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // drawing
        BeginDrawing();
        ClearBackground(BLACK);
        {
            std::scoped_lock _{ audio_lock_ };

            keyboard_.checkInFrame();
            synth_layout_.paint();

            const auto& partials = synth_.GetDisplayOscillor().GetPartials();
            for (size_t i = 0; i < mana::kNumPartials; ++i) {
                auto x_nor = partials.pitches[i] / 140.0f;
                auto y_nor = 0.0f;
                auto y_gain = std::abs(partials.gains[i]);
                if (y_gain >= 0.00001f) {
                    auto y_del = std::clamp(20.0f * std::log10(y_gain), -60.0f, 20.0f);
                    y_nor = (y_del + 60.0f) / 80.0f;
                }
                int x = static_cast<int>(800 * x_nor);
                int y = static_cast<int>(600 * (1.0f - y_nor));
                DrawLine(x, y, x, 600, GREEN);
            }
        }
        EndDrawing();
    }

    UnloadAudioStream(stream);   // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

    CloseWindow();              // Close window and OpenGL context

    return 0;
}