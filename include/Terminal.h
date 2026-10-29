//==============================================================================
// Terminal.h
//------------------------------------------------------------------------------
// 终端交互层：封装所有与“屏幕 / 键盘”直接相关的操作，
// 包括核心需求 slowprint（剧情逐字显示）。
//
// 特性：
//   * 逐字符输出，字符间可配置延迟；
//   * 遇到逗号 / 句号 / 省略号等标点自动加长停顿，模拟叙事节奏；
//   * 输出过程中按任意键，可立即“快进”完当前段落（无需干等）；
//   * 完整支持中文（按 UTF-8 多字节序列输出，不会把汉字切成乱码）；
//   * 跨平台：Linux / macOS（termios + select）与 Windows（conio + VT）；
//   * v1.0.3 起菜单支持 ↑ ↓ ← → 方向键 + 回车（也保留数字键），
//     直接用 termios 配置“当前终端”，不创建 / 不依赖任何伪终端（pty）。
//==============================================================================
#ifndef TERMINAL_H
#define TERMINAL_H

#include <string_view>
#include <string>
#include <vector>

namespace term
{

//--- ANSI 颜色与样式（Windows 10+ 在 init() 中开启虚拟终端后同样可用）--------
inline constexpr std::string_view kReset   = "\033[0m";
inline constexpr std::string_view kBold    = "\033[1m";
inline constexpr std::string_view kDim     = "\033[2m";
inline constexpr std::string_view kRed     = "\033[31m";
inline constexpr std::string_view kGreen   = "\033[32m";
inline constexpr std::string_view kYellow  = "\033[33m";
inline constexpr std::string_view kBlue    = "\033[34m";
inline constexpr std::string_view kMagenta = "\033[35m";
inline constexpr std::string_view kCyan    = "\033[36m";
inline constexpr std::string_view kGray    = "\033[90m";

// 初始化终端（Windows 下开启 ANSI 虚拟终端处理；POSIX 下为空操作）。
void init();

// 逐字输出文本（不自动换行）。
//   charDelayMs：每个字符的基础延迟（毫秒）。
//   输出期间按任意键可立即输出剩余部分。
void slowPrint(std::string_view text, int charDelayMs = 22);

// 逐字输出一行文本，并在末尾换行。
void slowLine(std::string_view text, int charDelayMs = 22);

// 暂停，等待玩家按下回车。
void pause(std::string_view prompt = "\n[ 按回车继续 ]");

// 清屏并把光标移到左上角。
void clear();

// 读取一个菜单选择。
//   maxChoice：合法选项的最大编号（编号从 1 开始）。
//   返回值：
//     >=  1  玩家选择的选项编号（已保证落在 [1, maxChoice]）；
//     == -1  玩家输入了 L / l：请求查看“线索本”；
//     == -2  玩家输入了 Q / q：请求退出游戏；
//     == -3  玩家输入了 S / s：请求打开“存档 / 读档菜单”；
//     == -4  玩家输入了 R / r：请求查看“已完成分支 / 探索进度”。
int readChoice(int maxChoice);

//------------------------------------------------------------------------------
// 方向键交互式菜单（v1.0.3）。
//
// 操作：
//   ↑ / ↓（或 ← / →）：移动高亮选项（首尾循环）；
//   回车（Enter）     ：确认当前高亮项；
//   数字键 1~N         ：直接选择对应项（保留旧的数字输入习惯）。
//
// 当 allowSpecialKeys 为 true 时，下列字母键仍可随时触发（与旧版一致）：
//   L=线索本  S=存档 / 读档  R=已完成分支  Q=退出。
//
// 返回值：
//   >=  0  选中项的 0-based 下标；
//   == -1  L / l（线索本）；
//   == -2  Q / q（退出）；
//   == -3  S / s（存档菜单）；
//   == -4  R / r（已完成分支）。
// （-1~-4 仅在 allowSpecialKeys == true 时才可能返回。）
//------------------------------------------------------------------------------
int menuSelect(const std::vector<std::string>& items, bool allowSpecialKeys);

} // namespace term

#endif // TERMINAL_H
