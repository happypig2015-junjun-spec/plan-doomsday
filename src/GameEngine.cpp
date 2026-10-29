//==============================================================================
// GameEngine.cpp
//------------------------------------------------------------------------------
// GameEngine.h 的实现。解释剧情数据表，驱动：
//   清屏 -> 标题 -> 正文逐字播放 -> （死亡回溯 | 选项菜单），
// 并维护线索集合、线索本、进度与结局判定。
//==============================================================================
#include "GameEngine.h"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "StoryData.h"
#include "Terminal.h"

namespace
{
// 不同阶段线索 / 物品在屏幕上的配色。
std::string_view tierColor(ClueTier tier)
{
    switch (tier)
    {
        case ClueTier::Basic:    return term::kCyan;
        case ClueTier::Advanced: return term::kYellow;
        case ClueTier::Supreme:  return term::kMagenta;
        case ClueTier::Item:     return term::kGreen;
    }
    return term::kReset;
}

const char* tierLabel(ClueTier tier)
{
    switch (tier)
    {
        case ClueTier::Basic:    return "基础线索";
        case ClueTier::Advanced: return "进阶线索";
        case ClueTier::Supreme:  return "高级线索";
        case ClueTier::Item:     return "随身物品";
    }
    return "";
}
} // namespace

GameEngine::GameEngine()
    : currentSceneId_(std::string(story::kSceneStart))
{
}

//==============================================================================
// 主循环
//==============================================================================
void GameEngine::run()
{
    while (running_)
    {
        const Scene* scene = story::findScene(currentSceneId_);
        if (scene == nullptr)
        {
            // 兜底：任何非法跳转都回到中枢，避免游戏卡死。
            currentSceneId_ = std::string(story::kSceneHub);
            continue;
        }
        playScene(*scene);
    }
}

//==============================================================================
// 播放一个场景
//==============================================================================
void GameEngine::playScene(const Scene& scene)
{
    term::clear();

    // 记录“探索足迹”：进入过的场景（用于已完成分支统计与存档）。
    visitedScenes_.insert(scene.id);

    // 标题（非空时加粗显示）。
    if (!scene.title.empty())
    {
        term::slowPrint(std::string(term::kBold) + "—— " + scene.title + " ——" +
                        std::string(term::kReset) + "\n");
    }

    // 正文逐段逐字播放。
    for (const std::string& paragraph : scene.paragraphs)
    {
        term::slowPrint(paragraph);
        term::slowPrint("\n");
    }

    // 进入中枢时，额外显示三档线索收集进度。
    if (scene.id == story::kSceneHub)
    {
        auto line = std::string("\n") + std::string(term::kDim) +
                    "线索进度  基础 " +
                    std::to_string(ownedCountOfTier(ClueTier::Basic)) + "/" +
                    std::to_string(story::clueCountOfTier(ClueTier::Basic)) +
                    "   进阶 " +
                    std::to_string(ownedCountOfTier(ClueTier::Advanced)) + "/" +
                    std::to_string(story::clueCountOfTier(ClueTier::Advanced)) +
                    "   高级 " +
                    std::to_string(ownedCountOfTier(ClueTier::Supreme)) + "/" +
                    std::to_string(story::clueCountOfTier(ClueTier::Supreme)) +
                    std::string(term::kReset) + "\n";
        term::slowPrint(line);
    }

    // 死亡场景：发动时间回溯。
    if (scene.isDeath)
    {
        term::slowPrint(std::string(term::kRed) +
                        "\n  ▌ 时间在此断裂……" + std::string(term::kReset));
        term::pause(std::string(term::kRed) +
                    "  ▌ 发动回溯，回到锚点 [按回车]" + std::string(term::kReset));
        rewindToHub();
        return;
    }

    // 普通场景：呈现选项菜单。
    const int pickedIndex = presentMenu(scene);
    if (!running_) return; // 玩家在菜单中选择了退出
    if (pickedIndex == -100) return; // 玩家刚读档：直接返回，主循环按新场景重绘

    const Choice& picked = scene.choices[static_cast<std::size_t>(pickedIndex)];

    // 授予线索 / 物品（单个 + 多个）。
    grantClue(picked.grantedClue);
    for (const std::string& id : picked.grantedClues)
        grantClue(id);

    // 触发结局。
    if (!picked.endingId.empty())
    {
        const Ending* ending = story::findEnding(picked.endingId);
        if (ending != nullptr) triggerEnding(*ending);
        return;
    }

    // 特殊目标：退出游戏。
    if (picked.target == "__quit")
    {
        running_ = false;
        return;
    }

    // 跳转：目标为空则回中枢。
    currentSceneId_ = picked.target.empty() ? std::string(story::kSceneHub)
                                           : picked.target;
}

//==============================================================================
// 选项菜单：过滤、展示、读取（含线索本 / 退出的特殊输入）
//==============================================================================
int GameEngine::presentMenu(const Scene& scene)
{
    // 收集当前状态下“可见”的选项下标，以及对应的菜单文本。
    std::vector<int> visible;
    std::vector<std::string> labels;
    for (std::size_t i = 0; i < scene.choices.size(); ++i)
    {
        if (isChoiceAvailable(scene.choices[i]))
        {
            visible.push_back(static_cast<int>(i));
            labels.push_back(scene.choices[i].text);
        }
    }

    while (true)
    {
        // 操作提示（固定在菜单上方，不随高亮重绘）。
        term::slowPrint(std::string("\n") + std::string(term::kGray) +
                        "（↑↓ 移动，回车确认；L=线索本；S=存档/读档；R=已完成分支；Q=退出）" +
                        std::string(term::kReset) + "\n");

        // 方向键菜单（同时支持数字键与 L/S/R/Q）。
        const int input = term::menuSelect(labels, true);
        if (input == -1)
        {
            showClueJournal();
            continue; // 看完线索本后重新展示菜单
        }
        if (input == -2)
        {
            running_ = false;
            return 0;
        }
        if (input == -3)
        {
            // 读档成功则返回特殊值 -100，让 playScene 退出、主循环按新场景重绘。
            if (showSaveMenu()) return -100;
            continue; // 仅存档/取消，则重新展示菜单
        }
        if (input == -4)
        {
            showProgress();
            continue; // 看完进度后重新展示菜单
        }
        // menuSelect 返回 0-based 下标，映射回 scene.choices 的真实下标。
        return visible[static_cast<std::size_t>(input)];
    }
}

//==============================================================================
// 状态判定
//==============================================================================
bool GameEngine::hasClue(std::string_view id) const
{
    return ownedClues_.find(std::string(id)) != ownedClues_.end();
}

bool GameEngine::isChoiceAvailable(const Choice& choice) const
{
    // requireAll：必须全部拥有。
    for (const std::string& id : choice.requireAll)
    {
        if (!hasClue(id)) return false;
    }
    // requireNone：必须全部未拥有。
    for (const std::string& id : choice.requireNone)
    {
        if (hasClue(id)) return false;
    }
    return true;
}

std::size_t GameEngine::ownedCountOfTier(ClueTier tier) const
{
    std::size_t count = 0;
    for (const Clue& clue : story::clues())
    {
        if (clue.tier == tier && hasClue(clue.id)) ++count;
    }
    return count;
}

//==============================================================================
// 获得线索 / 物品
//==============================================================================
void GameEngine::grantClue(std::string_view id)
{
    if (id.empty()) return;
    const auto [it, inserted] = ownedClues_.insert(std::string(id));
    if (!inserted) return; // 重复获得，不重复提示

    const Clue* clue = story::findClue(id);
    if (clue == nullptr) return;

    const std::string color(tierColor(clue->tier));
    const std::string kind = (clue->tier == ClueTier::Item) ? "获得物品" : "获得线索";
    term::slowPrint("\n" + color + "  ▶ " + kind + "【" + clue->name + "】" +
                    std::string(term::kReset) + "\n");
}

//==============================================================================
// 线索本
//==============================================================================
void GameEngine::showClueJournal() const
{
    term::clear();
    term::slowPrint(std::string(term::kBold) + "════ 线 索 本 ════" +
                    std::string(term::kReset) + "\n");

    const ClueTier tiers[] = { ClueTier::Basic, ClueTier::Advanced,
                               ClueTier::Supreme, ClueTier::Item };
    for (ClueTier tier : tiers)
    {
        const std::string color(tierColor(tier));
        term::slowPrint("\n" + color + std::string(term::kBold) + "【" +
                        tierLabel(tier) + "】" +
                        std::string(term::kReset) + "\n");

        for (const Clue& clue : story::clues())
        {
            if (clue.tier != tier) continue;
            if (hasClue(clue.id))
            {
                term::slowPrint(color + "  ■ " + clue.name + std::string(term::kReset) +
                                "\n      " + clue.detail + "\n");
            }
            else
            {
                term::slowPrint(std::string(term::kGray) + "  □ ？？？（尚未发现）" +
                                std::string(term::kReset) + "\n");
            }
        }
    }
    term::pause();
}

//==============================================================================
// 结局
//==============================================================================
void GameEngine::triggerEnding(const Ending& ending)
{
    term::clear();

    // 记录“已达成结局”（用于结局收集进度与存档）。
    achievedEndings_.insert(ending.id);
    term::slowPrint(std::string(term::kBold) + std::string(term::kYellow) +
                    "════ " + ending.title + " ════" +
                    std::string(term::kReset) + "\n\n");
    for (const std::string& paragraph : ending.paragraphs)
    {
        term::slowPrint(paragraph);
        term::slowPrint("\n");
    }

    // 真结局 / 完美真结局：全剧终，结束游戏。
    if (ending.id == "ending_true" || ending.id == "ending_true_complete")
    {
        term::slowPrint("\n" + std::string(term::kBold) +
                        "—— 全 剧 终 ——" + std::string(term::kReset) + "\n");
        running_ = false;
        return;
    }

    // 非真结局：允许继续探索，去凑齐更深的线索。
    term::slowPrint("\n" + std::string(term::kGray) +
                    "（更深的真相仍在雾中，你可以继续探索。）" +
                    std::string(term::kReset) + "\n");
    const std::vector<std::string> after = { "回到锚点，继续探索",
                                             "就此结束游戏" };
    const int input = term::menuSelect(after, false);
    if (input == 0)
    {
        currentSceneId_ = std::string(story::kSceneHub);
        return;
    }
    if (input == 1)
    {
        running_ = false;
        return;
    }
}

//==============================================================================
// 回溯
//==============================================================================
void GameEngine::rewindToHub()
{
    ++loopCount_;
    currentSceneId_ = std::string(story::kSceneHub);
}

//==============================================================================
// v1.0.2 存档 / 读档 / 探索进度
//------------------------------------------------------------------------------
// 存档文件格式（纯文本，方便手工查看/排错，不依赖第三方库）：
//   # ECHO_SAVE          <- 魔数，用来识别“这是本游戏的存档”
//   version=1
//   loop=12              <- 回溯次数
//   scene=hub            <- 读档后出现的场景 id
//   clue=B1              <- 已拥有线索（可多行）
//   clue=A2
//   visited=market_outside  <- 已访问场景（可多行）
//   ending=ending_basic  <- 已达成结局（可多行）
//
// 说明：所有 id 都只含字母/数字/下划线，不含 '='，因此用 key=value 解析安全。
//==============================================================================
std::string GameEngine::slotPath(int slot)
{
    // 存档生成在“运行游戏时的当前工作目录”，即项目根目录。
    return "doomsday_save" + std::to_string(slot) + ".dat";
}

void GameEngine::saveGame(int slot)
{
    std::ofstream f(slotPath(slot), std::ios::trunc);
    f << "# ECHO_SAVE\n";
    f << "version=1\n";
    f << "loop=" << loopCount_ << "\n";
    f << "scene=" << currentSceneId_ << "\n";
    for (const std::string& c : ownedClues_)
        f << "clue=" << c << "\n";
    for (const std::string& v : visitedScenes_)
        f << "visited=" << v << "\n";
    for (const std::string& e : achievedEndings_)
        f << "ending=" << e << "\n";
}

bool GameEngine::loadGame(int slot)
{
    std::ifstream f(slotPath(slot));
    if (!f) return false;

    // 先读到临时变量，全部成功后再一次性覆盖当前状态，避免读到一半损坏状态。
    std::unordered_set<std::string> clues;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> endings;
    std::string scene;
    int loop = 0;
    bool magic = false;

    std::string line;
    while (std::getline(f, line))
    {
        if (line.empty()) continue;
        if (line[0] == '#')
        {
            if (line == "# ECHO_SAVE") magic = true;
            continue;
        }
        const std::size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        const std::string key = line.substr(0, eq);
        const std::string val = line.substr(eq + 1);

        if (key == "loop")
        {
            try { loop = std::stoi(val); } catch (...) {}
        }
        else if (key == "scene")   scene = val;
        else if (key == "clue")    clues.insert(val);
        else if (key == "visited") visited.insert(val);
        else if (key == "ending")  endings.insert(val);
    }

    // 校验：魔数正确、场景存在，才认为存档有效。
    if (!magic) return false;
    if (scene.empty() || story::findScene(scene) == nullptr) return false;

    ownedClues_       = std::move(clues);
    visitedScenes_    = std::move(visited);
    achievedEndings_  = std::move(endings);
    currentSceneId_   = scene;
    loopCount_        = loop;
    return true;
}

std::string GameEngine::slotSummary(int slot)
{
    std::ifstream f(slotPath(slot));
    if (!f) return "";

    bool magic = false;
    int loop = 0, clueCount = 0;
    std::string scene;
    std::string line;
    while (std::getline(f, line))
    {
        if (line == "# ECHO_SAVE") magic = true;
        else if (line.rfind("loop=", 0) == 0)
        {
            try { loop = std::stoi(line.substr(5)); } catch (...) {}
        }
        else if (line.rfind("scene=", 0) == 0) scene = line.substr(6);
        else if (line.rfind("clue=", 0) == 0) ++clueCount;
    }
    if (!magic) return "[存档损坏]";
    return "第" + std::to_string(loop) + "次回溯 · " +
           std::to_string(clueCount) + "线索 · " + scene;
}

bool GameEngine::showSaveMenu()
{
    while (true)
    {
        term::clear();
        // 标题与槽位摘要用即时输出（cout），避免逐字等待。
        std::cout << std::string(term::kBold)
                  << "════ 存 档 / 读 档 ════"
                  << std::string(term::kReset) << "\n\n";
        for (int s = 1; s <= kSaveSlots; ++s)
        {
            const std::string sum = slotSummary(s);
            std::cout << "  槽位" << s << "："
                      << (sum.empty() ? "[空]" : sum) << "\n";
        }
        std::cout << "\n";

        // 方向键菜单：3 个存档 + 3 个读档 + 返回。
        const std::vector<std::string> ops = {
            "存到槽位 1",
            "存到槽位 2",
            "存到槽位 3",
            "读取槽位 1",
            "读取槽位 2",
            "读取槽位 3",
            "返回游戏",
        };
        const int pick = term::menuSelect(ops, false);

        if (pick >= 0 && pick <= 2)
        {
            saveGame(pick + 1);
            term::slowPrint("\n[ 已存档到槽位 " + std::to_string(pick + 1) + " ]");
            term::pause();
            return false;
        }
        if (pick >= 3 && pick <= 5)
        {
            const int slot = pick - 3 + 1;
            if (loadGame(slot))
            {
                term::slowPrint("\n[ 读档成功，继续游戏 ]");
                term::pause();
                return true; // 通知 presentMenu 重新加载场景
            }
            term::slowPrint("\n[ 槽位为空或存档损坏 ]");
            term::pause();
            continue;
        }
        return false; // 选择“返回游戏”
    }
}

void GameEngine::showProgress() const
{
    term::clear();
    term::slowPrint(std::string(term::kBold) +
                    "════ 已 完 成 分 支 ════" +
                    std::string(term::kReset) + "\n\n");

    //---- 场景探索进度 ----
    const std::size_t totalScenes = story::scenes().size();
    const std::size_t seen = visitedScenes_.size();
    term::slowPrint(std::string(term::kCyan) +
                    "  场景探索：" + std::to_string(seen) + "/" +
                    std::to_string(totalScenes) +
                    std::string(term::kReset) + "\n");

    //---- 结局收集进度 ----
    const std::size_t totalEndings = story::endings().size();
    const std::size_t got = achievedEndings_.size();
    term::slowPrint(std::string(term::kYellow) +
                    "  结局收集：" + std::to_string(got) + "/" +
                    std::to_string(totalEndings) +
                    std::string(term::kReset) + "\n\n");

    term::slowPrint(std::string(term::kBold) + "  【结局一览】" +
                    std::string(term::kReset) + "\n");
    for (const Ending& e : story::endings())
    {
        if (achievedEndings_.find(e.id) != achievedEndings_.end())
        {
            term::slowPrint(std::string(term::kGreen) + "    ✓ " + e.title +
                            std::string(term::kReset) + "\n");
        }
        else
        {
            term::slowPrint(std::string(term::kGray) + "    □ ？？？（尚未达成）" +
                            std::string(term::kReset) + "\n");
        }
    }

    term::slowPrint("\n" + std::string(term::kGray) +
                    "  （每一次死亡回溯，都会保留你的探索足迹。）" +
                    std::string(term::kReset));
    term::pause();
}
