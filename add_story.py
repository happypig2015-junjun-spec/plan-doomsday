#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#==============================================================================
# add_story.py  ——  《末日回声 ECHO OF DOOMSDAY v1.0.1》快捷剧情编辑器
#------------------------------------------------------------------------------
# 本脚本的作用：
#   在 Termux / 任意带 Python3 的终端里，通过一问一答的菜单，
#   让你**不看 C++ 代码**就能往游戏里加：
#       1) 新线索 / 新道具（Clue）
#       2) 新剧情场景（Scene），并可一键自动在“避难所 hub”加入口
#       3) 新结局（Ending）
#   写好后自动调用 clang++/g++ 重新编译，编译通过即可 ./doomsday 开玩。
#
# 设计原理：
#   游戏是“数据驱动”的——所有剧情都在 src/StoryData.cpp 里，
#   引擎 GameEngine.cpp 只负责读数据跑逻辑。所以加剧情 = 往这张表里
#   再塞几条数据。本脚本做的就是“帮你按 C++20 指定初始化器的格式
#   把数据拼好，插到表里正确的位置”。
#
# 安全说明：
#   - 每次修改都会先读盘、改完立即写回，中途断电也最多丢最后一次操作；
#   - 插入用的是文件里**唯一的锚点注释**（如 “// （场景表结束）”），
#     不会误插到别的地方；
#   - 编译失败会把报错原样打印，StoryData.cpp 不会被“改坏到不能编译”——
#     因为插入的是结构完整的代码块，最坏情况只是逻辑不对。
#
# 用法：
#     python3 add_story.py
#==============================================================================

import sys
import shutil
import subprocess
from pathlib import Path

#------------------------------------------------------------------------------
# 路径定位：脚本放在项目根目录（和 include/ src/ 同级）
#------------------------------------------------------------------------------
ROOT = Path(__file__).resolve().parent
SRC  = ROOT / "src" / "StoryData.cpp"

if not SRC.exists():
    print(f"[错误] 找不到 {SRC}")
    print("       请把本脚本放在项目根目录（include/ 与 src/ 所在那一层）。")
    sys.exit(1)

# 编译器：Termux 上是 clang++；别处退到 g++
CXX = shutil.which("clang++") or shutil.which("g++")
if CXX is None:
    print("[错误] 既没有 clang++ 也没有 g++，无法编译。")
    sys.exit(1)


#==============================================================================
# 工具函数
#==============================================================================
def cxx(s: str) -> str:
    """把用户输入转成安全的 C++ 字符串字面量内容。

    C++ 字符串用英文双引号包裹，因此：
      - 反斜杠 \\  -> \\\\
      - 英文双引号 " -> \\\"
    中文引号“”不需要转义，原样保留。
    """
    return s.replace("\\", "\\\\").replace('"', '\\"')


def ask(prompt: str, default: str = "") -> str:
    """读一行输入，回车则用默认值。"""
    s = input(prompt).strip()
    return s if s else default


def ask_lines(prompt: str) -> list[str]:
    """逐行收集正文段落，单独一行回车结束。返回段落字符串列表。"""
    print(prompt)
    print("  （每行一段；单独一行回车结束本段输入）")
    out = []
    while True:
        line = input(f"  段落{len(out)+1}> ").strip()
        if not line:
            break
        out.append(line)
    return out


def has_scene_id(text: str, sid: str) -> bool:
    """粗查 StoryData.cpp 里是否已存在某个场景/线索 id。"""
    return f'.id = "{sid}"' in text


#==============================================================================
# 生成：一条 Clue（线索/道具）的 C++ 代码
#==============================================================================
TIER_MAP = {
    "B": "ClueTier::Basic",    # 基础
    "A": "ClueTier::Advanced", # 进阶
    "S": "ClueTier::Supreme",  # 高级
    "I": "ClueTier::Item",     # 道具
}

def build_clue() -> str:
    print("\n=== 添加新线索 / 道具 ===")
    cid = ask("线索编号（如 C1 / item_rope；唯一）: ")
    if not cid:
        print("  已取消。")
        return ""
    tier = ask("阶段（B基础 / A进阶 / S高级 / I道具，默认I）: ", "I").upper()
    if tier not in TIER_MAP:
        tier = "I"
    name = ask("名称（线索本里显示的名字）: ")
    detail = ask("详情（一句话说清它揭示了什么）: ")
    code = (
        f'    Clue{{ .id = "{cxx(cid)}", .tier = {TIER_MAP[tier]},\n'
        f'          .name = "{cxx(name)}", .detail = "{cxx(detail)}" }},\n'
    )
    return code


#==============================================================================
# 生成：一个 Choice（选项）的 C++ 代码
#   注意：C++20 指定初始化器【必须】按 GameTypes.h 里字段声明的顺序写，
#   顺序是：text -> target -> grantedClue -> requireAll -> requireNone -> endingId
#==============================================================================
def build_choice() -> str:
    print("\n  --- 新增一个选项 ---")
    text = ask("  选项文字（屏幕上显示什么）: ")
    if not text:
        return ""
    target = ask("  跳转到的场景 id（回车=回避难所 hub）: ", "hub")
    granted = ask("  选完后获得的线索/道具 id（可空）: ")
    reqall = ask("  显示条件·必须全有（逗号分隔 id，可空）: ")
    reqnone = ask("  显示条件·必须没有（逗号分隔 id，可空）: ")
    ending = ask("  触发的结局 id（填了就不跳转，直接结局；可空）: ")

    parts = [f'Choice{{ .text = "{cxx(text)}", .target = "{cxx(target)}"']
    if granted:
        parts.append(f'.grantedClue = "{cxx(granted)}"')
    if reqall:
        items = ", ".join(f'"{cxx(x.strip())}"' for x in reqall.split(",") if x.strip())
        if items:
            parts.append(f".requireAll = {{ {items} }}")
    if reqnone:
        items = ", ".join(f'"{cxx(x.strip())}"' for x in reqnone.split(",") if x.strip())
        if items:
            parts.append(f".requireNone = {{ {items} }}")
    if ending:
        parts.append(f'.endingId = "{cxx(ending)}"')
    return "    " + ",\n            ".join(parts) + " },\n"


#==============================================================================
# 生成：一个 Scene（场景）的 C++ 代码
#==============================================================================
def build_scene(text: str) -> str:
    print("\n=== 添加新剧情场景 ===")
    sid = ask("场景 id（英文小写+下划线，如 mall_secret_room；唯一）: ")
    if not sid:
        print("  已取消。")
        return ""
    if has_scene_id(text, sid):
        print(f"  [警告] id \"{sid}\" 已存在，仍继续（可能造成跳转混乱）。")
    title = ask("场景标题（可空）: ")
    paras = ask_lines("  正文段落：")
    if not paras:
        paras = ["（此处暂无描述。）"]
    is_death = ask("是否死亡回溯场景？(y/N): ").lower() == "y"

    # 收集选项
    choices = []
    while True:
        c = build_choice()
        if not c:
            break
        choices.append(c)

    # 拼装段落数组
    para_lines = ",\n            ".join(f'"{cxx(p)}"' for p in paras)
    choice_block = ""
    if choices:
        choice_block = "".join("        " + c for c in choices)
    else:
        choice_block = "        /* 无选项（死亡场景自动回溯） */\n"

    death_field = "true" if is_death else "false"
    code = (
        f"    // --- 由 add_story.py 自动生成 ---\n"
        f"    Scene{{\n"
        f'        .id = "{cxx(sid)}",\n'
        f'        .title = "{cxx(title)}",\n'
        f"        .paragraphs =\n"
        f"        {{\n"
        f"            {para_lines}\n"
        f"        }},\n"
        f"        .choices =\n"
        f"        {{\n"
        f"{choice_block}"
        f"        }},\n"
        f"        .isDeath = {death_field}\n"
        f"    }},\n\n"
    )
    return code, sid


#==============================================================================
# 生成：一个 Ending（结局）的 C++ 代码
#==============================================================================
def build_ending() -> str:
    print("\n=== 添加新结局 ===")
    eid = ask("结局 id（如 ending_myend；唯一）: ")
    if not eid:
        print("  已取消。")
        return ""
    title = ask("结局标题（如：结局 · 我的结局）: ")
    paras = ask_lines("  结局正文段落：")
    if not paras:
        paras = ["（结局内容待补。）"]
    para_lines = ",\n            ".join(f'"{cxx(p)}"' for p in paras)
    code = (
        f"    // --- 由 add_story.py 自动生成 ---\n"
        f"    Ending{{\n"
        f'        .id = "{cxx(eid)}",\n'
        f'        .title = "{cxx(title)}",\n'
        f"        .paragraphs =\n"
        f"        {{\n"
        f"            {para_lines}\n"
        f"        }}\n"
        f"    }},\n"
    )
    return code


#==============================================================================
# 写回文件：把生成的代码插到对应表的“结束括号”之前
#==============================================================================
ANCHOR_CLUE_END = "};\n\n//==============================================================================\n// 二、场景表"
ANCHOR_SCENE_END = "    // （场景表结束）"
ANCHOR_ENDING_END = "\n};\n\n} // anonymous namespace"
ANCHOR_HUB_EXIT = "            //--- 退出 ---"


def insert_clue(code: str) -> bool:
    text = SRC.read_text(encoding="utf-8")
    if ANCHOR_CLUE_END not in text:
        print("[错误] 找不到线索表结束锚点，未修改文件。")
        return False
    text = text.replace(ANCHOR_CLUE_END, code + ANCHOR_CLUE_END, 1)
    SRC.write_text(text, encoding="utf-8")
    print("[ok] 已插入新线索。")
    return True


def insert_scene(code: str, hub_entry: bool, sid: str) -> bool:
    text = SRC.read_text(encoding="utf-8")
    if ANCHOR_SCENE_END not in text:
        print("[错误] 找不到场景表结束锚点，未修改文件。")
        return False
    text = text.replace(ANCHOR_SCENE_END, code + ANCHOR_SCENE_END, 1)

    # 可选：在 hub 的“退出”选项之前，加一个入口
    if hub_entry:
        entry = (f'            Choice{{ .text = "【自定义】前往 {sid}", '
                 f'.target = "{cxx(sid)}" }},\n')
        if ANCHOR_HUB_EXIT in text:
            text = text.replace(ANCHOR_HUB_EXIT, entry + ANCHOR_HUB_EXIT, 1)
            print("[ok] 已在避难所(hub)加入入口。")
        else:
            print("[警告] 找不到 hub 退出锚点，未加入口，请手动在 hub 加选项。")

    SRC.write_text(text, encoding="utf-8")
    print("[ok] 已插入新场景。")
    return True


def insert_ending(code: str) -> bool:
    text = SRC.read_text(encoding="utf-8")
    if ANCHOR_ENDING_END not in text:
        print("[错误] 找不到结局表结束锚点，未修改文件。")
        return False
    text = text.replace(ANCHOR_ENDING_END, "\n" + code + "};", 1)
    SRC.write_text(text, encoding="utf-8")
    print("[ok] 已插入新结局。")
    return True


#==============================================================================
# 编译
#==============================================================================
def compile_game() -> int:
    print("\n=== 编译中（首次会慢一点）===")
    cmd = [
        CXX, "-std=c++20", "-O2", "-Wall", "-Wextra", "-Wpedantic",
        "-I", "include",
        "src/main.cpp", "src/Terminal.cpp", "src/StoryData.cpp", "src/GameEngine.cpp",
        "-o", "doomsday",
    ]
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if r.returncode == 0:
        print("[成功] 编译通过！运行：  ./doomsday")
    else:
        print("[失败] 编译报错如下：")
        if r.stderr:
            print(r.stderr)
        if r.stdout:
            print(r.stdout)
        print("\n排查建议：")
        print("  - 检查你刚加的场景 id 是否和别的选项跳转对得上；")
        print("  - 检查选项里填的 requireAll / grantedClue id 是否拼写正确；")
        print("  - 把报错行号附近的新场景贴出来对照。")
    return r.returncode


#==============================================================================
# 列出现有场景 id
#==============================================================================
def list_scenes():
    import re
    text = SRC.read_text(encoding="utf-8")
    ids = re.findall(r'\.id = "([^"]+)"', text)
    print(f"\n当前共 {len(ids)} 个 .id 条目（含线索/场景/结局）：")
    # 每行打印 4 个
    for i in range(0, len(ids), 4):
        print("  " + "  ".join(ids[i:i+4]))


#==============================================================================
# 主菜单
#==============================================================================
def main():
    print("=" * 56)
    print("   末日回声 v1.0.1  快捷剧情编辑器 add_story.py")
    print("=" * 56)
    while True:
        print("\n请选择要做的事：")
        print("  1) 添加新线索 / 道具")
        print("  2) 添加新剧情场景（可选自动加避难所入口）")
        print("  3) 添加新结局")
        print("  4) 列出现有全部 id（方便复制跳转目标）")
        print("  5) 编译")
        print("  0) 退出")
        op = input("> ").strip()

        if op == "1":
            code = build_clue()
            if code:
                insert_clue(code)
        elif op == "2":
            text = SRC.read_text(encoding="utf-8")
            res = build_scene(text)
            if not res:
                continue
            code, sid = res
            hub = input("是否在避难所自动加入入口？(Y/n): ").lower() != "n"
            insert_scene(code, hub, sid)
        elif op == "3":
            code = build_ending()
            if code:
                insert_ending(code)
        elif op == "4":
            list_scenes()
        elif op == "5":
            compile_game()
        elif op == "0":
            print("再见。改完记得选 5 编译再玩。")
            break
        else:
            print("无效选项。")


if __name__ == "__main__":
    main()
