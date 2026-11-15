// menu.cpp — C++17 · Windows 11 · MSVC · menu overlay separado + stats
// Compila: cl /std:c++17 /O2 menu.cpp /Fe:menu.exe user32.lib gdi32.lib

#include <windows.h>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <atomic>

std::atomic<bool> autofarm{false};
std::wstring chests_text = L"Baús: 0";
std::wstring matches_text = L"Partidas: 0";

void load_stats() {
    std::ifstream f("stats.json");
    if (!f) return;
    std::stringstream ss; ss << f.rdbuf();
    std::string s = ss.str();

    auto extract = [&](const std::string& key) -> int {
        auto p = s.find("\"" + key + "\"");
        if (p == std::string::npos) return 0;
        auto c = s.find(':', p);
        auto start = s.find_first_of("0123456789", c);
        auto end = s.find_first_not_of("0123456789", start);
        if (start == std::string::npos) return 0;
        return std::stoi(s.substr(start, end - start));
    };

    int chests = extract("chests");
    int matches = extract("matches");
    chests_text = L"Baús: " + std::to_wstring(chests);
    matches_text = L"Partidas: " + std::to_wstring(matches);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(240,165,0));
        HFONT font = CreateFontW(22,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH,L"Consolas");
        SelectObject(hdc, font);

        TextOutW(hdc, 20, 20, L"AFK Farm Control", 16);
        TextOutW(hdc, 20, 60, chests_text.c_str(), chests_text.size());
        TextOutW(hdc, 20, 90, matches_text.c_str(), matches_text.size());

        SetTextColor(hdc, autofarm ? RGB(80,220,120) : RGB(220,80,80));
        TextOutW(hdc, 20, 140, autofarm ? L"AUTOFARM: ON" : L"AUTOFARM: OFF", 13);

        SetTextColor(hdc, RGB(136,153,166));
        HFONT small = CreateFontW(16,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
            DEFAULT_PITCH,L"Consolas");
        SelectObject(hdc, small);
        TextOutW(hdc, 20, 180, L"[F8] toggle autofarm", 20);
        TextOutW(hdc, 20, 205, L"[F12] encerrar", 14);

        DeleteObject(font); DeleteObject(small);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_HOTKEY:
        if (wp == 1) autofarm = !autofarm;
        if (wp == 2) PostQuitMessage(0);
        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int main() {
    RegisterHotKey(nullptr, 1, 0, VK_F8);
    RegisterHotKey(nullptr, 2, 0, VK_F12);

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hbrBackground = CreateSolidBrush(RGB(20,26,34));
    wc.lpszClassName = L"AFKFarmMenu";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, L"AFKFarmMenu", L"AFK Farm",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 260,
        nullptr, nullptr, wc.hInstance, nullptr);
    ShowWindow(hwnd, SW_SHOW);

    std::thread refresher([]{
        while (true) {
            load_stats();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    });
    refresher.detach();

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
