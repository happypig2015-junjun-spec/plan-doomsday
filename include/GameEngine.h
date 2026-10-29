//==============================================================================
// GameEngine.h
//------------------------------------------------------------------------------
// 游戏引擎：解释 StoryData 中的数据表，驱动场景流转、线索收集、
// 选项条件过滤、结局判定与“死亡—回溯”循环。
//
// 引擎本身不包含任何具体剧情文字，所有内容均来自 StoryData，
// 因此同一份引擎可以承载任意多套剧情（甚至可以做出“剧情包切换”）。
//==============================================================================
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <string>
#include <string_view>
#include <unordered_set>

#include "GameTypes.h"

class GameEngine
{
public:
    GameEngine();

    // 游戏主循环：从开场场景开始，直到玩家达成真结局或退出为止。
    void run();

private:
    // 玩家当前已获得的线索 id 集合（跨回溯保留，是“记忆”的具象化）。
    std::unordered_set<std::string> ownedClues_;

    // 当前所在场景 id。
    std::string currentSceneId_;

    // 主循环是否继续。
    bool running_ = true;

    // 已发动的回溯次数（死亡次数），真结局文本中会呼应这个数字。
    int  loopCount_ = 0;

    //==== v1.0.2 新增：探索足迹（用于“已完成分支”统计与存档）====
    // 玩家已经进入过的场景 id 集合（跨回溯保留）。
    std::unordered_set<std::string> visitedScenes_;
    // 玩家已经触发过的结局 id 集合（跨回溯保留，用于结局收集进度）。
    std::unordered_set<std::string> achievedEndings_;

    //---- 内部流程 -----------------------------------------------------------
    // 播放一个场景：标题 -> 正文 -> （死亡回溯 | 选项菜单）。
    void playScene(const Scene& scene);

    // 打印并读取一个场景的选项菜单，返回被选中的选项在 scene.choices 中的下标。
    // （L 查看线索本、Q 退出等特殊输入在本函数内部处理完毕。）
    int  presentMenu(const Scene& scene);

    // 获得一条线索（重复获得不提示），并播放“获得线索”提示。
    void grantClue(std::string_view id);

    // 判断某个选项在当前线索状态下是否应当显示。
    bool isChoiceAvailable(const Choice& choice) const;

    // 判定玩家是否已拥有某线索。
    bool hasClue(std::string_view id) const;

    // 玩家在某一阶段已拥有的线索数量。
    std::size_t ownedCountOfTier(ClueTier tier) const;

    // 展示线索本（按三档分组，含未获得线索的占位与进度）。
    void showClueJournal() const;

    // 触发结局：播放结局文本；非真结局后可继续探索，真结局后结束游戏。
    void triggerEnding(const Ending& ending);

    // 发动时间回溯：回到中枢（计数 +1）。
    void rewindToHub();

    //==== v1.0.2 新增：存档 / 读档 / 探索进度 ====
    // 存档槽位数量（固定 3 个槽位）。
    static constexpr int kSaveSlots = 3;

    // 把当前游戏状态写入第 slot 个槽位文件。
    void saveGame(int slot);

    // 从第 slot 个槽位文件读取状态；成功返回 true，失败（文件不存在/损坏）返回 false。
    bool loadGame(int slot);

    // 存档 / 读档菜单（按 S 进入）。
    // 返回值：true 表示玩家刚刚成功读档，调用方应立即让主循环按新场景重绘。
    bool showSaveMenu();

    // 查看“已完成分支”：场景探索进度 + 结局收集进度（按 R 进入）。
    void showProgress() const;

    // 第 slot 个槽位的存档文件路径（相对当前工作目录）。
    static std::string slotPath(int slot);

    // 读取某个槽位文件的一句话摘要（用于存档菜单展示）；文件不存在返回空串。
    static std::string slotSummary(int slot);
};

#endif // GAME_ENGINE_H
