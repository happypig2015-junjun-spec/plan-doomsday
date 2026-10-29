//==============================================================================
// Terminal.cpp
//------------------------------------------------------------------------------
// Terminal.h 的实现：逐字输出（slowprint）、标点停顿、任意键快进、
// UTF-8 中文支持、跨平台键盘检测、颜色、数字菜单与方向键菜单（v1.0.3）。
//==============================================================================
#include "Terminal.h"

#include <chrono>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>

//--- 平台相关头文件 ----------------------------------------------------------
#if defined(_WIN32)
#  include <windows.h>
#  include <conio.h>
#else
#  include <sys/select.h>
#  include <termios.h>
#  include <unistd.h>
#endif

namespace term
{

#if defined(_WIN32)
// Windows：用 conio 的 _kbhit 非阻塞检测按键（不取出，留给后续 cin 读取）。
static bool inputReady() { return _kbhit() != 0; }
#else
// POSIX：临时把标准输入切为“非规范、无回显”模式，用 select 探测是否有键
// （只探测、不读取，字节留在缓冲区供后续 getline 使用）。
static bool inputReady()
{
    termios oldTerm{};
    tcgetattr(STDIN_FILENO, &oldTerm);
    termios newTerm = oldTerm;
    newTerm.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);

    timeval tv{};
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    const int ready = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
    return ready > 0;
}
#endif

void init()
{
#if defined(_WIN32)
    // 开启 Windows 控制台的 ANSI 转义序列支持。
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  mode   = 0;
    if (handle != nullptr && GetConsoleMode(handle, &mode))
    {
        SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
    // 统一使用 UTF-8，避免中文乱码。
    std::cout << "\033[?25h"; // 确保光标可见
}

//--- UTF-8 工具 --------------------------------------------------------------
// 返回从字节 i 开始的 UTF-8 字符所占字节数（1~4）。
static int utf8CharLength(const std::string& s, std::size_t i)
{
    const auto c = static_cast<unsigned char>(s[i]);
    if ((c & 0x80) == 0x00) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    return 4;
}

// 若字节 i 位于一个“中文标点”的首字节，返回应停顿的毫秒数；否则返回 0。
static int chinesePunctPause(const std::string& s, std::size_t i)
{
    if (i + 2 >= s.size()) return 0;
    const auto a = static_cast<unsigned char>(s[i]);
    const auto b = static_cast<unsigned char>(s[i + 1]);
    const auto cc = static_cast<unsigned char>(s[i + 2]);

    // 全角标点位于 U+FF00..U+FFEF（UTF-8: EF BC xx）
    if (a == 0xEF && b == 0xBC)
    {
        switch (cc)
        {
            case 0x8C: // ，
            case 0x9A: // ：
            case 0x9B: // ；
                return 120;
            case 0x81: // ！
            case 0x9F: // ？
                return 360;
            default:
                return 0;
        }
    }
    if (a == 0xE3 && b == 0x80 && cc == 0x82) return 380; // 。
    if (a == 0xE2 && b == 0x80 && cc == 0xA6) return 180; // …
    if (a == 0xE2 && b == 0x80 && cc == 0x94) return 120; // —
    return 0;
}

void slowPrint(std::string_view text, int charDelayMs)
{
    const std::string s(text);
    bool skipped = false; // 按下任意键后变为 true，剩余字符瞬时输出

    for (std::size_t i = 0; i < s.size();)
    {
        if (!skipped && inputReady())
        {
            // 仅切换为“快进”，不要取出该键：
            // 字节留在输入缓冲区，随后菜单的 getline 仍能读到它——
            // 这样玩家在播放期间提前输入的数字 / 回车都不会被吞掉。
            skipped = true;
        }

        const int len = utf8CharLength(s, i);
        std::cout.write(s.data() + static_cast<std::ptrdiff_t>(i), len);
        std::cout.flush();

        if (!skipped)
        {
            int delay = charDelayMs;
            if (len == 1)
            {
                const char c = s[i];
                if (c == '\n')
                    delay = 160;
                else if (c == ',' || c == ';' || c == ':')
                    delay = 110;
                else if (c == '.' || c == '!' || c == '?')
                    delay = 320;
            }
            else if (len == 3)
            {
                const int punct = chinesePunctPause(s, i);
                if (punct > 0) delay = punct;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        i += static_cast<std::size_t>(len);
    }
}

void slowLine(std::string_view text, int charDelayMs)
{
    slowPrint(text, charDelayMs);
    std::cout << '\n';
}

void pause(std::string_view prompt)
{
    slowPrint(prompt);
    std::string dummy;
    std::getline(std::cin, dummy);
}

void clear()
{
    // ANSI 清屏 + 光标归位（Windows VT 同样支持）。
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

int readChoice(int maxChoice)
{
    while (true)
    {
        std::string line;
        std::getline(std::cin, line);
        if (!line.empty())
        {
            if (line[0] == 'L' || line[0] == 'l') return -1;
            if (line[0] == 'Q' || line[0] == 'q') return -2;
            if (line[0] == 'S' || line[0] == 's') return -3;
            if (line[0] == 'R' || line[0] == 'r') return -4;
            try
            {
                std::size_t pos = 0;
                const int value = std::stoi(line, &pos);
                if (value >= 1 && value <= maxChoice) return value;
            }
            catch (...)
            {
                // 落入下方提示
            }
        }
        slowPrint("  （请输入 1~" + std::to_string(maxChoice) +
                  " 之间的编号；L=线索本，S=存档，R=进度，Q=退出）\n");
    }
}

//==============================================================================
// v1.0.3 方向键菜单
//------------------------------------------------------------------------------
// 关于“不依赖伪终端”：
//   * 不调用 posix_openpt / grantpt / unlockpt / forkpty 等任何“创建伪终端”
//     的接口；
//   * 仅通过 termios 把“当前已连接的标准输入终端”临时切换到非规范
//     （non-canonical：一次 read 即可读到单个按键、不回显）模式，
//     菜单结束 / 析构时立即恢复原设置；
//   * 方向键在终端里是一组 ANSI 转义序列：ESC '[' 后接 'A'=上 'B'=下
//     'C'=右 'D'=左。
//==============================================================================
namespace
{

// 按键的语义类型。
enum class KeyKind { Character, Up, Down, Left, Right, Enter, Escape };

// 一次按键：kind 表示类型；为 Character 时 ch 为对应字符。
struct Key
{
    KeyKind kind;
    char ch;
};

#if defined(_WIN32)
// ---- Windows：_getch 本身即“单字符、不回显”，无需额外切换终端模式 ----
class RawMode
{
public:
    void enter() {}
    void leave() {}
};

static Key readKey()
{
    int c = _getch();
    // 方向键 / 功能键在 Windows 控制台以 0x00 或 0xE0 为前缀，再读一个码。
    if (c == 0x00 || c == 0xE0)
    {
        switch (_getch())
        {
            case 72: return { KeyKind::Up, 0 };
            case 80: return { KeyKind::Down, 0 };
            case 77: return { KeyKind::Right, 0 };
            case 75: return { KeyKind::Left, 0 };
            default: return { KeyKind::Escape, 0 };
        }
    }
    if (c == '\r' || c == '\n') return { KeyKind::Enter, 0 };
    if (c == 27)                 return { KeyKind::Escape, 0 };
    return { KeyKind::Character, static_cast<char>(c) };
}
#else
// ---- POSIX（Linux / macOS / Termux）：用 termios 临时进入非规范模式 ----
class RawMode
{
public:
    void enter()
    {
        // 若标准输入不是终端（被重定向 / 管道），则不修改，避免破坏其它场景。
        if (!isatty(STDIN_FILENO)) return;
        tcgetattr(STDIN_FILENO, &saved_);
        termios raw = saved_;
        raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO)); // 关行缓冲、关回显
        raw.c_cc[VMIN]  = 1; // read 至少读到 1 字节才返回
        raw.c_cc[VTIME] = 0; // 不做读超时
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        active_ = true;
    }
    void leave()
    {
        if (active_)
        {
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_);
            active_ = false;
        }
    }
    ~RawMode() { leave(); }
private:
    termios saved_{};
    bool active_ = false;
};

static Key readKey()
{
    char c = 0;
    if (read(STDIN_FILENO, &c, 1) != 1) return { KeyKind::Escape, 0 };
    if (c == '\r' || c == '\n') return { KeyKind::Enter, 0 };
    if (c != 0x1b)              return { KeyKind::Character, c };

    // 读到 ESC：区分“单独的 ESC”与“方向键序列的开头”。
    // 用 select 等待极短时间（15ms），看是否有后续字节。
    timeval tv{};
    tv.tv_sec  = 0;
    tv.tv_usec = 15000;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) <= 0)
        return { KeyKind::Escape, 0 }; // 无后续 -> 单独 ESC

    char c2 = 0;
    if (read(STDIN_FILENO, &c2, 1) != 1) return { KeyKind::Escape, 0 };
    if (c2 != '[') return { KeyKind::Escape, 0 };

    char c3 = 0;
    if (read(STDIN_FILENO, &c3, 1) != 1) return { KeyKind::Escape, 0 };
    switch (c3)
    {
        case 'A': return { KeyKind::Up, 0 };
        case 'B': return { KeyKind::Down, 0 };
        case 'C': return { KeyKind::Right, 0 };
        case 'D': return { KeyKind::Left, 0 };
        default:  return { KeyKind::Escape, 0 };
    }
}
#endif

} // namespace

int menuSelect(const std::vector<std::string>& items, bool allowSpecialKeys)
{
    if (items.empty()) return 0;

    RawMode raw;
    raw.enter();

    std::size_t selected = 0;
    const std::size_t count = items.size();

    // 绘制 / 重绘整个选项列表。
    // 高亮行用反色（SGR 7），每行先清整行（SGR 2K）+ 回车，避免文字残留。
    auto draw = [&]()
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            std::cout << "\033[2K\r";
            if (i == selected)
            {
                std::cout << "\033[7m ▶ " << (i + 1) << ". " << items[i]
                          << " \033[0m\n";
            }
            else
            {
                std::cout << "   " << (i + 1) << ". " << items[i] << "\n";
            }
        }
        std::cout.flush();
    };

    draw(); // 首次绘制

    while (true)
    {
        const Key key = readKey();
        bool changed = false;

        switch (key.kind)
        {
            case KeyKind::Up:
            case KeyKind::Left:
                selected = (selected + count - 1) % count;
                changed = true;
                break;
            case KeyKind::Down:
            case KeyKind::Right:
                selected = (selected + 1) % count;
                changed = true;
                break;
            case KeyKind::Enter:
                raw.leave();
                return static_cast<int>(selected);
            case KeyKind::Character:
            {
                const char lower =
                    static_cast<char>(std::tolower(static_cast<unsigned char>(key.ch)));
                // 数字键直接选择（1~N），保留旧输入习惯。
                if (key.ch >= '1' && key.ch <= '9')
                {
                    const int n = key.ch - '0';
                    if (static_cast<std::size_t>(n) <= count)
                    {
                        raw.leave();
                        return n - 1;
                    }
                }
                if (allowSpecialKeys)
                {
                    if (lower == 'l') { raw.leave(); return -1; }
                    if (lower == 'q') { raw.leave(); return -2; }
                    if (lower == 's') { raw.leave(); return -3; }
                    if (lower == 'r') { raw.leave(); return -4; }
                }
                break;
            }
            case KeyKind::Escape:
                // 单独 ESC 不做选择（防误触），停留在菜单。
                break;
        }

        if (changed)
        {
            // 光标上移 count 行回到列表第一行，再整体重绘。
            std::cout << "\033[" << count << "A";
            draw();
        }
    }
}

} // namespace term
