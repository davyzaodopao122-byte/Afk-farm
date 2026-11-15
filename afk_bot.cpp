// afk_bot.cpp — C++17 · Windows 11 · MSVC · bot AFK + auto-queue + stats

#include <windows.h>
#include <random>
#include <thread>
#include <chrono>
#include <atomic>

std::mt19937 rng{std::random_device{}()};
std::atomic<bool> autofarm{false};
std::atomic<bool> running{true};

int rr(int lo, int hi) {
    std::uniform_int_distribution<int> d(lo, hi);
    return d(rng);
}

void key_down(WORD vk) { INPUT i{}; i.type=INPUT_KEYBOARD; i.ki.wVk=vk; SendInput(1,&i,sizeof(INPUT)); }
void key_up(WORD vk)   { INPUT i{}; i.type=INPUT_KEYBOARD; i.ki.wVk=vk; i.ki.dwFlags=KEYEVENTF_KEYUP; SendInput(1,&i,sizeof(INPUT)); }

void hold(WORD vk, int ms) {
    key_down(vk);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    key_up(vk);
}

void move_mouse(int dx, int dy) {
    INPUT i{}; i.type=INPUT_MOUSE; i.mi.dx=dx; i.mi.dy=dy; i.mi.dwFlags=MOUSEEVENTF_MOVE;
    SendInput(1,&i,sizeof(INPUT));
}

void click_at(int x, int y) {
    SetCursorPos(x, y);
    std::this_thread::sleep_for(std::chrono::milliseconds(rr(80,180)));
    INPUT d{}; d.type=INPUT_MOUSE; d.mi.dwFlags=MOUSEEVENTF_LEFTDOWN; SendInput(1,&d,sizeof(INPUT));
    std::this_thread::sleep_for(std::chrono::milliseconds(rr(60,140)));
    INPUT u{}; u.type=INPUT_MOUSE; u.mi.dwFlags=MOUSEEVENTF_LEFTUP; SendInput(1,&u,sizeof(INPUT));
}

void auto_queue() {
    click_at(1780, 40);
    std::this_thread::sleep_for(std::chrono::milliseconds(rr(1500,2500)));
    click_at(960, 540);
    std::this_thread::sleep_for(std::chrono::milliseconds(rr(800,1500)));
    click_at(960, 900);
    std::this_thread::sleep_for(std::chrono::milliseconds(rr(3000,5000)));
}

void movement_cycle() {
    WORD dirs[] = { 'W', 'A', 'S', 'D' };
    WORD dir = dirs[rr(0,3)];
    hold(dir, rr(800, 2500));
    std::this_thread::sleep_for(std::chrono::milliseconds(rr(300, 1200)));

    move_mouse(rr(-400,400), rr(-100,100));

    if (rr(0,100) < 25) hold(VK_SPACE, rr(60,150));
    if (rr(0,100) < 15) hold(VK_CONTROL, rr(200,600));

    if (rr(0,100) < 10) {
        INPUT d{}; d.type=INPUT_MOUSE; d.mi.dwFlags=MOUSEEVENTF_LEFTDOWN; SendInput(1,&d,sizeof(INPUT));
        std::this_thread::sleep_for(std::chrono::milliseconds(rr(80,200)));
        INPUT u{}; u.type=INPUT_MOUSE; u.mi.dwFlags=MOUSEEVENTF_LEFTUP; SendInput(1,&u,sizeof(INPUT));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(rr(2000, 6000)));
}

int main() {
    RegisterHotKey(nullptr, 1, 0, VK_F8);
    RegisterHotKey(nullptr, 2, 0, VK_F12);

    std::thread worker([]{
        while (running) {
            if (autofarm) {
                movement_cycle();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
        }
    });

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            if (msg.wParam == 1) autofarm = !autofarm;
            if (msg.wParam == 2) { running = false; break; }
        }
    }
    running = false;
    worker.join();
    return 0;
}
