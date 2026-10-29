//==============================================================================
// StoryData.h
//------------------------------------------------------------------------------
// 剧情数据库接口。所有剧情内容（线索 / 场景 / 结局）都在 StoryData.cpp 中
// 以“数据表”的形式给出，本头文件只提供只读查询接口。
//
// 拓展新剧情时，90% 的情况下你只需要：
//   1. 若引入新线索，在 StoryData.cpp 的 kClues 表中追加一条；
//   2. 在 kScenes 表中追加新场景 / 给已有场景追加选项；
//   3. 若引入新结局，在 kEndings 表中追加，并让某个选项的 endingId 指向它。
// 完全不需要修改 GameEngine。
//==============================================================================
#ifndef STORY_DATA_H
#define STORY_DATA_H

#include <cstddef>
#include <string_view>

#include "GameTypes.h"

namespace story
{

// 取得全部线索 / 场景 / 结局（只读）。
const std::vector<Clue>&   clues();
const std::vector<Scene>&  scenes();
const std::vector<Ending>& endings();

// 按 id 查找；找不到时返回 nullptr。
const Clue*   findClue(std::string_view id);
const Scene*  findScene(std::string_view id);
const Ending* findEnding(std::string_view id);

// 某一阶段“应当拥有”的线索总数（用于线索本 / 中枢处显示进度）。
std::size_t clueCountOfTier(ClueTier tier);

// 中枢场景与开场场景的固定 id（引擎回溯 / 启动时使用）。
inline constexpr std::string_view kSceneStart = "start";
inline constexpr std::string_view kSceneHub   = "hub";

} // namespace story

#endif // STORY_DATA_H
