//==============================================================================
// StoryData.cpp
//------------------------------------------------------------------------------
// ★★ 剧情数据库（拓展游戏时主要编辑本文件）★★
//
// 本表包含：
//   kClues   —— 9 条三档线索 + 12 件关联道具；
//   kScenes  —— 80+ 个互相关联的场景节点（含 20 个死亡回溯点）；
//   kEndings —— 3 个结局（基础 / 进阶 / 真结局）。
//
// 场景之间通过“道具 / 线索条件”彼此关联，形成一张探索网：
//   例如：地铁列车(撬棍) → 避难所储藏室 / 居民楼设备间；
//         检查站军车(门禁卡) → 军械库(手枪) → 红区 / 实验室收容区；
//         公园防空洞(防护服) → 诊所隔离区 / 实验室深冷库 / 医院 ICU；
//         B3传单(密码) → A2通讯(坐标) → 地下实验室 → S1/S2/S3 → 真结局。
//
// 所有数据使用 C++20 指定初始化器书写，字段含义见 GameTypes.h。
//==============================================================================
#include "StoryData.h"

#include <algorithm>
#include <functional>

namespace story
{

//==============================================================================
// 一、线索与道具表
//==============================================================================
namespace
{

const std::vector<Clue> kClues =
{
    //---------------- 基础线索（现象层）-------------------------------------
    Clue{
        .id = "B1",
        .tier = ClueTier::Basic,
        .name = "被重新封装的矿泉水",
        .detail = "批号 ECHO-W-00，生产日期早于灰雾日 11 天，瓶口有二次高温封装"
                  "痕迹，瓶底有絮状沉淀。说明污染在灾难前就被蓄意装入瓶中。"
    },
    Clue{
        .id = "B2",
        .tier = ClueTier::Basic,
        .name = "实验动物耳标",
        .detail = "中央公园的变异狗佩戴 ECHO-K 系列编号金属耳标。它们不是偶然"
                  "感染的流浪动物，而是来自某座设施、被编号管理的“资产”。"
    },
    Clue{
        .id = "B3",
        .tier = ClueTier::Basic,
        .name = "代号《回声》的政府传单",
        .detail = "空投传单要求居民禁止饮用一切市政供水，空白处留有无线电频率"
                  "与手写密码，暗示“回声”是一场有组织行动的代号。"
    },

    //---------------- 进阶线索（阴谋层）-------------------------------------
    Clue{
        .id = "A1",
        .tier = ClueTier::Advanced,
        .name = "流调员的早期病例",
        .detail = "所有早期患者同日抵达同一供水点，如同被“同时按下开关”，不符合"
                  "自然传染病的传播规律。末页红笔批注：ECHO？非自然传播？"
    },
    Clue{
        .id = "A2",
        .tier = ClueTier::Advanced,
        .name = "破译的军方加密通讯",
        .detail = "ECHO 样本已转移至 D 区地下设施（坐标 7-3-9）；“零号宿主”仍在"
                  "“锚点”周边活动，一旦接触立即启动回溯协议。"
    },
    Clue{
        .id = "A3",
        .tier = ClueTier::Advanced,
        .name = "死者的水箱作业日记",
        .detail = "灰雾日前 5 天，ECHO 人员在楼顶水箱“检修”；前一天有人饮下自来水"
                  "后发病。日记断言：水是被“提前写好”的。"
    },

    //---------------- 高级线索（真相层）-------------------------------------
    Clue{
        .id = "S1",
        .tier = ClueTier::Supreme,
        .name = "ECHO-0 初代样本标签",
        .detail = "初代株提取自“时间锚点介质”，标签中没有任何“武器 / 投放”字样——"
                  "它从诞生起就不是为了被投放，而是为了被“带回”。"
    },
    Clue{
        .id = "S2",
        .tier = ClueTier::Supreme,
        .name = "研究员的循环录音",
        .detail = "第 97 次循环记录：回声是时间介质，病毒随意识回溯进入更早的"
                  "时间线；零号宿主，就是那个反复在锚点醒来的人。"
    },
    Clue{
        .id = "S3",
        .tier = ClueTier::Supreme,
        .name = "主控室真相文件",
        .detail = "病毒唯一的传播途径是回溯。终止方式：集齐全部真相后停止回溯，"
                  "哪怕这条时间线里的自己终将死去。"
    },

    //---------------- 关联道具（Item：不计入推理进度）-----------------------
    Clue{ .id = "tool_crowbar",   .tier = ClueTier::Item,
          .name = "生锈的撬棍",   .detail = "地铁废弃列车里找到，能撬开锈蚀的门。" },
    Clue{ .id = "tool_medicine",  .tier = ClueTier::Item,
          .name = "急救药品",     .detail = "超市冷库中保存完好的抗生素与止血药，能救人。" },
    Clue{ .id = "tool_radiopart", .tier = ClueTier::Item,
          .name = "电台电子管",   .detail = "军车仪表盘上拆下的备件，能修好各种无线电设备。" },
    Clue{ .id = "tool_keycard",   .tier = ClueTier::Item,
          .name = "军方门禁卡",   .detail = "军官遗体上的磁卡，可打开军械库与实验室主管室。" },
    Clue{ .id = "tool_weapon",    .tier = ClueTier::Item,
          .name = "九二式手枪",   .detail = "军械库中尚有弹药的手枪，危急时刻能保命。" },
    Clue{ .id = "tool_protection",.tier = ClueTier::Item,
          .name = "全套防护服",   .detail = "公园防空洞封存的防护服，可抵御高浓度感染环境。" },
    Clue{ .id = "tool_map",       .tier = ClueTier::Item,
          .name = "手绘城区地图", .detail = "超市天台绘制，标出了信号塔、供水厂与红区方位。" },
    Clue{ .id = "tool_wrench",    .tier = ClueTier::Item,
          .name = "管钳扳手",     .detail = "居民楼设备间工具，可关闭供水总阀。" },
    Clue{ .id = "tool_freeze",    .tier = ClueTier::Item,
          .name = "冷冻试剂瓶",   .detail = "超市冷库中贴着 ECHO 标签、被异常低温保存的试剂。" },
    Clue{ .id = "tool_scales",    .tier = ClueTier::Item,
          .name = "变异鱼鳞",     .detail = "公园池塘死鱼身上的鳞片，皮下组织呈青黑色丝状。" },
    Clue{ .id = "tool_tankres",   .tier = ClueTier::Item,
          .name = "水箱残留样本", .detail = "居民楼天台水箱内壁刮下的残留物，与 ECHO 絮状物一致。" },
    Clue{ .id = "tool_codes",     .tier = ClueTier::Item,
          .name = "幸存者密码本", .detail = "501 幸存者所赠，记着供水厂与医院门禁的数字密码。" },

    //==========================================================================
    // ===== 扩展篇 · 新增三档线索（现象/阴谋/真相的补全）=====
    //==========================================================================

    //---------------- 基础线索（新增 B4、B5）--------------------------------
    Clue{
        .id = "B4",
        .tier = ClueTier::Basic,
        .name = "孩子们的壁画",
        .detail = "第一小学的孩子们在墙上画满了“醒来的人”：每一个小人胸口都连着一根"
                  "线，线的另一端，全部指向同一个发光的圆圈。孩子的眼睛，往往最先看见真相。"
    },
    Clue{
        .id = "B5",
        .tier = ClueTier::Basic,
        .name = "港口货轮的异常舱单",
        .detail = "港口“远星号”货轮的舱单显示：灰雾日前 12 天，一批未申报的“密封医疗"
                  "容器”在午夜被装上船，目的地一栏写着“循环港”——一个地图上不存在的地方。"
    },

    //---------------- 进阶线索（新增 A4、A5）--------------------------------
    Clue{
        .id = "A4",
        .tier = ClueTier::Advanced,
        .name = "叛逃科学家的日志",
        .detail = "ECHO 计划核心成员苏敏的日志：锚点并非偶然开启，是有人在第 0 次循环里"
                  "“自愿”成为介质；她断言：“我们都不是受害者，是某个人的选择，把我们困在这里。”"
    },
    Clue{
        .id = "A5",
        .tier = ClueTier::Advanced,
        .name = "管廊深处的时间实验档案",
        .detail = "城市综合管廊最深处封存着“回声计划”前身——“灯塔工程”的档案：早在 30 年前，"
                  "就有人在同一坐标进行过时间锚定实验，且实验记录的最后一页，写着你的名字。"
    },

    //---------------- 高级线索（新增 S4、S5）--------------------------------
    Clue{
        .id = "S4",
        .tier = ClueTier::Supreme,
        .name = "陈教授的时间理论手稿",
        .detail = "图书馆陈教授毕生研究：时间不是一条线，而是“被观察才存在”的叠加态。"
                  "锚点之所以能循环，是因为有一个意识“拒绝让它结束”。终结循环，不是毁灭锚点，"
                  "而是让那个意识，真正地“放下”。"
    },
    Clue{
        .id = "S5",
        .tier = ClueTier::Supreme,
        .name = "植物园的天然抗体样本",
        .detail = "植物园温室深处，一株在灰雾中依然翠绿的植物，其汁液能让青黑丝状物"
                  "“沉睡”而非死亡。它证明：末日并非只能以牺牲终结，生命，自己找到了与病毒共存的方式。"
    },

    //==========================================================================
    // ===== 扩展篇 · 新增道具（8 件）=====
    //==========================================================================
    Clue{ .id = "tool_bomb",       .tier = ClueTier::Item,
          .name = "军用炸药",       .detail = "旧防空洞军火库的 C4，足以从物理上彻底炸毁锚点。" },
    Clue{ .id = "tool_boat_key",   .tier = ClueTier::Item,
          .name = "远星号船钥匙", .detail = "老船长所赠，能启动港口那艘加满油的远洋货轮。" },
    Clue{ .id = "tool_flare",      .tier = ClueTier::Item,
          .name = "信号弹",       .detail = "高架废墟老赵的信号弹，能在最高处向远方发出求救。" },
    Clue{ .id = "tool_old_map",    .tier = ClueTier::Item,
          .name = "旧军事地图",   .detail = "标注了所有地下设施、管廊与防空洞的军方绝密地图。" },
    Clue{ .id = "tool_tape",       .tier = ClueTier::Item,
          .name = "苏敏的录音带", .detail = "叛逃科学家留下的最后陈述，记录着第 0 次循环的真相。" },
    Clue{ .id = "tool_radio_key",  .tier = ClueTier::Item,
          .name = "广播总控钥匙", .detail = "旧广播大楼顶层发射机的总控钥匙，能开启全国频段。" },
    Clue{ .id = "tool_antidote",   .tier = ClueTier::Item,
          .name = "植物抗体试剂", .detail = "用植物园抗体样本提纯的试剂，可让感染者“沉睡”病毒。" },
    Clue{ .id = "tool_diary",      .tier = ClueTier::Item,
          .name = "第0次循环日记", .detail = "灯塔工程遗物，扉页照片上的人，和你一模一样。" },
};

//==============================================================================
// 二、场景表（按区域分块，注释清晰）
//==============================================================================
const std::vector<Scene> kScenes =
{
    //==========================================================================
    // 【区域 0】序章 & 中枢
    //==========================================================================
    Scene{
        .id = "start",
        .title = "",
        .paragraphs =
        {
            "================================================\n"
            "        E C H O   O F   D O O M S D A Y\n"
            "                  末  日  回  声\n"
            "================================================\n",
            "灰雾降临的第七天。",
            "你在地铁深处的应急避难所里醒来。头顶灯管发出垂死的嗡鸣，应急广播"
            "早已只剩下循环的电流声。",
            "你记得一件不该记得的事——就在昨天，你死了。被带着铁锈味的饮水杀死；"
            "被长着人手的野狗撕开；被实验室警报声中涌出的白雾吞没……",
            "而每一次死亡，你都会回到这里，回到这张冰冷的行军床上。世界重置，"
            "伤痕消失，只有记忆，像烧红的铁印，留在脑海里。",
            "你把那些用命换来的信息称为“线索”。你隐约明白：这座城市的末日藏着"
            "一个答案，而答案，需要你一次次走进雾里，把它拼完整。"
        },
        .choices =
        {
            Choice{ .text = "睁开眼睛，从行军床上坐起", .target = "hub" }
        }
    },

    Scene{
        .id = "hub",
        .title = "避难所 · 时间锚点",
        .paragraphs =
        {
            "避难所是你唯一的锚点。墙上的粉笔痕迹记录着你发动回溯的次数。",
            "四个方向通向城市的不同角落，更深处还有地铁隧道与排水渠。收音机里，"
            "一个沙哑的女声在循环：“……不要饮用自来水……不要相信穿白色防护服的人……”",
            "你检查了一下背包，深吸一口气。"
        },
        .choices =
        {
            //--- 避难所内部 ---
            Choice{ .text = "查看墙壁上密密麻麻的刻字",       .target = "shelter_wall" },
            Choice{ .text = "用撬棍撬开储藏室锈死的门",       .target = "shelter_storage",
                    .requireAll = { "tool_crowbar" } },
            Choice{ .text = "用电子管修好角落的收音机",       .target = "shelter_radio",
                    .requireAll = { "tool_radiopart" } },
            //--- 五大表层区域 ---
            Choice{ .text = "【东】前往惠民超市",             .target = "market_outside" },
            Choice{ .text = "【北】前往中央公园",             .target = "park_entrance" },
            Choice{ .text = "【西】前往军警检查站",           .target = "cp_outside" },
            Choice{ .text = "【南】前往社区诊所",             .target = "clinic_lobby" },
            Choice{ .text = "【东南】搜索居民楼",             .target = "apt_lobby" },
            //--- 深层区域 ---
            Choice{ .text = "【下】沿地铁隧道探索",           .target = "metro_entrance" },
            Choice{ .text = "沿排水渠寻找 D 区地下设施",      .target = "sewer_entrance",
                    .requireAll = { "A2" } },
            Choice{ .text = "按地图前往信号塔",               .target = "tower_base",
                    .requireAll = { "tool_map" } },
            Choice{ .text = "按地图前往城市供水厂",           .target = "wp_gate",
                    .requireAll = { "tool_map" } },
            //--- 扩展篇 · 新区域（需地图）---
            Choice{ .text = "【东北】前往旧广播大楼",         .target = "radio_outside",
                    .requireAll = { "tool_map" } },
            Choice{ .text = "【东·远】前往港口码头",         .target = "dock_gate",
                    .requireAll = { "tool_map" } },
            Choice{ .text = "【西南】前往第一小学",           .target = "school_gate",
                    .requireAll = { "tool_map" } },
            Choice{ .text = "【西北】前往高架废墟",           .target = "overpass_base",
                    .requireAll = { "tool_map" } },
            Choice{ .text = "【西·远】前往植物园温室",       .target = "garden_gate",
                    .requireAll = { "tool_map" } },
            Choice{ .text = "【南·远】前往城市图书馆",       .target = "library_gate",
                    .requireAll = { "tool_map" } },
            //--- 扩展篇 · 深层新区域（需旧军事地图）---
            Choice{ .text = "按旧军事地图进入城市综合管廊",   .target = "pipe_entrance",
                    .requireAll = { "tool_old_map" } },
            Choice{ .text = "按旧军事地图寻找旧军事防空洞",   .target = "bunker_gate",
                    .requireAll = { "tool_old_map" } },
            //--- 推理与结局（原有）---
            Choice{ .text = "【推理】整合基础线索，得出结论",  .target = "",
                    .requireAll = { "B1", "B2", "B3" },        .endingId = "ending_basic" },
            Choice{ .text = "【推理】整合进阶线索，得出结论",  .target = "",
                    .requireAll = { "A1", "A2", "A3" },        .endingId = "ending_advanced" },
            //--- 扩展篇 · 新结局（在锚点/特殊场景触发）---
            Choice{ .text = "走向感知中的“锚点核心”",         .target = "core_gate",
                    .requireAll = { "B1","B2","B3","A1","A2","A3",
                                    "S1","S2","S3" } },
            //--- 退出 ---
            Choice{ .text = "放下一切，离开游戏",             .target = "__quit" },
        }
    },

    //==========================================================================
    // 【区域 0.5】避难所内部（3 个关联场景）
    //==========================================================================
    Scene{
        .id = "shelter_wall",
        .title = "避难所 · 墙壁刻字",
        .paragraphs =
        {
            "墙壁上留着前几任“醒来者”的刻字，有些已经模糊：",
            "“第 3 次。别喝自来水。”\n"
            "“第 11 次。别相信穿白防护服的人，他们不是来救我们的。”\n"
            "“第 40 次。我好像……在雾里看见了另一个我。”\n"
            "“第 96 次。如果你读得到这些字，去地下，去看那段录音。”",
            "刻字到这里戛然而止，最后一道划痕深得几乎凿穿墙壁。"
        },
        .choices = { Choice{ .text = "回到锚点", .target = "hub" } }
    },
    Scene{
        .id = "shelter_storage",
        .title = "避难所 · 储藏室",
        .paragraphs =
        {
            "你用撬棍撬开锈死的门。储藏室里积着灰，货架上只剩几截蜡烛和一本"
            "被水泡过的《应急手册》。",
            "手册扉页有人潦草写着：“锚点不是避难所，是病毒的‘插座’。每次醒来，"
            "都是它在充电。”"
        },
        .choices = { Choice{ .text = "收起手册，回到锚点", .target = "hub" } }
    },
    Scene{
        .id = "shelter_radio",
        .title = "避难所 · 修好的收音机",
        .paragraphs =
        {
            "你换上电子管，收音机嘶嘶响了几声，一个清晰的女声传出——正是循环"
            "广播里的那个人，但这一次她在说新的话：",
            "“……如果你能听到完整频段，说明你也在循环里。我在学校的救援点，"
            "红区深处。求求你，来之前，先找到真相，否则你只会一遍遍死在我面前。”",
            "信号随即中断。"
        },
        .choices = { Choice{ .text = "记住她的话，回到锚点", .target = "hub" } }
    },

    //==========================================================================
    // 【区域 1】惠民超市（7 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "market_outside",
        .title = "惠民超市 · 门外",
        .paragraphs =
        {
            "卷帘门半开着，像一张垂死的嘴。门口血迹向内延伸，消失在黑暗里，"
            "里面传来潮湿的咀嚼声。",
            "建筑外侧有一架生锈的消防梯，通向超市天台。"
        },
        .choices =
        {
            Choice{ .text = "从半开的正门进去",       .target = "market_front" },
            Choice{ .text = "绕到员工通道",           .target = "market_back" },
            Choice{ .text = "攀爬消防梯上天台",       .target = "market_roof" },
            Choice{ .text = "返回避难所",             .target = "hub" },
        }
    },
    Scene{
        .id = "market_front",
        .title = "惠民超市 · 正门",
        .paragraphs =
        {
            "货架倒了一半。一个佝偻的身影伏在收银台后咀嚼，脖子以活人做不到的"
            "角度歪着。",
            "一只空罐头滚到你脚边。"
        },
        .choices =
        {
            Choice{ .text = "扔出罐头，引开它",      .target = "market_warehouse" },
            Choice{ .text = "压低身子从货架底爬过去",.target = "market_warehouse" },
            Choice{ .text = "硬冲过去",              .target = "death_market" },
            Choice{ .text = "退回门外",              .target = "market_outside" },
        }
    },
    Scene{
        .id = "market_back",
        .title = "惠民超市 · 员工通道",
        .paragraphs =
        {
            "通道狭窄安静。一具员工尸体靠在墙上，手里攥着冷库钥匙，皮肤下有"
            "青黑色线条在游走。"
        },
        .choices =
        {
            Choice{ .text = "小心取下钥匙，进入冷库",.target = "market_coldroom" },
            Choice{ .text = "翻动尸体寻找其他东西",  .target = "death_market" },
            Choice{ .text = "不碰尸体，退回去",      .target = "market_outside" },
        }
    },
    Scene{
        .id = "market_warehouse",
        .title = "惠民超市 · 仓库",
        .paragraphs =
        {
            "仓库里堆满“应急供水”，瓶身标签崭新，瓶口却有一圈极细的二次封装"
            "痕迹。",
            "深处是经理室，门缝里透出一线光。"
        },
        .choices =
        {
            Choice{ .text = "检查整箱矿泉水的封装",  .target = "market_b1",
                    .grantedClue = "", .requireNone = { "B1" } },
            Choice{ .text = "拧开一瓶“应急供水”喝下",.target = "death_water" },
            Choice{ .text = "进入经理室",            .target = "market_office" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "market_b1",
        .title = "惠民超市 · 线索",
        .paragraphs =
        {
            "你举起一瓶水对着光：瓶底沉着几缕几乎看不见的絮状物，批号 ECHO-W-00，"
            "生产日期比灰雾日早了整整 11 天。",
            "不是水源被污染了——是有人，在灾难发生之前，把污染装进了瓶子。",
            "你没有带走任何一瓶水，把批号和封瓶痕迹刻进脑海。"
        },
        .choices =
        {
            Choice{ .text = "（收好线索）",          .target = "market_warehouse",
                    .grantedClue = "B1" },
        }
    },
    Scene{
        .id = "market_coldroom",
        .title = "惠民超市 · 冷库",
        .paragraphs =
        {
            "你用钥匙打开冷库，寒气扑面而来。货架上除了冻货，还有一箱保存完好的"
            "急救药品；最里面的冰架上，单独冻着一支贴 ECHO 标签的试剂瓶。",
            "冷库门在身后缓缓回弹。"
        },
        .choices =
        {
            Choice{ .text = "取走药品和冷冻试剂",    .target = "market_cold_got",
                    .requireNone = { "tool_freeze" } },
            Choice{ .text = "让冷库门彻底关上，摸黑停留", .target = "death_coldroom" },
            Choice{ .text = "立即离开冷库",          .target = "market_back" },
        }
    },
    Scene{
        .id = "market_cold_got",
        .title = "惠民超市 · 冷库",
        .paragraphs =
        {
            "你把药品和冷冻试剂塞进背包。试剂瓶冷得反常，即便隔着保温层，仍让你"
            "指尖发麻。",
            "你用卡子顶住冷库门，迅速退了出去。"
        },
        .choices =
        {
            Choice{ .text = "（收好药品与冷冻试剂）",  .target = "market_back",
                    .grantedClues = { "tool_medicine", "tool_freeze" } },
        }
    },
    Scene{
        .id = "market_office",
        .title = "惠民超市 · 经理室",
        .paragraphs =
        {
            "电脑居然还亮着。进货记录里，灰雾日前 11 天有一批“市政公益供水”入库，"
            "供货方一栏写着 ECHO，经手人签名处是空白。",
            "记录最下方有一行未发出的留言：“他们说这是政府物资，可我从没见过"
            "送水的人，货是半夜自己出现在门口的。”"
        },
        .choices =
        {
            Choice{ .text = "记下记录，返回仓库",    .target = "market_warehouse" },
        }
    },
    Scene{
        .id = "market_roof",
        .title = "惠民超市 · 天台",
        .paragraphs =
        {
            "天台上风很大，吹散了些许灰雾。你第一次俯瞰整座城市：城北的信号塔、"
            "城东的供水厂、城西被铁丝网封死的红区，尽收眼底。",
            "你凭记忆在一张广告纸背面画出城区地图。"
        },
        .choices =
        {
            Choice{ .text = "（收好手绘地图）",      .target = "market_outside",
                    .grantedClue = "tool_map", .requireNone = { "tool_map" } },
            Choice{ .text = "直接沿消防梯下去",      .target = "market_outside" },
        }
    },
    Scene{
        .id = "death_market",
        .title = "",
        .paragraphs =
        {
            "那东西猛地抬头，速度快得不像活人。冰冷的手指扣住你的喉咙，你被拖进"
            "黑暗，咀嚼声变成了你自己的。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_water",
        .title = "",
        .paragraphs =
        {
            "水很凉，带着一丝微甜。半小时后，高热从脊柱炸开，你感到皮肤下有东西"
            "在游动、生长，喉咙里涌出不属于你的低语。水壶从手中滑落。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_coldroom",
        .title = "",
        .paragraphs =
        {
            "冷库门在身后咔嗒一声彻底锁死。寒气一点点渗进骨头，你拍打着门，"
            "声音被厚重的冰层吞没。黑暗中，连哈气都结成了霜。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 2】中央公园（6 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "park_entrance",
        .title = "中央公园 · 入口",
        .paragraphs =
        {
            "公园被灰雾吞没一半，成群的死鸟铺在草坪上，像一层灰色的雪。",
            "长椅上坐着一位一动不动的老人；灌木丛后传来低低的呜咽，夹杂着金属"
            "碰撞的轻响。更深处可见池塘与一座半掩的防空洞。"
        },
        .choices =
        {
            Choice{ .text = "蹲在灌木后观察",        .target = "park_bush" },
            Choice{ .text = "走向长椅上的老人",      .target = "park_oldman" },
            Choice{ .text = "前往池塘",              .target = "park_pond" },
            Choice{ .text = "前往防空洞",            .target = "park_bunker" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "park_bush",
        .title = "中央公园 · 灌木丛",
        .paragraphs =
        {
            "你屏住呼吸。三只瘦骨嶙峋的流浪狗走出雾来，皮肤下有东西在不规则地"
            "起伏，像水流穿过薄冰。",
            "每只狗的左耳都钉着金属耳标：ECHO-K-07、K-08、K-09。",
            "它们不是被感染的流浪动物——它们是“编号”。"
        },
        .choices =
        {
            Choice{ .text = "（记下耳标）",          .target = "park_entrance",
                    .grantedClue = "B2", .requireNone = { "B2" } },
            Choice{ .text = "捡起石头驱赶它们",      .target = "death_park" },
        }
    },
    Scene{
        .id = "park_oldman",
        .title = "中央公园 · 长椅",
        .paragraphs =
        {
            "老人早已没有呼吸，怀里紧紧抱着孩子的照片，背面写着：“西边检查站有"
            "空投，别喝任何水，爸爸对不起你。”",
            "你把照片放回他怀里，站起身。"
        },
        .choices = { Choice{ .text = "回到公园入口", .target = "park_entrance" } }
    },
    Scene{
        .id = "park_pond",
        .title = "中央公园 · 池塘",
        .paragraphs =
        {
            "池塘水面漂浮着大片死鱼，鱼腹朝天地挤在岸边。一条鱼的鳃还在抽动，"
            "皮下组织呈青黑色丝状。",
            "池水看起来异常清澈，清澈得不正常。"
        },
        .choices =
        {
            Choice{ .text = "刮下一片鱼鳞收好",      .target = "park_pond_got",
                    .requireNone = { "tool_scales" } },
            Choice{ .text = "弯腰捧起池水查看",      .target = "death_pond" },
            Choice{ .text = "离开池塘",              .target = "park_entrance" },
        }
    },
    Scene{
        .id = "park_pond_got",
        .title = "中央公园 · 池塘",
        .paragraphs =
        {
            "你把鳞片装进空袋。鳞片在掌心微微发凉，青黑的纹路像极了 B1 瓶底的"
            "絮状物——它们来自同一个东西。"
        },
        .choices =
        {
            Choice{ .text = "（收好鳞片）",          .target = "park_pond",
                    .grantedClue = "tool_scales" },
        }
    },
    Scene{
        .id = "park_bunker",
        .title = "中央公园 · 防空洞",
        .paragraphs =
        {
            "防空洞铁门挂着密码锁，锁盘上有动物抓挠的痕迹。",
            "你想起耳标上的编号——K-07、K-08、K-09。"
        },
        .choices =
        {
            Choice{ .text = "输入耳标编号 070809",   .target = "park_bunker_open",
                    .requireAll = { "B2" } },
            Choice{ .text = "盲目尝试其他密码",      .target = "death_park" },
            Choice{ .text = "离开防空洞",            .target = "park_entrance" },
        }
    },
    Scene{
        .id = "park_bunker_open",
        .title = "中央公园 · 防空洞",
        .paragraphs =
        {
            "锁扣弹开。防空洞内整齐封存着一箱物资：罐头、净水片，还有一套叠得"
            "平整、从未拆封的全套防护服。",
            "墙上贴着一张旧地图，标注着 D 区地下设施的通风口，与外界的排水渠"
            "相连。"
        },
        .choices =
        {
            Choice{ .text = "（穿上防护服带走）",    .target = "park_entrance",
                    .grantedClue = "tool_protection", .requireNone = { "tool_protection" } },
            Choice{ .text = "记下通风口位置",        .target = "park_entrance" },
        }
    },
    Scene{
        .id = "death_park",
        .title = "",
        .paragraphs =
        {
            "响声在雾里传出很远。十几双眼睛在雾中同时亮起，它们从四面八方涌来，"
            "皮毛下是游动的青黑色血管——你这才明白，“三只”从来不是全部。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_pond",
        .title = "",
        .paragraphs =
        {
            "池水触手的瞬间，无数青黑色细丝缠上你的手腕，把你向水里拽。你看见"
            "水面下密密麻麻全是鱼——和一张张泡得发白的人脸。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 3】军警检查站（6 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "cp_outside",
        .title = "军警检查站 · 外围",
        .paragraphs =
        {
            "路障东倒西歪，一辆军车侧翻在路边，车门弹孔像一排黑色眼睛。",
            "指挥帐篷还在，里面的无线电亮着微弱红灯；军车后方是军械库帐篷，"
            "更深处有一道通向红区的密封铁门。"
        },
        .choices =
        {
            Choice{ .text = "搜查指挥帐篷",          .target = "cp_tent" },
            Choice{ .text = "调试无线电",            .target = "cp_radio" },
            Choice{ .text = "搜查侧翻的军车",        .target = "cp_vehicle" },
            Choice{ .text = "用门禁卡打开军械库",    .target = "cp_armory",
                    .requireAll = { "tool_keycard" } },
            Choice{ .text = "推开红区密封铁门",      .target = "cp_deepgate" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "cp_tent",
        .title = "检查站 · 指挥帐篷",
        .paragraphs =
        {
            "空投物资箱散落一地，最里面的铁箱还封着。",
            "帐篷角落，一颗拔掉保险的诡雷用细线连着箱盖，线在暗处闪着微光。"
        },
        .choices =
        {
            Choice{ .text = "小心收起油包里的传单",  .target = "cp_b3",
                    .requireNone = { "B3" } },
            Choice{ .text = "用力去拽最里面的箱子",  .target = "death_cp_trap" },
            Choice{ .text = "退出帐篷",              .target = "cp_outside" },
        }
    },
    Scene{
        .id = "cp_b3",
        .title = "检查站 · 传单",
        .paragraphs =
        {
            "传单印着红色通告：“代号【回声】行动：禁止饮用一切市政供水，居民"
            "就地等待转移。”",
            "空白处有人用铅笔写下一串频率与一句话：“他们说这是演习，可我见过"
            "他们在水箱里放东西。”"
        },
        .choices =
        {
            Choice{ .text = "（收好传单）",          .target = "cp_tent",
                    .grantedClue = "B3" },
        }
    },
    Scene{
        .id = "cp_radio",
        .title = "检查站 · 无线电",
        .paragraphs =
        {
            "喇叭里是杂乱电流声，偶尔夹杂变形的音节，像有人在很远的地方用密码"
            "说话。"
        },
        .choices =
        {
            Choice{ .text = "按传单频率与密码调试",  .target = "cp_a2",
                    .requireAll = { "B3" }, .requireNone = { "A2" } },
            Choice{ .text = "盲目旋转频率旋钮",      .target = "death_cp_radio" },
            Choice{ .text = "返回",                  .target = "cp_outside" },
        }
    },
    Scene{
        .id = "cp_a2",
        .title = "检查站 · 破译的通讯",
        .paragraphs =
        {
            "噪音突然分开，一个冷静的男声清晰传出：",
            "“……ECHO 样本已转移至 D 区地下设施，坐标 7-3-9。重复，零号宿主仍在"
            "锚点周边活动，一旦接触立即启动回溯协议……”",
            "录音在此循环。你记下了坐标——入口就在城市排水渠尽头。"
        },
        .choices =
        {
            Choice{ .text = "（记下坐标）",          .target = "cp_radio",
                    .grantedClue = "A2" },
        }
    },
    Scene{
        .id = "cp_vehicle",
        .title = "检查站 · 军车",
        .paragraphs =
        {
            "军车驾驶室被压得变形，仪表盘上插着一支完好的电子管；后排军官的"
            "遗体上，挂着一张军方门禁卡。"
        },
        .choices =
        {
            Choice{ .text = "拆下电子管",            .target = "cp_vehicle",
                    .grantedClue = "tool_radiopart", .requireNone = { "tool_radiopart" } },
            Choice{ .text = "取下门禁卡",            .target = "cp_vehicle",
                    .grantedClue = "tool_keycard",   .requireNone = { "tool_keycard" } },
            Choice{ .text = "离开军车",              .target = "cp_outside" },
        }
    },
    Scene{
        .id = "cp_armory",
        .title = "检查站 · 军械库",
        .paragraphs =
        {
            "军械库里大部分武器已被带走，只剩一支压在箱底的九二式手枪和两个"
            "弹匣，角落里还有几枚震撼弹。"
        },
        .choices =
        {
            Choice{ .text = "取走手枪和震撼弹",      .target = "cp_armory_got",
                    .requireNone = { "tool_weapon" } },
            Choice{ .text = "空手离开",              .target = "cp_outside" },
        }
    },
    Scene{
        .id = "cp_armory_got",
        .title = "检查站 · 军械库",
        .paragraphs = { "你把枪上膛、插在腰后。有了它，红区那道铁门似乎也不再那么可怕。" },
        .choices =
        {
            Choice{ .text = "（收好武器）",          .target = "cp_outside",
                    .grantedClue = "tool_weapon" },
        }
    },
    Scene{
        .id = "cp_deepgate",
        .title = "检查站 · 红区铁门",
        .paragraphs =
        {
            "铁门后是被铁丝网封死的红区，雾气浓得几乎看不见指尖，远处隐约有"
            "教堂的尖顶。"
        },
        .choices =
        {
            Choice{ .text = "全副武装，推门进入红区",.target = "redzone_street",
                    .requireAll = { "tool_weapon", "tool_protection" } },
            Choice{ .text = "没有把握，先退回去",    .target = "cp_outside" },
        }
    },
    Scene{
        .id = "death_cp_trap",
        .title = "",
        .paragraphs =
        {
            "细线在指缝间绷紧，你只来得及看清那是一颗老式手雷。白光吞没了帐篷，"
            "也吞没了这条时间线。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_cp_radio",
        .title = "",
        .paragraphs =
        {
            "电台发出尖锐啸叫。哨卡外的雾里，无数身影同时转向帐篷，你听见了"
            "奔跑声——不是一个人的，是几十个。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 4】社区诊所（4 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "clinic_lobby",
        .title = "社区诊所 · 大厅",
        .paragraphs =
        {
            "玻璃门碎了一半，药味和甜腻的腐味混在一起。",
            "治疗室门虚掩着，里面传来压抑喘息；档案室黑得没有一丝光；走廊尽头"
            "是检验室与贴着警示条的隔离区。"
        },
        .choices =
        {
            Choice{ .text = "推开治疗室的门",        .target = "clinic_treatment" },
            Choice{ .text = "摸黑走进档案室",        .target = "death_clinic_archive" },
            Choice{ .text = "进入检验室",            .target = "clinic_lab" },
            Choice{ .text = "穿防护服进入隔离区",    .target = "clinic_quarantine",
                    .requireAll = { "tool_protection" } },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "clinic_treatment",
        .title = "诊所 · 治疗室",
        .paragraphs =
        {
            "一个中年男人靠在药柜旁，腿被倒塌的铁架砸中，鲜血浸透裤腿。他看见"
            "你，又警惕地望向不断作响的大门。",
            "“别……别先管药，”他咬牙说，“门外那些东西，快进来了。”"
        },
        .choices =
        {
            Choice{ .text = "先撕外套为他包扎止血",  .target = "clinic_a1",
                    .requireNone = { "A1" } },
            Choice{ .text = "先翻他身后的药柜",      .target = "death_clinic_rob" },
            Choice{ .text = "离开治疗室",            .target = "clinic_lobby" },
        }
    },
    Scene{
        .id = "clinic_a1",
        .title = "诊所 · 早期病例",
        .paragraphs =
        {
            "你扎紧他的伤口。男人缓过一口气，从贴身口袋掏出一叠湿透的纸。",
            "“我爱人是流调员，失踪前留下的。所有早期病人同一天去了同一个供水点，"
            "那不是传染病该有的速度——他们像被同时按下了开关。”",
            "病历最后一页红笔圈着：ECHO？非自然传播？"
        },
        .choices =
        {
            Choice{ .text = "（收好病例）",          .target = "clinic_lobby",
                    .grantedClue = "A1" },
        }
    },
    Scene{
        .id = "clinic_lab",
        .title = "诊所 · 检验室",
        .paragraphs =
        {
            "显微镜下还残留着样本涂片。你用 B1 批号调来的水样记录比对，又举起"
            "超市冷库带来的冷冻试剂——涂片里的青黑丝状物，与两者完全吻合。",
            "检验单上写着：“非已知病原体，遇回溯性观测活性异常升高。”"
        },
        .choices =
        {
            Choice{ .text = "记下结果，返回大厅",    .target = "clinic_lobby",
                    .requireAll = { "B1" } },
            Choice{ .text = "没有依据，直接离开",    .target = "clinic_lobby" },
        }
    },
    Scene{
        .id = "clinic_quarantine",
        .title = "诊所 · 隔离区",
        .paragraphs =
        {
            "防护服里，你靠近隔离病房。玻璃后的早期病人还清醒，他看见你，反而"
            "平静地笑了：",
            "“你也在循环里，对不对？我数过，我是第 3 个‘最早’的病人，可每一次"
            "醒来，总有人比我更早。去医院停尸房看看吧，那里躺着第 0 个。”"
        },
        .choices =
        {
            Choice{ .text = "记下这条线索，退出隔离区", .target = "clinic_lobby" },
        }
    },
    Scene{
        .id = "death_clinic_archive",
        .title = "",
        .paragraphs =
        {
            "手电扫过铁皮柜，最里面的柜门后蹲着的东西猛地回头——白大褂还穿在"
            "身上，胸牌名字被血污盖住。你连呼救都来不及。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_clinic_rob",
        .title = "",
        .paragraphs =
        {
            "你越过他去抓抗生素。治疗室的门被撞开，他没有为你包扎，你也没有"
            "为他停留——门外的东西公平地收走了你们两个。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 5】居民楼（5 个普通场景 + 3 个死亡场景）
    //==========================================================================
    Scene{
        .id = "apt_lobby",
        .title = "居民楼 · 楼道",
        .paragraphs =
        {
            "楼道里贴满停水通知，最早的一张是灰雾日前 11 天。",
            "402 室门虚掩、飘出腐臭；地下室门缝透出一线摇晃灯光；楼梯通向天台"
            "水箱；501 室门紧闭、里面传来微弱敲击；设备间的门锈死了。"
        },
        .choices =
        {
            Choice{ .text = "推开 402 室",            .target = "apt_402" },
            Choice{ .text = "下地下室",              .target = "death_apt_basement" },
            Choice{ .text = "上天台",                .target = "apt_roof" },
            Choice{ .text = "敲 501 室的门",          .target = "apt_501" },
            Choice{ .text = "用撬棍开设备间",        .target = "apt_utility",
                    .requireAll = { "tool_crowbar" } },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "apt_402",
        .title = "居民楼 · 402 室",
        .paragraphs =
        {
            "桌上摊着一本日记，主人蜷在床上，早已没了声息。衣柜门半开，塞满"
            "没来得及带走的行李。"
        },
        .choices =
        {
            Choice{ .text = "阅读日记",              .target = "apt_a3",
                    .requireNone = { "A3" } },
            Choice{ .text = "翻找衣柜",              .target = "death_apt_loot" },
            Choice{ .text = "离开 402",              .target = "apt_lobby" },
        }
    },
    Scene{
        .id = "apt_a3",
        .title = "居民楼 · 日记",
        .paragraphs =
        {
            "字迹从工整变得潦草：",
            "“灰雾日前 5 天，穿白防护服的人在楼顶水箱作业，车上印着 ECHO，说是"
            "水质检修。”\n"
            "“前一天我喝了自来水，今天皮肤下有东西在动。水不是被动过手脚——是"
            "早就被写好了。”"
        },
        .choices =
        {
            Choice{ .text = "（收好日记）",          .target = "apt_lobby",
                    .grantedClue = "A3" },
        }
    },
    Scene{
        .id = "apt_roof",
        .title = "居民楼 · 天台水箱",
        .paragraphs =
        {
            "巨大的水箱内壁，在 A3 所述的“作业”位置，残留着一圈未被冲洗干净的"
            "青黑色附着物。"
        },
        .choices =
        {
            Choice{ .text = "刮取残留样本",          .target = "apt_roof_got",
                    .requireAll = { "A3" }, .requireNone = { "tool_tankres" } },
            Choice{ .text = "直接返回",              .target = "apt_lobby" },
        }
    },
    Scene{
        .id = "apt_roof_got",
        .title = "居民楼 · 天台",
        .paragraphs =
        {
            "残留物在袋中微微蠕动，与 B1 瓶底、诊所涂片里的东西一模一样。"
            "你离“是谁、何时、如何”又近了一步。"
        },
        .choices =
        {
            Choice{ .text = "（收好样本）",          .target = "apt_lobby",
                    .grantedClue = "tool_tankres" },
        }
    },
    Scene{
        .id = "apt_501",
        .title = "居民楼 · 501 室",
        .paragraphs =
        {
            "门后是一个腿部受伤的年轻男人，他警惕地举起碎玻璃，看清你没有恶意"
            "后才松了口气。",
            "“我知道很多他们的密码……但我快不行了，伤口感染了。”"
        },
        .choices =
        {
            Choice{ .text = "用急救药品为他处理伤口",.target = "apt_501_save",
                    .requireAll = { "tool_medicine" }, .requireNone = { "tool_codes" } },
            Choice{ .text = "空手强行闯入",          .target = "death_apt_501" },
            Choice{ .text = "退出去",                .target = "apt_lobby" },
        }
    },
    Scene{
        .id = "apt_501_save",
        .title = "居民楼 · 501 室",
        .paragraphs =
        {
            "你给他注射了抗生素。男人缓过来，把一本记满数字密码的小本塞给你：",
            "“供水厂中控密码、医院太平间门禁……都在里面。替我把真相带出去。”"
        },
        .choices =
        {
            Choice{ .text = "（收好密码本）",        .target = "apt_lobby",
                    .grantedClue = "tool_codes" },
        }
    },
    Scene{
        .id = "apt_utility",
        .title = "居民楼 · 设备间",
        .paragraphs =
        {
            "你撬开设备间，墙上挂着一把管钳扳手，供水总阀的结构图画在旁边。"
        },
        .choices =
        {
            Choice{ .text = "取走管钳扳手",          .target = "apt_utility_got",
                    .requireNone = { "tool_wrench" } },
            Choice{ .text = "直接离开",              .target = "apt_lobby" },
        }
    },
    Scene{
        .id = "apt_utility_got",
        .title = "居民楼 · 设备间",
        .paragraphs = { "有了管钳，供水厂的管道阀门就能由你亲手关闭。" },
        .choices =
        {
            Choice{ .text = "（收好扳手）",          .target = "apt_lobby",
                    .grantedClue = "tool_wrench" },
        }
    },
    Scene{
        .id = "death_apt_basement",
        .title = "",
        .paragraphs =
        {
            "应急灯熄灭又亮起，围成一圈的身影齐刷刷回头，每一张脸你都认识——"
            "是这栋楼的邻居们。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_apt_loot",
        .title = "",
        .paragraphs =
        {
            "行李哗啦砸在地上，床上的“尸体”猛地坐起——它一直醒着。腐臭的气息"
            "扑面而来。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_apt_501",
        .title = "",
        .paragraphs =
        {
            "你空手逼近，男人拼死挥动碎玻璃，扭打声引来了楼道里的东西。黑暗里，"
            "谁也没能活下去。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 6】地铁隧道（3 个普通场景 + 1 个死亡场景）
    //==========================================================================
    Scene{
        .id = "metro_entrance",
        .title = "地铁隧道 · 入口",
        .paragraphs =
        {
            "避难所下方的隧道漆黑一片，手电光里，轨道向两个方向延伸，一节废弃"
            "列车停在站台边。",
            "更深处有一间亮着微弱指示灯的信号室。"
        },
        .choices =
        {
            Choice{ .text = "登上废弃列车",          .target = "metro_train" },
            Choice{ .text = "进入信号室",            .target = "metro_signal" },
            Choice{ .text = "沿隧道向黑暗深处走",    .target = "death_metro" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "metro_train",
        .title = "地铁隧道 · 废弃列车",
        .paragraphs =
        {
            "车厢座椅东倒西歪，维修工的工具箱还挂在扶手上，里面是一根生锈的"
            "撬棍。",
            "驾驶室的线路图显示：这条地铁维护线，一直通向 D 区地下设施的货运口。"
        },
        .choices =
        {
            Choice{ .text = "拿走撬棍",              .target = "metro_train",
                    .grantedClue = "tool_crowbar",    .requireNone = { "tool_crowbar" } },
            Choice{ .text = "记下线路图",            .target = "metro_entrance" },
        }
    },
    Scene{
        .id = "metro_signal",
        .title = "地铁隧道 · 信号室",
        .paragraphs =
        {
            "信号台缺了一支电子管。"
        },
        .choices =
        {
            Choice{ .text = "用电子管修好信号台",    .target = "metro_signal_on",
                    .requireAll = { "tool_radiopart" } },
            Choice{ .text = "返回",                  .target = "metro_entrance" },
        }
    },
    Scene{
        .id = "metro_signal_on",
        .title = "地铁隧道 · 信号室",
        .paragraphs =
        {
            "信号灯重新亮起，维护隧道的锁扣咔嗒弹开。你确认：穿过这条维护隧道，"
            "可以不经排水渠、直接抵达 D 区设施的货运口。",
            "（你已掌握实验室的另一条通路。）"
        },
        .choices =
        {
            Choice{ .text = "记下，返回隧道",        .target = "metro_entrance" },
        }
    },
    Scene{
        .id = "death_metro",
        .title = "",
        .paragraphs =
        {
            "你沿着轨道越走越深，手电骤然熄灭。黑暗中列车汽笛毫无征兆地响起——"
            "这个早已停运的世界里，不该还有列车。你被光圈吞没。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 7】排水渠 & D 区地下实验室（核心：10 个普通场景 + 4 个死亡场景）
    //==========================================================================
    Scene{
        .id = "sewer_entrance",
        .title = "排水渠 · 入口",
        .paragraphs =
        {
            "你顺着 A2 通讯的坐标找到排水渠入口，污水里漂着青黑色的絮状物，"
            "空气里有刺鼻的甜腥味。"
        },
        .choices =
        {
            Choice{ .text = "继续深入",              .target = "sewer_fork" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "sewer_fork",
        .title = "排水渠 · 岔路",
        .paragraphs =
        {
            "渠水在此分岔：左侧是干涸的维护通道，右侧是被污水淹没的暗渠，"
            "隐约能看见后方合金门的轮廓。"
        },
        .choices =
        {
            Choice{ .text = "走干涸的维护通道",      .target = "lab_gate" },
            Choice{ .text = "强渡污水暗渠",          .target = "death_sewer" },
            Choice{ .text = "退回",                  .target = "sewer_entrance" },
        }
    },
    Scene{
        .id = "lab_gate",
        .title = "D 区设施 · 大门",
        .paragraphs =
        {
            "厚重的合金门紧闭，门禁读卡器上印着你已熟悉的 ECHO 标志；门旁的"
            "通风管道少了两颗螺丝。"
        },
        .choices =
        {
            Choice{ .text = "拧开通风管钻进去",      .target = "lab_corridor" },
            Choice{ .text = "用铁棍撬开合金门",      .target = "death_lab_gate" },
            Choice{ .text = "原路返回",              .target = "sewer_entrance" },
        }
    },
    Scene{
        .id = "lab_corridor",
        .title = "D 区设施 · 走廊",
        .paragraphs =
        {
            "你落进一条雪白走廊，培养舱整齐排列。指示牌指向五个方向：样本室、"
            "办公区、深冷库、生物收容区、主管室，走廊尽头是主控室。"
        },
        .choices =
        {
            Choice{ .text = "进入样本室",            .target = "lab_sample" },
            Choice{ .text = "进入办公区",            .target = "lab_office" },
            Choice{ .text = "进入深冷库",            .target = "lab_freezer" },
            Choice{ .text = "进入生物收容区",        .target = "lab_containment" },
            Choice{ .text = "进入主管室",            .target = "lab_director" },
            Choice{ .text = "进入走廊尽头的主控室",  .target = "lab_control" },
            Choice{ .text = "撤离设施",              .target = "sewer_entrance" },
        }
    },
    Scene{
        .id = "lab_sample",
        .title = "D 区设施 · 样本室",
        .paragraphs =
        {
            "培养舱内悬浮着青黑色絮状物，最近的舱体标签写着：ECHO-0 初代株，"
            "提取源——时间锚点介质，日期早于灰雾日 15 天。"
        },
        .choices =
        {
            Choice{ .text = "拍下全部样本标签",      .target = "lab_s1",
                    .requireNone = { "S1" } },
            Choice{ .text = "砸碎培养舱取样",        .target = "death_lab_containment" },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_s1",
        .title = "D 区设施 · 样本标签",
        .paragraphs =
        {
            "没有一个字提到“武器”。这东西从诞生起就不是为了被投放——而是为了"
            "被“带回”。你把标签一张张拍进脑海。"
        },
        .choices =
        {
            Choice{ .text = "（收好线索）",          .target = "lab_corridor",
                    .grantedClue = "S1" },
        }
    },
    Scene{
        .id = "lab_office",
        .title = "D 区设施 · 办公区",
        .paragraphs =
        {
            "桌上文件散落一地，一台老式录音机还插着电，磁带停在开头。"
        },
        .choices =
        {
            Choice{ .text = "播放录音",              .target = "lab_s2",
                    .requireNone = { "S2" } },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_s2",
        .title = "D 区设施 · 录音",
        .paragraphs =
        {
            "疲惫的女声在沙沙声中响起：“第 97 次循环记录。回声是时间介质，每一次"
            "回溯，病毒都跟着意识回到更早的时间线。零号宿主，是那个反复在锚点"
            "醒来的人。”",
            "“集齐全部真相后——永远不要再回来。”"
        },
        .choices =
        {
            Choice{ .text = "（收好录音）",          .target = "lab_corridor",
                    .grantedClue = "S2" },
        }
    },
    Scene{
        .id = "lab_freezer",
        .title = "D 区设施 · 深冷库",
        .paragraphs =
        {
            "库门需要抵御极低温度与高浓度污染。"
        },
        .choices =
        {
            Choice{ .text = "穿防护服进入",          .target = "lab_freezer_in",
                    .requireAll = { "tool_protection" } },
            Choice{ .text = "无防护硬闯",            .target = "death_lab_containment" },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_freezer_in",
        .title = "D 区设施 · 深冷库",
        .paragraphs =
        {
            "冷气里冻着一排排标着循环编号的日志。最新一页写着：“第 0 次循环的"
            "样本，来自一个尚未发生的未来。病毒不是被‘发明’的，是被锚点从未来"
            "‘接’回来的。”",
            "你把日志与 S1、S2 默默对应。"
        },
        .choices =
        {
            Choice{ .text = "退出深冷库",            .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_containment",
        .title = "D 区设施 · 生物收容区",
        .paragraphs =
        {
            "厚重的隔离门后传来缓慢的撞击声，门缝渗出青黑色雾气。"
        },
        .choices =
        {
            Choice{ .text = "持枪进入",              .target = "lab_containment_in",
                    .requireAll = { "tool_weapon" } },
            Choice{ .text = "徒手进入",              .target = "death_lab_containment" },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_containment_in",
        .title = "D 区设施 · 收容区",
        .paragraphs =
        {
            "隔离舱中央悬浮着一个人形轮廓，它的脸——和你一模一样。它隔着玻璃"
            "安静地看着你，口型说着同一句话：",
            "“我是上一次循环里，没能走出去的你。”",
            "你握紧枪，最终没有开火，把它的存在记在心里。"
        },
        .choices =
        {
            Choice{ .text = "退出收容区",            .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_director",
        .title = "D 区设施 · 主管室",
        .paragraphs =
        {
            "门禁需要军方磁卡。"
        },
        .choices =
        {
            Choice{ .text = "刷门禁卡进入",          .target = "lab_director_in",
                    .requireAll = { "tool_keycard" } },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_director_in",
        .title = "D 区设施 · 主管室",
        .paragraphs =
        {
            "主管已伏在桌上，手边是一封未寄出的自白：",
            "“我们以为能从未来借回救命的技术，锚点打开的那一刻，接回来的却是"
            "病毒。它沿着回溯一路向上游传播，越想纠正，末日来得越早。能终结它的"
            "人，必须愿意在最后一条时间线里，不再复活。”"
        },
        .choices =
        {
            Choice{ .text = "收起自白，退出",        .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_control",
        .title = "D 区设施 · 主控室",
        .paragraphs =
        {
            "巨大的服务器无声运转，主屏上的 ECHO 计划总览需要三重验证：样本编号、"
            "录音密钥、以及一段尚未补齐的内容。"
        },
        .choices =
        {
            Choice{ .text = "输入样本编号与录音密钥",.target = "lab_s3",
                    .requireAll = { "S1", "S2" }, .requireNone = { "S3" } },
            Choice{ .text = "强行拔出存储阵列",      .target = "death_lab_pull" },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_s3",
        .title = "D 区设施 · 真相文件",
        .paragraphs =
        {
            "最后一段文字缓缓展开：“病毒不通过空气、水或接触传播，它通过回溯传播。"
            "每一次死亡后的回溯，都是一次向过去的播种。”",
            "“终止方式只有一个：带着全部真相，停止回溯。哪怕这条时间线里的你，"
            "终将死去。”",
            "你站在屏幕前，终于懂了这座城市为何沦陷——它的末日，始于你一次次"
            "“醒来”。"
        },
        .choices =
        {
            Choice{ .text = "（收好真相文件）",      .target = "lab_corridor",
                    .grantedClue = "S3" },
        }
    },
    Scene{
        .id = "death_sewer",
        .title = "",
        .paragraphs =
        {
            "污水漫过口鼻，青黑色细丝顺着七窍钻入。你在自己的倒影里，看见无数张"
            "循环中沉没的脸。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_lab_gate",
        .title = "",
        .paragraphs =
        {
            "警报炸开，门缝喷出的白雾先一步把你吞没。你最后看见的，是自己的手臂"
            "以肉眼可见的速度变得透明。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_lab_containment",
        .title = "",
        .paragraphs =
        {
            "青黑色雾气裹住你，没有疼痛，只有一种被无数双眼睛同时注视的感觉——"
            "仿佛过去与未来的每一条时间线，都在这一刻看见了你。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_lab_pull",
        .title = "",
        .paragraphs =
        {
            "红色警报铺满房间，喷淋头喷出白雾，铁门一道道落下。你以为自己在"
            "毁掉真相，却只是毁掉了这条时间线里唯一的出口。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 8】红区（3 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "redzone_street",
        .title = "红区 · 封锁街道",
        .paragraphs =
        {
            "雾气浓得几乎看不见指尖，废弃车辆在雾里排成一条死寂的河。",
            "三条岔路分别通向尖顶的教堂、挂着彩旗的学校、以及一栋黑沉沉的医院；"
            "雾深处有缓慢拖动的声响。"
        },
        .choices =
        {
            Choice{ .text = "进入教堂",              .target = "redzone_church" },
            Choice{ .text = "前往学校",              .target = "redzone_school" },
            Choice{ .text = "前往医院",              .target = "hospital_lobby" },
            Choice{ .text = "循着拖动声走进雾深处",  .target = "death_redzone" },
            Choice{ .text = "退回检查站",            .target = "cp_deepgate" },
        }
    },
    Scene{
        .id = "redzone_church",
        .title = "红区 · 教堂",
        .paragraphs =
        {
            "教堂里聚着十几个幸存者，烛光摇曳。一位老神父把你领到告解室：",
            "“医院里躺着第 0 号病人，学校楼顶有还能用的救援电台。孩子，记住——"
            "在这里，真相比子弹更能救命。”"
        },
        .choices =
        {
            Choice{ .text = "记下情报，离开教堂",    .target = "redzone_street" },
        }
    },
    Scene{
        .id = "redzone_school",
        .title = "红区 · 学校",
        .paragraphs =
        {
            "教学楼里空无一人，楼顶广播室的设备半新，只是缺了一支关键电子管。",
            "操场方向传来成群游荡的脚步声。"
        },
        .choices =
        {
            Choice{ .text = "装上电子管，呼叫救援",  .target = "redzone_school_call",
                    .requireAll = { "tool_radiopart" } },
            Choice{ .text = "徒手爬上旗杆大声呼救",  .target = "death_redzone_school" },
            Choice{ .text = "离开学校",              .target = "redzone_street" },
        }
    },
    Scene{
        .id = "redzone_school_call",
        .title = "红区 · 学校",
        .paragraphs =
        {
            "电流声后，一个声音回应：“收到，救援直升机将在天亮抵达，请坚持。”",
            "你松了口气，却在设备玻璃的倒影里看见：自己的身后，空无一人的楼道"
            "站满了“人”。你忽然明白，这一幕你已经经历过很多次。"
        },
        .choices =
        {
            Choice{ .text = "退出广播室",            .target = "redzone_street" },
        }
    },
    Scene{
        .id = "death_redzone",
        .title = "",
        .paragraphs =
        {
            "拖动声越来越近，你走进雾最浓的地方。一张脸从雾里贴到你面前——它"
            "穿着和你一样的衣服，带着和你一样的伤痕，只是没有眼珠。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_redzone_school",
        .title = "",
        .paragraphs =
        {
            "你的呼喊在操场上空回荡，回应它的是齐刷刷抬起的头。无数身影从教学楼"
            "涌出，旗杆下的空地，成了这条时间线的终点。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 9】医院（4 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "hospital_lobby",
        .title = "医院 · 大厅",
        .paragraphs =
        {
            "大厅的挂号屏还亮着，滚动着永远叫不完的号。",
            "电梯停在底层，楼梯通向病房楼层；ICU 在走廊东侧；地下一层是停尸房，"
            "门禁锁着。"
        },
        .choices =
        {
            Choice{ .text = "前往病房楼层",          .target = "hospital_ward" },
            Choice{ .text = "穿防护服进入 ICU",      .target = "hospital_icu",
                    .requireAll = { "tool_protection" } },
            Choice{ .text = "用密码打开停尸房门禁",  .target = "hospital_morgue",
                    .requireAll = { "tool_codes" } },
            Choice{ .text = "徒手撬开停尸房门",      .target = "death_hospital" },
            Choice{ .text = "离开医院",              .target = "redzone_street" },
        }
    },
    Scene{
        .id = "hospital_ward",
        .title = "医院 · 病房",
        .paragraphs =
        {
            "病房床头卡上的入院时间，全部是灰雾日凌晨同一时刻，与 A1 病历完全"
            "吻合——“同时按下开关”。",
            "值班室白板上写着：“全部患者均有同一特征：体内检出‘回溯性标记’。”"
        },
        .choices =
        {
            Choice{ .text = "返回大厅",              .target = "hospital_lobby" },
        }
    },
    Scene{
        .id = "hospital_icu",
        .title = "医院 · ICU",
        .paragraphs =
        {
            "ICU 里，一位插着管的研究员尚有微弱意识，他看见防护服上的灰尘，"
            "艰难地开口：",
            "“停尸房……第 0 号……不是病人，是‘锚点’本身……别再让他醒来……”"
            "心电监护仪随即拉成一条直线。"
        },
        .choices =
        {
            Choice{ .text = "退出 ICU",              .target = "hospital_lobby" },
        }
    },
    Scene{
        .id = "hospital_morgue",
        .title = "医院 · 停尸房",
        .paragraphs =
        {
            "冷柜最深处，编号 000 的抽屉拉开：里面躺着一具盖着白布的躯体，"
            "胸前标签写着——“来源：时间锚点；身份：未知（与报告人面容一致）”。",
            "你颤抖着揭开白布，看见的是自己沉睡的脸。"
        },
        .choices =
        {
            Choice{ .text = "合上抽屉，退出停尸房",  .target = "hospital_lobby" },
        }
    },
    Scene{
        .id = "death_hospital",
        .title = "",
        .paragraphs =
        {
            "你用力撬门，停尸房的冷柜在身后一个个自行滑开。你回头，每一个抽屉里"
            "都躺着同一个人，他们同时睁开了眼。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_hospital_icu",
        .title = "",
        .paragraphs =
        {
            "没有防护服，ICU 高浓度的空气在你踏入的瞬间侵入肺腑。你看见自己的手"
            "迅速布满青黑纹路，监护仪上，你的心跳和那些患者并成了同一条线。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 10】信号塔（3 个普通场景 + 1 个死亡场景）
    //==========================================================================
    Scene{
        .id = "tower_base",
        .title = "信号塔 · 塔底",
        .paragraphs =
        {
            "锈红色的信号塔直插灰雾，塔底机房的门虚掩，旋转楼梯向上盘旋，"
            "望不到头。"
        },
        .choices =
        {
            Choice{ .text = "沿旋转楼梯向上攀爬",    .target = "tower_climb" },
            Choice{ .text = "返回",                  .target = "hub" },
        }
    },
    Scene{
        .id = "tower_climb",
        .title = "信号塔 · 楼梯",
        .paragraphs =
        {
            "风从钢梁间呼啸穿过，楼梯在脚下轻轻摇晃。你已攀到云端，下方城市"
            "缩成一片灰色的棋盘。"
        },
        .choices =
        {
            Choice{ .text = "稳住，继续登顶",        .target = "tower_top" },
            Choice{ .text = "俯身向下张望",          .target = "death_tower" },
            Choice{ .text = "退回塔底",              .target = "tower_base" },
        }
    },
    Scene{
        .id = "tower_top",
        .title = "信号塔 · 广播室",
        .paragraphs =
        {
            "塔顶广播室的发射机仍可使用，只是缺了电子管。在这里，声音可以覆盖"
            "整座城市。"
        },
        .choices =
        {
            Choice{ .text = "装上电子管，向全城广播真相", .target = "tower_broadcast",
                    .requireAll = { "tool_radiopart" } },
            Choice{ .text = "强行短接发射机",      .target = "death_tower" },
            Choice{ .text = "下楼",                .target = "tower_climb" },
        }
    },
    Scene{
        .id = "tower_broadcast",
        .title = "信号塔 · 广播",
        .paragraphs =
        {
            "你对着话筒，把“回声”、投毒、零号宿主与循环的一切，一字一句广播出去。",
            "你想象无数人在收音机前抬头——但你也清楚，若你此刻死去、再次回溯，"
            "这场广播将从未发生。要让真相留下，唯有让自己不再醒来。",
            "话音落下，你忽然看见，远方的天边，泛起了一道金色的光。",
            "是……黎明吗？"
        },
        .choices =
        {
            // 虚假黎明陷阱：在“金光”前停下，便会被锚点奖赏一层梦
            Choice{ .text = "如释重负，迎接眼前的黎明", .target = "",
                    .endingId = "ending_false_dawn" },
            Choice{ .text = "保持清醒，那光来得太巧，下楼", .target = "tower_base" },
        }
    },
    Scene{
        .id = "death_tower",
        .title = "",
        .paragraphs =
        {
            "脚下一滑 / 电弧一闪，你从钢梁间坠落。风声里，城市的灯光在你眼前迅速"
            "放大，最后归于一片漆黑。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 11】城市供水厂（3 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "wp_gate",
        .title = "供水厂 · 大门",
        .paragraphs =
        {
            "供水厂安静得反常，蓄水池像一面面灰色的镜子。中控楼的大门锁着数字密码。"
        },
        .choices =
        {
            Choice{ .text = "用密码打开中控楼",      .target = "wp_tanks",
                    .requireAll = { "tool_codes" } },
            Choice{ .text = "翻越围墙进入",          .target = "death_wp" },
            Choice{ .text = "返回",                  .target = "hub" },
        }
    },
    Scene{
        .id = "wp_tanks",
        .title = "供水厂 · 水箱群",
        .paragraphs =
        {
            "巨大的水箱顶部，焊着一台不属于供水厂的“自动投放装置”，出料口残留着"
            "青黑色附着物。"
        },
        .choices =
        {
            Choice{ .text = "用天台残留样本比对",    .target = "wp_tanks_match",
                    .requireAll = { "tool_tankres" } },
            Choice{ .text = "用管钳关闭投放总阀",    .target = "wp_tanks_valve",
                    .requireAll = { "tool_wrench" } },
            Choice{ .text = "下到敞开的清水池查看",  .target = "death_wp_drown" },
            Choice{ .text = "进入中控室",            .target = "wp_control" },
        }
    },
    Scene{
        .id = "wp_tanks_match",
        .title = "供水厂 · 水箱群",
        .paragraphs =
        {
            "比对结果完全一致：投放装置的残留物，与居民楼水箱、超市瓶底、诊所涂片"
            "里的东西同源。",
            "你终于从“物证”上锁定了污染的起点。"
        },
        .choices =
        {
            Choice{ .text = "返回",                  .target = "wp_tanks" },
        }
    },
    Scene{
        .id = "wp_tanks_valve",
        .title = "供水厂 · 水箱群",
        .paragraphs =
        {
            "你用管钳死死拧住总阀，管道发出沉闷的轰鸣，投放装置的指示灯一盏盏"
            "熄灭。",
            "这一条时间线里，被污染的水终于停止了输送——虽然你知道，回溯之后，"
            "它仍会重来。"
        },
        .choices =
        {
            Choice{ .text = "返回",                  .target = "wp_tanks" },
        }
    },
    Scene{
        .id = "wp_control",
        .title = "供水厂 · 中控室",
        .paragraphs =
        {
            "电脑里的投放记录从灰雾日前 15 天开始，执行方一栏始终是 ECHO，"
            "触发条件写着一行冰冷的字：",
            "“每当锚点发生一次回溯，自动向对应时间线补投一次。”",
            "记录与 S3 互为印证——你越挣扎，它越严密。"
        },
        .choices =
        {
            Choice{ .text = "离开供水厂",            .target = "wp_gate" },
        }
    },
    Scene{
        .id = "death_wp",
        .title = "",
        .paragraphs =
        {
            "你翻上围墙，电网的蓝光在雾里一闪。你甚至没来得及感到疼痛，世界就"
            "连同电流一起，归于寂静。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_wp_drown",
        .title = "",
        .paragraphs =
        {
            "清水池的水面下，青黑色细丝像水草一样缠上你的脚踝，把你缓缓拖向池底。"
            "你最后看见的，是水面上一圈圈扩散的、属于你自己的倒影。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 12】锚点核心（通往真结局）
    //==========================================================================
    Scene{
        .id = "core_gate",
        .title = "锚点核心 · 门前",
        .paragraphs =
        {
            "你循着九条线索的指引，来到避难所最深处一扇从未见过的门前。门后没有"
            "雾气，只有温暖而安静的光，像一个等待被结束的梦。",
            "门心刻着一行字：“入此门者，当舍一切重来。”"
        },
        .choices =
        {
            Choice{ .text = "推门，走入光中",        .target = "core_anchor" },
            Choice{ .text = "还没有准备好，退回去",  .target = "hub" },
        }
    },
    Scene{
        .id = "core_anchor",
        .title = "锚点核心",
        .paragraphs =
        {
            "光的中央，悬浮着这座城市所有时间线的“锚点”，它随着你的心跳轻轻搏动。",
            "你想起收容区里那个和你一模一样的人，想起停尸房里编号 000 的躯体，"
            "想起录音里那句“集齐之后，永远不要再回来”。",
            "你可以选择关闭它——这一次，你将不再醒来。"
        },
        .choices =
        {
            // 原真结局（9 条线索）
            Choice{ .text = "亲手关闭锚点，终结一切循环", .target = "",
                    .endingId = "ending_true" },
            // 扩展：炸毁锚点（同归于尽）
            Choice{ .text = "引爆炸药，与锚点同归于尽",   .target = "",
                    .requireAll = { "tool_bomb" },          .endingId = "ending_bomb" },
            // 扩展：自己成为新锚点
            Choice{ .text = "走进光中，成为新的锚点",     .target = "",
                    .requireAll = { "S4" },                 .endingId = "ending_newanchor" },
            // 扩展：用抗体让病毒沉睡，与世界共存
            Choice{ .text = "注射抗体，让病毒随锚点沉睡", .target = "",
                    .requireAll = { "S5", "tool_antidote" },.endingId = "ending_symbiosis" },
            // 扩展：放下，拒绝介入（旁观者）
            Choice{ .text = "放下一切，转身离开，不再介入", .target = "",
                    .requireAll = { "S4" },                 .endingId = "ending_watcher" },
            // 完美真结局：15 条全齐
            Choice{ .text = "【15 条真相全齐】以完整之心，轻轻合上时间", .target = "",
                    .requireAll = { "B1","B2","B3","B4","B5",
                                    "A1","A2","A3","A4","A5",
                                    "S1","S2","S3","S4","S5" },
                    .endingId = "ending_true_complete" },
            Choice{ .text = "再给自己一次“重来”",    .target = "hub" },
        }
    },

    //==========================================================================
    // 【扩展区域 13】旧广播大楼（广播女声真身 · 林晚）
    //==========================================================================
    Scene{
        .id = "radio_outside",
        .title = "旧广播大楼 · 门外",
        .paragraphs =
        {
            "灰白色的广播大楼矗立在雾中，楼顶天线像一片枯树林。",
            "大门虚掩，门缝里透出微弱的红光；侧门挂着一把生锈的锁。"
        },
        .choices =
        {
            Choice{ .text = "推开虚掩的大门",          .target = "radio_lobby" },
            Choice{ .text = "用撬棍撬开侧门",          .target = "radio_side",
                    .requireAll = { "tool_crowbar" } },
            Choice{ .text = "返回避难所",              .target = "hub" },
        }
    },
    Scene{
        .id = "radio_lobby",
        .title = "广播大楼 · 大厅",
        .paragraphs =
        {
            "大厅里散落着翻倒的桌椅，前台的广播还在播放，只是声音断断续续。",
            "一个女人的声音从二楼直播间传来，她在循环播放同一段录音，然后停顿，"
            "再播放——仿佛在等什么人。"
        },
        .choices =
        {
            Choice{ .text = "上二楼直播间",            .target = "radio_studio" },
            Choice{ .text = "走进一楼设备间",          .target = "radio_equip" },
            Choice{ .text = "退出大楼",                .target = "radio_outside" },
        }
    },
    Scene{
        .id = "radio_equip",
        .title = "广播大楼 · 设备间",
        .paragraphs =
        {
            "设备间里，一台老式发射机还亮着红灯，只是缺了总控钥匙。",
            "桌上有一张工作证，照片上的女人眉眼温柔，名字写着：林晚，首席主播。"
        },
        .choices =
        {
            Choice{ .text = "收起工作证",              .target = "radio_lobby" },
        }
    },
    Scene{
        .id = "radio_side",
        .title = "广播大楼 · 侧门",
        .paragraphs =
        {
            "侧门通向消防通道，墙上贴着灰雾日当天的节目单：",
            "“00:00 特别节目《回声》——主播：林晚。”",
            "节目单边角，用红笔写着一行小字：“如果我开始循环，请关掉我。”"
        },
        .choices =
        {
            Choice{ .text = "进入大楼",                .target = "radio_lobby" },
        }
    },
    Scene{
        .id = "radio_studio",
        .title = "广播大楼 · 直播间",
        .paragraphs =
        {
            "直播间里，林晚坐在话筒前，她没有回头，只是轻声说：",
            "“你终于来了。我已经在这里，循环了……我数不清多少次。”",
            "“我是第一个自愿走进锚点的人。我以为能救回我的女儿，却把整座城市拖进了"
            "循环。广播里的女声，是我留给你的路标。”",
            "她转过身，把一把总控钥匙递给你：“顶层发射机，可以让全城的人，在最后一刻"
            "听见真相。怎么用，由你决定。”"
        },
        .choices =
        {
            Choice{ .text = "（接过广播总控钥匙）",    .target = "radio_lobby",
                    .grantedClue = "tool_radio_key" },
            Choice{ .text = "关掉她的录音设备",        .target = "radio_studio_off" },
        }
    },
    Scene{
        .id = "radio_studio_off",
        .title = "广播大楼 · 直播间",
        .paragraphs =
        {
            "你按下停止键，林晚的身影在寂静中变得透明，她如释重负地笑了：",
            "“谢谢你，让我终于……不用再等了。”",
            "直播间的红灯熄灭，整座城市的广播，第一次彻底安静。"
        },
        .choices =
        {
            Choice{ .text = "退出直播间",              .target = "radio_lobby" },
        }
    },
    Scene{
        .id = "radio_roof",
        .title = "广播大楼 · 顶层发射机",
        .paragraphs =
        {
            "顶层发射机需要总控钥匙。"
        },
        .choices =
        {
            Choice{ .text = "插入钥匙，开启全国频段",  .target = "radio_roof_on",
                    .requireAll = { "tool_radio_key" } },
            Choice{ .text = "退回",                    .target = "radio_lobby" },
        }
    },
    Scene{
        .id = "radio_roof_on",
        .title = "广播大楼 · 顶层",
        .paragraphs =
        {
            "你把全部真相通过全国频段播送出去。这一次，不止是这座城市，整个世界都在听。",
            "你忽然明白林晚的苦心：她用自己的循环，为你铺了一条比信号塔更远的路。"
        },
        .choices =
        {
            Choice{ .text = "下楼",                    .target = "radio_lobby" },
        }
    },
    Scene{
        .id = "death_radio",
        .title = "",
        .paragraphs =
        {
            "你在黑暗中摸索设备，裸露的高压线在雾里闪了一下。你最后听见的，是自己的声音"
            "从无数个喇叭里，循环着同一句没说完的话。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【扩展区域 14】港口码头（老船长 · 远航结局）
    //==========================================================================
    Scene{
        .id = "dock_gate",
        .title = "港口码头 · 入口",
        .paragraphs =
        {
            "咸腥的海风穿过雾，码头的吊臂像巨兽的骨架。",
            "几艘船静静泊在港内，其中一艘“远星号”货轮亮着微弱的灯。"
        },
        .choices =
        {
            Choice{ .text = "登上远星号",              .target = "dock_ship" },
            Choice{ .text = "搜索码头仓库",            .target = "dock_warehouse" },
            Choice{ .text = "前往灯塔",                .target = "dock_lighthouse" },
            // 远航结局：拿到船钥匙与舱单后，可选择离港
            Choice{ .text = "【远航】登船，驶向雾外的世界", .target = "",
                    .requireAll = { "tool_boat_key", "B5" }, .endingId = "ending_voyage" },
            Choice{ .text = "返回避难所",              .target = "hub" },
        }
    },
    Scene{
        .id = "dock_ship",
        .title = "远星号 · 甲板",
        .paragraphs =
        {
            "甲板上，一位老船长靠着栏杆，他的眼睛在雾里依然明亮：",
            "“年轻人，你是这几天第一个走到这里的活人。”",
            "“这艘船加满了油，能驶到任何一座没有被雾笼罩的城市。我老了，走不动了，"
            "但你可以。”"
        },
        .choices =
        {
            Choice{ .text = "请求查看舱单",            .target = "dock_manifest" },
            Choice{ .text = "接受船钥匙，准备远航",    .target = "dock_sail",
                    .requireNone = { "tool_boat_key" } },
            Choice{ .text = "下船",                    .target = "dock_gate" },
        }
    },
    Scene{
        .id = "dock_manifest",
        .title = "远星号 · 船长室",
        .paragraphs =
        {
            "老船长把舱单递给你：“灰雾日前，有人半夜往我船上装了一批密封容器，"
            "目的地写着‘循环港’。我觉得不对劲，就没开船。”",
            "你翻到那一页，舱单上的签名是 ECHO，时间是午夜。"
        },
        .choices =
        {
            Choice{ .text = "（收好异常舱单 B5）",     .target = "dock_ship",
                    .grantedClue = "B5" },
        }
    },
    Scene{
        .id = "dock_sail",
        .title = "远星号",
        .paragraphs =
        {
            "老船长把船钥匙放进你手心：“走吧，趁雾还没追上来。”",
            "你发动引擎，货轮缓缓离港。"
        },
        .choices =
        {
            Choice{ .text = "（收下船钥匙）",          .target = "dock_ship",
                    .grantedClue = "tool_boat_key" },
        }
    },
    Scene{
        .id = "dock_warehouse",
        .title = "码头 · 仓库",
        .paragraphs =
        {
            "仓库里堆满了集装箱，其中一个没有锁，里面是成箱的、印着 ECHO 标志的密封容器，"
            "和舱单上的描述完全一致。",
            "容器表面凝着青黑色的霜。"
        },
        .choices =
        {
            Choice{ .text = "撬开一个容器",            .target = "death_dock" },
            Choice{ .text = "退出去",                  .target = "dock_gate" },
        }
    },
    Scene{
        .id = "dock_lighthouse",
        .title = "码头 · 灯塔",
        .paragraphs =
        {
            "灯塔的灯还能转，只是缺了电子管。在这里点亮，能为远海的船指引方向。"
        },
        .choices =
        {
            Choice{ .text = "装上电子管，点亮灯塔",    .target = "dock_lighthouse_on",
                    .requireAll = { "tool_radiopart" } },
            Choice{ .text = "徒手攀爬灯塔外壁",        .target = "death_dock" },
            Choice{ .text = "下去",                    .target = "dock_gate" },
        }
    },
    Scene{
        .id = "dock_lighthouse_on",
        .title = "码头 · 灯塔",
        .paragraphs =
        {
            "光柱刺破海雾，一圈圈扫过海面。你看见远方有零星的灯火回应——",
            "这个世界，还有人活着。"
        },
        .choices =
        {
            Choice{ .text = "下去",                    .target = "dock_gate" },
        }
    },
    Scene{
        .id = "death_dock",
        .title = "",
        .paragraphs =
        {
            "容器裂开的瞬间，青黑色的雾气扑面而来 / 你从灯塔外壁滑落。咸腥的海风里，"
            "你听见锚点的心跳，和海浪，合成了同一个节拍。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【扩展区域 15】第一小学（幸存女孩小雨 · 壁画线索）
    //==========================================================================
    Scene{
        .id = "school_gate",
        .title = "第一小学 · 校门",
        .paragraphs =
        {
            "小学校门的铁栅栏上挂着褪色的彩旗，操场空空荡荡，秋千在风里轻轻晃。",
            "教学楼的墙上，从远处就能看见大片彩色的壁画。"
        },
        .choices =
        {
            Choice{ .text = "走进教学楼",              .target = "school_hall" },
            Choice{ .text = "先去看墙上的壁画",        .target = "school_mural" },
            // 传承结局：把真相与抗体交给小雨
            Choice{ .text = "【传承】把真相和抗体托付给小雨", .target = "",
                    .requireAll = { "B4", "tool_antidote" }, .endingId = "ending_legacy" },
            Choice{ .text = "返回避难所",              .target = "hub" },
        }
    },
    Scene{
        .id = "school_hall",
        .title = "第一小学 · 走廊",
        .paragraphs =
        {
            "走廊两侧的教室门都开着，课桌上还摆着没来得及收的课本。",
            "最里面的教室传来轻轻的、压抑的哭声。"
        },
        .choices =
        {
            Choice{ .text = "循声走进教室",            .target = "school_classroom" },
            Choice{ .text = "走进教师办公室",          .target = "school_office" },
            Choice{ .text = "退出教学楼",              .target = "school_gate" },
        }
    },
    Scene{
        .id = "school_classroom",
        .title = "第一小学 · 教室",
        .paragraphs =
        {
            "一个小女孩缩在课桌下，她叫小雨，怀里抱着一盒蜡笔。",
            "看见你没有恶意，她小声说：",
            "“老师说，只要把‘醒来的人’画在墙上，他们就不会再迷路了。”",
            "“我画了好多好多……每一个醒来的人，胸口都连着一根线，线的那头，是一个"
            "发光的圆圈。”"
        },
        .choices =
        {
            Choice{ .text = "让她带你去看壁画",        .target = "school_mural" },
            Choice{ .text = "把食物留给她",            .target = "school_classroom_food" },
        }
    },
    Scene{
        .id = "school_classroom_food",
        .title = "第一小学 · 教室",
        .paragraphs =
        {
            "你把仅有的食物分给小雨，她第一次露出笑容，把一张画着太阳的小画塞给你：",
            "“这个给你，这样你走到哪里，都不会黑了。”"
        },
        .choices =
        {
            Choice{ .text = "退出教室",                .target = "school_hall" },
        }
    },
    Scene{
        .id = "school_mural",
        .title = "第一小学 · 壁画",
        .paragraphs =
        {
            "整面墙画满了小人，每一个小人胸口都连着一根线，无数根线汇聚到墙中央一个"
            "发光的圆圈——那是锚点。",
            "最角落里，有一个没有连线的小人，正转身离开。小雨指着它说：",
            "“老师说，这个小人最勇敢，因为他不用再醒来了。”",
            "你久久凝视着这幅画——孩子的眼睛，早已看穿了循环的全部结构。"
        },
        .choices =
        {
            Choice{ .text = "（拍下壁画 B4）",         .target = "school_gate",
                    .grantedClue = "B4" },
        }
    },
    Scene{
        .id = "school_office",
        .title = "第一小学 · 教师办公室",
        .paragraphs =
        {
            "办公室的日历停在灰雾日，班主任的备课本最后一页写着：",
            "“今天要带孩子们画《我心中的明天》。如果明天没有来，希望这些画，能替我们记住"
            "明天该有的样子。”"
        },
        .choices =
        {
            Choice{ .text = "退出办公室",              .target = "school_hall" },
        }
    },
    Scene{
        .id = "death_school",
        .title = "",
        .paragraphs =
        {
            "你在空荡的走廊里大声呼喊，回应你的，是从每一间教室里同时抬起的、小小的头。"
            "他们胸口的线，在黑暗中，齐刷刷地亮了。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【扩展区域 16】高架废墟（退伍老兵老赵 · 旧军事地图）
    //==========================================================================
    Scene{
        .id = "overpass_base",
        .title = "高架废墟 · 桥下",
        .paragraphs =
        {
            "断裂的立交桥横亘在雾里，扭曲的钢筋像凝固的闪电。",
            "桥墩上有人用白漆画了一个向上的箭头，旁边写着：“活着的，往上走。”"
        },
        .choices =
        {
            Choice{ .text = "沿钢筋向上攀爬",          .target = "overpass_climb" },
            Choice{ .text = "搜索桥下的废弃车辆",      .target = "overpass_cars" },
            Choice{ .text = "返回避难所",              .target = "hub" },
        }
    },
    Scene{
        .id = "overpass_climb",
        .title = "高架废墟 · 攀爬",
        .paragraphs =
        {
            "你踩着扭曲的钢筋向上，风在耳边呼啸，脚下的城市在雾里若隐若现。",
            "前方有一段断裂的桥面，需要跳过去。"
        },
        .choices =
        {
            Choice{ .text = "助跑，跳过断裂处",        .target = "overpass_camp" },
            Choice{ .text = "低头看脚下的深渊",        .target = "death_overpass" },
            Choice{ .text = "退回桥下",                .target = "overpass_base" },
        }
    },
    Scene{
        .id = "overpass_camp",
        .title = "高架废墟 · 老兵据点",
        .paragraphs =
        {
            "桥面最高处，搭着一个简易帐篷，一位退伍老兵老赵正在擦拭一支信号枪。",
            "“能爬到这儿，说明你命硬。”他打量着你，“我在这守了十几天，就为了等一个"
            "能把事情办完的人。”",
            "“我这儿有一张当年的旧军事地图，全城的地下设施、管廊、防空洞，都在上面。”"
        },
        .choices =
        {
            Choice{ .text = "请求看旧军事地图",        .target = "overpass_map" },
            Choice{ .text = "讨一支信号弹",            .target = "overpass_flare" },
            Choice{ .text = "询问他看到了什么",        .target = "overpass_story" },
            Choice{ .text = "向老赵道别，沿钢筋下桥",  .target = "overpass_base" },
        }
    },
    Scene{
        .id = "overpass_map",
        .title = "高架废墟 · 据点",
        .paragraphs =
        {
            "老赵把一张泛黄的军用地图摊开，上面密密麻麻标注着地下设施：",
            "“综合管廊连着全城，旧防空洞在城北，那地方……灰雾日之前，就常有穿白大褂的人"
            "半夜进出。”",
            "他把地图折好，郑重地递给你。"
        },
        .choices =
        {
            Choice{ .text = "（收好旧军事地图）",      .target = "overpass_camp",
                    .grantedClue = "tool_old_map" },
        }
    },
    Scene{
        .id = "overpass_flare",
        .title = "高架废墟 · 据点",
        .paragraphs =
        {
            "老赵把一支信号弹递给你：“在最高处打出去，方圆百里都能看见。能不能等来救援，"
            "就看天意了。”"
        },
        .choices =
        {
            Choice{ .text = "（收好信号弹）",          .target = "overpass_camp",
                    .grantedClue = "tool_flare" },
        }
    },
    Scene{
        .id = "overpass_story",
        .title = "高架废墟 · 据点",
        .paragraphs =
        {
            "老赵望着雾中的城市：“灰雾日凌晨，我亲眼看见城北防空洞的方向，亮起一道光，"
            "然后天就灰了。”",
            "“我打了一辈子仗，从没见过那种武器——它不是杀人，是让人……一遍遍地重来。”"
        },
        .choices =
        {
            Choice{ .text = "沉默地点头",              .target = "overpass_camp" },
        }
    },
    Scene{
        .id = "overpass_cars",
        .title = "高架废墟 · 桥下",
        .paragraphs =
        {
            "废弃车辆里，有一辆军车的车门锁着，里面隐约能看到一个金属箱。",
            "没有撬棍，车窗纹丝不动。"
        },
        .choices =
        {
            Choice{ .text = "用撬棍撬开军车",          .target = "overpass_cars_open",
                    .requireAll = { "tool_crowbar" } },
            Choice{ .text = "退回去",                  .target = "overpass_base" },
        }
    },
    Scene{
        .id = "overpass_cars_open",
        .title = "高架废墟 · 军车",
        .paragraphs =
        {
            "金属箱里是一份旧电报，日期是 30 年前，上面写着：",
            "“灯塔工程首次锚定成功，实验体编号 000，状态：稳定（拒绝结束）。”",
            "电报的落款处，沾着一枚早已干涸的指纹。"
        },
        .choices =
        {
            Choice{ .text = "收起电报",                .target = "overpass_base" },
        }
    },
    Scene{
        .id = "death_overpass",
        .title = "",
        .paragraphs =
        {
            "你低头望向深渊，雾里有一双手，轻轻托住了你的脚踝，像是等了很久，终于等到你"
            "探出头来。你坠进了那片温柔的、灰色的雾。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【扩展区域 17】植物园温室（天然抗体 · 共生结局）
    //==========================================================================
    Scene{
        .id = "garden_gate",
        .title = "植物园 · 大门",
        .paragraphs =
        {
            "植物园的铁门上爬满了藤蔓，雾里却有一片反常的、浓郁的绿。",
            "温室的玻璃顶在灰雾中透着微光，像一座漂浮在死海上的绿色岛屿。"
        },
        .choices =
        {
            Choice{ .text = "推开藤蔓，进入温室",      .target = "garden_greenhouse" },
            Choice{ .text = "搜索露天苗圃",            .target = "garden_outdoor" },
            Choice{ .text = "返回避难所",              .target = "hub" },
        }
    },
    Scene{
        .id = "garden_greenhouse",
        .title = "植物园 · 温室",
        .paragraphs =
        {
            "温室里，大部分植物已经枯萎，唯独中央一株半人高的植物，叶片翠绿，",
            "它周围的青黑丝状物，全都安静地“沉睡”在土壤里，没有蔓延。",
            "叶片上凝着一滴晶莹的汁液。"
        },
        .choices =
        {
            Choice{ .text = "小心采集叶片汁液",        .target = "garden_sample" },
            Choice{ .text = "直接舔食那滴汁液",        .target = "death_garden" },
            Choice{ .text = "走进温室深处",            .target = "garden_deep" },
            Choice{ .text = "退出温室",                .target = "garden_gate" },
        }
    },
    Scene{
        .id = "garden_sample",
        .title = "植物园 · 温室",
        .paragraphs =
        {
            "你用随身的空瓶采集了汁液，滴在一片青黑丝状物上——",
            "细丝没有死亡，而是缓缓蜷缩、沉睡，像被一首无声的歌安抚。",
            "你意识到：这株植物，给出了末日的另一种答案——不是毁灭，不是牺牲，而是共存。"
        },
        .choices =
        {
            Choice{ .text = "（收好天然抗体样本 S5）", .target = "garden_greenhouse",
                    .grantedClue = "S5" },
        }
    },
    Scene{
        .id = "garden_deep",
        .title = "植物园 · 温室深处",
        .paragraphs =
        {
            "温室深处，有一张简陋的工作台，上面摆着提纯过的试剂和一本笔记。",
            "笔记的主人是一位植物学家，灰雾日之后，她一直在研究这株植物，直到最后一页：",
            "“抗体已提纯，它能让病毒沉睡。我没能等到把它送出去的那天。后来者，拜托了。”"
        },
        .choices =
        {
            Choice{ .text = "取走提纯好的抗体试剂",    .target = "garden_deep_take",
                    .requireAll = { "S5" } },
            Choice{ .text = "退回去",                  .target = "garden_greenhouse" },
        }
    },
    Scene{
        .id = "garden_deep_take",
        .title = "植物园 · 温室深处",
        .paragraphs =
        {
            "你把提纯好的抗体试剂小心收好，带着那位植物学家未竟的希望。",
            "瓶身上贴着一张小标签：“愿生命，自己找到出路。”"
        },
        .choices =
        {
            Choice{ .text = "（收好抗体试剂）",        .target = "garden_deep",
                    .grantedClue = "tool_antidote" },
        }
    },
    Scene{
        .id = "garden_outdoor",
        .title = "植物园 · 露天苗圃",
        .paragraphs =
        {
            "露天苗圃里，变异的藤蔓在雾里轻轻摆动，它们对脚步声有反应。",
            "地上有一把锈住的修枝剪，和一串被踩进泥里的脚印。"
        },
        .choices =
        {
            Choice{ .text = "用手枪开路，穿过藤蔓",    .target = "garden_outdoor_gun",
                    .requireAll = { "tool_weapon" } },
            Choice{ .text = "徒手拨开藤蔓",            .target = "death_garden" },
            Choice{ .text = "退回去",                  .target = "garden_gate" },
        }
    },
    Scene{
        .id = "garden_outdoor_gun",
        .title = "植物园 · 苗圃",
        .paragraphs =
        {
            "你鸣枪示警，藤蔓在枪声里退缩，露出一条通往温室后门的小径。",
            "你没有伤害它们，只是借了一条路。"
        },
        .choices =
        {
            Choice{ .text = "进入温室",                .target = "garden_greenhouse" },
        }
    },
    Scene{
        .id = "death_garden",
        .title = "",
        .paragraphs =
        {
            "汁液入喉的瞬间，你感到无数根须在体内生长，你被温柔地、不可逆地，",
            "种进了这片绿色的雾里。多年以后，你的身上，也长出了翠绿的叶。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【扩展区域 18】城市图书馆（陈教授 · 时间理论）
    //==========================================================================
    Scene{
        .id = "library_gate",
        .title = "城市图书馆 · 门前",
        .paragraphs =
        {
            "图书馆的穹顶在雾里若隐若现，石阶上落满了灰，",
            "却有一行刚扫过的、干净的脚印，一直通向大门。"
        },
        .choices =
        {
            Choice{ .text = "沿脚印走进图书馆",        .target = "library_hall" },
            Choice{ .text = "搜索门口的还书箱",        .target = "library_bookdrop" },
            Choice{ .text = "返回避难所",              .target = "hub" },
        }
    },
    Scene{
        .id = "library_hall",
        .title = "图书馆 · 大厅",
        .paragraphs =
        {
            "大厅里，成排的书架像沉默的森林，一盏煤油灯亮在阅读区。",
            "一位白发老人正伏案疾书，他抬头看见你，平静地笑了：",
            "“我等你很久了。请坐，我叫陈渊，研究了一辈子时间。”"
        },
        .choices =
        {
            Choice{ .text = "坐下，听他讲时间理论",    .target = "library_professor" },
            Choice{ .text = "先去查馆藏档案",          .target = "library_archive" },
            Choice{ .text = "退出图书馆",              .target = "library_gate" },
        }
    },
    Scene{
        .id = "library_professor",
        .title = "图书馆 · 阅读区",
        .paragraphs =
        {
            "陈教授推来一叠手稿：",
            "“时间不是一条河，而是一面被‘观察’才存在的湖。锚点之所以循环，是因为湖底"
            "有一个意识，紧紧抓着岸，不肯让水面平静。”",
            "“毁灭锚点，是炸掉湖；成为锚点，是换一个人沉下去；而真正的终结，是让那个"
            "意识，心甘情愿地，松开手。”",
            "他凝视着你：“那个意识，孩子，就是第 0 次循环里的你。”"
        },
        .choices =
        {
            Choice{ .text = "请求借走他的手稿",        .target = "library_manuscript" },
            Choice{ .text = "问他该如何‘松手’",        .target = "library_how" },
        }
    },
    Scene{
        .id = "library_manuscript",
        .title = "图书馆 · 阅读区",
        .paragraphs =
        {
            "陈教授把毕生手稿放进你手里：",
            "“我已时日无多，这些留给你。记住：终结不是答案，放下才是。”"
        },
        .choices =
        {
            Choice{ .text = "（收好时间理论手稿 S4）", .target = "library_hall",
                    .grantedClue = "S4" },
        }
    },
    Scene{
        .id = "library_how",
        .title = "图书馆 · 阅读区",
        .paragraphs =
        {
            "陈教授轻声说：",
            "“第 0 次的你，是为了救一个人，才自愿走进锚点。要让他松手，你得先回到"
            "第 0 次，去看看他到底想救谁，然后，替他完成那个没能完成的告别。”",
            "“去城北的旧防空洞吧，那里是一切开始的地方。”"
        },
        .choices =
        {
            Choice{ .text = "记下，起身",              .target = "library_hall" },
        }
    },
    Scene{
        .id = "library_archive",
        .title = "图书馆 · 档案室",
        .paragraphs =
        {
            "档案室里，有一卷被单独封存的文献，标题是《灯塔工程 30 年观察记录》。",
            "记录显示：30 年来，锚点每隔一段时间就会“呼唤”一个面容相同的人，",
            "而这一次，被呼唤的人，是你。"
        },
        .choices =
        {
            Choice{ .text = "退出档案室",              .target = "library_hall" },
        }
    },
    Scene{
        .id = "library_bookdrop",
        .title = "图书馆 · 还书箱",
        .paragraphs =
        {
            "还书箱里有一本逾期 30 年的书，书名是《如何与明天告别》，",
            "借书卡上的名字，和你的名字，一字不差。"
        },
        .choices =
        {
            Choice{ .text = "收起书",                  .target = "library_gate" },
        }
    },
    Scene{
        .id = "death_library",
        .title = "",
        .paragraphs =
        {
            "你在书架深处点起明火，无数泛黄的书页在火光里翻飞，像一群被惊醒的、",
            "写满了时间的白鸟。你和这座图书馆，一起烧成了灰雾里最亮的一盏灯。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【扩展区域 19】城市综合管廊（灯塔工程遗址 · A5）
    //==========================================================================
    Scene{
        .id = "pipe_entrance",
        .title = "综合管廊 · 入口",
        .paragraphs =
        {
            "旧军事地图上标注的入口，藏在一处废弃的检修井下。",
            "你顺着铁梯爬下，一股潮湿、混着金属味的空气扑面而来，",
            "眼前是一条望不到头的、灯火零星的地下长廊。"
        },
        .choices =
        {
            Choice{ .text = "沿主廊前行",              .target = "pipe_main" },
            Choice{ .text = "搜索入口处的值班室",      .target = "pipe_office" },
            Choice{ .text = "返回地面",                .target = "hub" },
        }
    },
    Scene{
        .id = "pipe_main",
        .title = "综合管廊 · 主廊",
        .paragraphs =
        {
            "主廊两侧是成排的管道与电缆，头顶偶尔有水滴落下。",
            "前方出现岔路：左侧通向一扇锈死的密封门，右侧通向一片被水淹没的低区。"
        },
        .choices =
        {
            Choice{ .text = "用撬棍撬开密封门",        .target = "pipe_deep",
                    .requireAll = { "tool_crowbar" } },
            Choice{ .text = "涉水进入低区",            .target = "death_pipe" },
            Choice{ .text = "退回入口",                .target = "pipe_entrance" },
        }
    },
    Scene{
        .id = "pipe_office",
        .title = "综合管廊 · 值班室",
        .paragraphs =
        {
            "值班室的墙上挂着一张 30 年前的管廊图，上面用红笔圈出了一个区域，",
            "标注着：“灯塔工程 · 禁区”。",
            "桌上有一本值班日志，最后一天的记录写着：",
            "“禁区里的人，已经在里面待了 72 小时，他说他能听见‘未来’在敲门。”"
        },
        .choices =
        {
            Choice{ .text = "退出值班室",              .target = "pipe_entrance" },
        }
    },
    Scene{
        .id = "pipe_deep",
        .title = "综合管廊 · 禁区",
        .paragraphs =
        {
            "密封门后，是一个被封存了 30 年的实验室，门上的铭牌还在：",
            "“灯塔工程 · 首次锚定实验舱”。",
            "舱内的设备早已锈蚀，中央的实验椅上，留着一道深深的、被反复摩挲的痕迹。"
        },
        .choices =
        {
            Choice{ .text = "查看封存的实验档案",      .target = "pipe_archive" },
            Choice{ .text = "坐上实验椅",              .target = "death_pipe_chair" },
            Choice{ .text = "退出禁区",                .target = "pipe_main" },
        }
    },
    Scene{
        .id = "pipe_archive",
        .title = "综合管廊 · 实验档案",
        .paragraphs =
        {
            "档案的最后一页，贴着一张照片——首次锚定实验的志愿者，",
            "那张脸，和你一模一样。",
            "下面的记录写着：",
            "“实验体编号 000，状态：稳定。备注：他自愿留下，理由是——‘我答应过她，"
            "要把明天带回来’。实验体拒绝结束，锚点由此形成。”",
            "你终于触碰到了第 0 次循环的边缘：困住全城的，是一个关于承诺的执念。"
        },
        .choices =
        {
            Choice{ .text = "（收好时间实验档案 A5）", .target = "pipe_deep",
                    .grantedClue = "A5" },
        }
    },
    Scene{
        .id = "death_pipe",
        .title = "",
        .paragraphs =
        {
            "污水在你踏入低区的瞬间漫过胸口，青黑细丝顺着管道，从四面八方游来。",
            "你在地下的河水里，听见 30 年前的自己，在实验舱里，轻轻敲着门。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_pipe_chair",
        .title = "",
        .paragraphs =
        {
            "你坐上实验椅的瞬间，舱门自行锁死，30 年前的电流穿过身体。",
            "你看见自己的一生在眼前倒流，然后，停在了第 0 次循环——",
            "这一次，你成了那个“拒绝结束”的人。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【扩展区域 20】旧军事防空洞（ECHO 真正起源 · 苏敏博士）
    //==========================================================================
    Scene{
        .id = "bunker_gate",
        .title = "旧军事防空洞 · 洞口",
        .paragraphs =
        {
            "城北的山坳里，旧军事防空洞的混凝土入口被藤蔓半掩，",
            "铁门上方的红漆标语已经斑驳：“深挖洞，广积粮”。",
            "门缝里，透出一丝不属于这个时代的、冷白色的光。"
        },
        .choices =
        {
            Choice{ .text = "推开沉重的铁门",          .target = "bunker_hall" },
            Choice{ .text = "搜索洞口的废弃岗亭",      .target = "bunker_guard" },
            Choice{ .text = "返回地面",                .target = "hub" },
        }
    },
    Scene{
        .id = "bunker_guard",
        .title = "防空洞 · 岗亭",
        .paragraphs =
        {
            "岗亭里有一本 30 年前的出入登记，最后一页写着：",
            "“灯塔工程转 ECHO 计划，全体科研人员转入地下，首席：苏敏。”",
            "“最高机密：实验体 000 不得唤醒，不得转移，不得……结束。”"
        },
        .choices =
        {
            Choice{ .text = "退回去",                  .target = "bunker_gate" },
        }
    },
    Scene{
        .id = "bunker_hall",
        .title = "防空洞 · 主通道",
        .paragraphs =
        {
            "通道两侧是一扇扇密封的舱门，冷白色的灯管还在嗡嗡作响。",
            "尽头的指挥室门虚掩，里面传来一个女人平静的、自言自语的声音。"
        },
        .choices =
        {
            Choice{ .text = "走进指挥室",              .target = "bunker_su" },
            Choice{ .text = "进入军火库",              .target = "bunker_armory" },
            Choice{ .text = "进入最深的实验舱",        .target = "bunker_core" },
            Choice{ .text = "退出防空洞",              .target = "bunker_gate" },
        }
    },
    Scene{
        .id = "bunker_su",
        .title = "防空洞 · 指挥室",
        .paragraphs =
        {
            "指挥室里，一位穿着白大褂的女人转过身，她的面容憔悴，眼神却异常清明：",
            "“我是苏敏，ECHO 计划的首席，也是……唯一一个，从循环里‘醒来’，却没有逃的人。”",
            "“我守在这里，是为了等一个能终结它的人。现在，你来了。”"
        },
        .choices =
        {
            Choice{ .text = "问她第 0 次循环的真相",  .target = "bunker_su_truth" },
            Choice{ .text = "请求看她的日志",          .target = "bunker_su_log" },
            Choice{ .text = "接过她的录音带",          .target = "bunker_su_tape" },
            Choice{ .text = "向苏敏道别，退出指挥室",  .target = "bunker_hall" },
        }
    },
    Scene{
        .id = "bunker_su_truth",
        .title = "防空洞 · 指挥室",
        .paragraphs =
        {
            "苏敏的声音很轻：",
            "“30 年前，有一个男人，为了救回他在意外中离世的女儿，自愿走进锚点，",
            "成为实验体 000。他以为能把女儿‘带回来’，却因为‘拒绝结束’，把每一次回溯",
            "都变成了新的播种。”",
            "“他的女儿，就是后来的林晚。而林晚，又为了救回她的父亲，走进了锚点。”",
            "“两代人的执念，首尾相扣，这就是循环真正的形状。”",
            "她凝视着你：“而你，是这对父女，在无数次循环里，共同的‘面容’。”"
        },
        .choices =
        {
            Choice{ .text = "沉默良久",                .target = "bunker_su" },
        }
    },
    Scene{
        .id = "bunker_su_log",
        .title = "防空洞 · 指挥室",
        .paragraphs =
        {
            "苏敏把一本日志递给你：",
            "“这是我作为知情者，能给你的最后一块拼图。”",
            "日志里写着：“我们都不是受害者，是某个人的选择，把我们困在这里；",
            "而终结它的人，必须替那个选择，完成那场迟到了 30 年的告别。”"
        },
        .choices =
        {
            Choice{ .text = "（收好叛逃科学家日志 A4）",.target = "bunker_su",
                    .grantedClue = "A4" },
        }
    },
    Scene{
        .id = "bunker_su_tape",
        .title = "防空洞 · 指挥室",
        .paragraphs =
        {
            "苏敏把一盘录音带放进你手心：",
            "“这是我留给后来者的最后陈述。如果有一天你走进广播大楼，请把它，",
            "和真相一起，播给这座城市听。”",
            "“我已经……在这条时间线里，走到尽头了。”"
        },
        .choices =
        {
            Choice{ .text = "（收好苏敏的录音带）",    .target = "bunker_su",
                    .grantedClue = "tool_tape" },
        }
    },
    Scene{
        .id = "bunker_armory",
        .title = "防空洞 · 军火库",
        .paragraphs =
        {
            "军火库里，成排的武器早已锈蚀，唯独一个密封箱里，",
            "几包军用 C4 炸药保存完好，旁边是引爆器。",
            "箱门上贴着一张警告：“此物可从物理上摧毁锚点，亦会摧毁锚点内的一切。”"
        },
        .choices =
        {
            Choice{ .text = "取走炸药与引爆器",        .target = "bunker_armory_take" },
            Choice{ .text = "空手退出",                .target = "bunker_hall" },
        }
    },
    Scene{
        .id = "bunker_armory_take",
        .title = "防空洞 · 军火库",
        .paragraphs =
        {
            "你把炸药和引爆器收好。这是最暴烈的一条路——",
            "以同归于尽，换一个再也不会循环的、寂静的黎明。"
        },
        .choices =
        {
            Choice{ .text = "（收好军用炸药）",        .target = "bunker_armory",
                    .grantedClue = "tool_bomb" },
        }
    },
    Scene{
        .id = "bunker_core",
        .title = "防空洞 · 最深实验舱",
        .paragraphs =
        {
            "最深的实验舱里，一具躯体悬浮在营养液中，",
            "他的脸——是 30 年前的你。",
            "舱壁上，用指甲刻满了同一句话：",
            "“我答应过晚晚，要把明天带回来。”",
            "你站在舱前，终于明白：要终结循环，不是杀死他，而是走到他面前，",
            "轻轻地告诉他——“晚晚，已经不在了。你可以，休息了。”"
        },
        .choices =
        {
            Choice{ .text = "把手贴在舱壁上，完成告别",.target = "bunker_core_farewell" },
            Choice{ .text = "强行砸开培养舱",          .target = "death_bunker" },
            Choice{ .text = "退出实验舱",              .target = "bunker_hall" },
        }
    },
    Scene{
        .id = "bunker_core_farewell",
        .title = "防空洞 · 最深实验舱",
        .paragraphs =
        {
            "你把手贴在冰冷的舱壁上，轻声说：",
            "“我知道你想救她。但晚晚，已经不在了。这 30 年，你已经……尽力了。”",
            "“睡吧。明天，我们会替你，好好地活。”",
            "营养液里，那张沉睡的脸，缓缓流下一行泪，紧锁的眉头，第一次舒展。",
            "你感到整座防空洞，连同 30 年的执念，都在这一刻，轻轻地、松开了。"
        },
        .choices =
        {
            Choice{ .text = "（收好第 0 次循环日记）", .target = "bunker_core",
                    .grantedClue = "tool_diary" },
        }
    },
    Scene{
        .id = "death_bunker",
        .title = "",
        .paragraphs =
        {
            "培养舱碎裂的瞬间，30 年高浓度的封存雾气喷涌而出。",
            "你在白雾里看见一个男人向你伸出手，他说：“谢谢你，终于让我……”",
            "话音未落，你和他，一起沉进了第 0 次循环的、最深处。"
        },
        .choices = {}, .isDeath = true
    },

    // （场景表结束）
};

//==============================================================================
// 三、结局表
//==============================================================================
const std::vector<Ending> kEndings =
{
    //---------------- 基础线索结局 ------------------------------------------
    Ending{
        .id = "ending_basic",
        .title = "结局 · 漫长的等待",
        .paragraphs =
        {
            "你把三条基础线索在地上排开：被提前封装的水、带着编号的实验动物、"
            "印着“回声”的传单。",
            "你得出结论：这是一场通过水源扩散的瘟疫，政府正在救援。只要锁好门、"
            "不喝自来水、安静等待，黎明总会来。",
            "于是你锁死避难所的门，数着储备一天天等下去。",
            "门外的灰雾没有散。广播里的女声循环了无数遍，然后彻底沉默。你成了"
            "最后一个等待黎明的人——而黎明，永远停在了下一次循环里。",
            "【达成结局：漫长的等待】——帷幕仍未揭开，更深的真相在雾中等你。"
        }
    },
    //---------------- 进阶线索结局 ------------------------------------------
    Ending{
        .id = "ending_advanced",
        .title = "结局 · 撕幕者",
        .paragraphs =
        {
            "你把三条进阶线索与基础线索并排：被“同时按下”的开关、写着坐标的"
            "加密通讯、水箱上的作业记录。",
            "你得出结论：这不是天灾，是一场精心策划的人为泄露，代号“回声”。",
            "你冲回电台、登上信号塔，把真相向全城广播。你听见远处有人回应、有人"
            "哭泣、有人开始行动。",
            "然后你死了——总会死的。你再次在锚点醒来，广播从未发生，世界从未"
            "听过。你在无数条时间线里重复着同一场冲锋，而真相永远停在下一次循环。",
            "【达成结局：撕幕者】——你撕开了帷幕，却还走不出帷幕。"
        }
    },
    //---------------- 真结局（三档全收集）----------------------------------
    Ending{
        .id = "ending_true",
        .title = "真结局 · 锚点之外",
        .paragraphs =
        {
            "九条线索在你脑海中拼成完整图景：病毒通过回溯传播，你是零号宿主，"
            "你每一次“醒来”，都是向过去的一次播种。",
            "这一次，你没有冲向任何一个方向。",
            "你把所有线索、坐标、密码和真相封进防水箱，交给在诊所里活下来的男人——"
            "他的爱人，那位流调员，会知道该怎么做。",
            "然后你回到锚点核心，在光中平躺下来，亲手关闭了它。你感到意识在下沉，"
            "像一条终于停摆的钟。",
            "这一次，你没有再醒来。",
            "……时间线第一次不再被改写。灰雾没有被“带回”过去，于是它在这条"
            "时间线里，被活着的人一点点击退。",
            "多年以后，重建的城市里，孩子们在干净的喷泉边追逐。公园的新长椅上"
            "钉着一块铜牌，上面写着：",
            "“献给那个没有回来的人。是你让明天，终于只属于明天。”",
            "【达成真结局：锚点之外】——感谢你走完了《末日回声》。"
        }
    },

    //==========================================================================
    // ===== 扩展篇 · 新增结局（8 个）=====
    //==========================================================================

    //---------------- 结局 4：同归于尽（炸锚点）----------------------------
    Ending{
        .id = "ending_bomb",
        .title = "结局 · 同归于尽",
        .paragraphs =
        {
            "你把炸药贴在锚点上，按下引爆器。",
            "没有惊天动地的巨响，只有一种“玻璃碎裂”的声音——无数条时间线，像镜子一样",
            "同时崩裂。你看见自己的身体在光里碎成无数片，每一片里，都有一个不同的你。",
            "锚点被毁了，循环，再也无法发生。",
            "但锚点里的一切——包括你，包括那些还没来得及离开的人，也一起，归于寂静。",
            "灰雾在没有锚点的世界里，失去了“回到过去”的能力，于是它在这一条时间线里，",
            "慢慢消散。多年以后，城市重建，人们在废墟中央，立起一块无字的碑。",
            "没有人知道碑下埋着谁，只知道：有一个人，用自己和所有循环的毁灭，",
            "为这个世界，强行换来了一个不会再重来的明天。",
            "【达成结局：同归于尽】——最暴烈的黎明，由最深的沉默铸就。"
        }
    },

    //---------------- 结局 5：新锚点（自己成为锚点）------------------------
    Ending{
        .id = "ending_newanchor",
        .title = "结局 · 新锚点",
        .paragraphs =
        {
            "你想起陈教授的话，想起苏敏的日志，想起那个在舱里沉了 30 年的男人。",
            "你走进光中，张开双臂，让锚点的光，顺着你的血管，流进你的身体。",
            "你感到整座城市的重量，轻轻落在你的肩上。你成了新的锚点。",
            "你做的第一件事，是把所有人，温柔地“推”出了循环——你看见他们在光里醒来，",
            "看见林晚、苏敏、老船长、小雨，一个接一个，回到了没有灰雾的明天。",
            "而你，留在了光里。",
            "你不再“拒绝结束”，所以你没有制造新的循环；你只是静静地，",
            "像一座灯塔，守着这个你亲手送出的世界。",
            "多年以后，孩子们在海边看见灯塔，会问：“那里面，有人吗？”",
            "大人们会说：“有。是他，让我们回了家。”",
            "【达成结局：新锚点】——你用一生的孤独，换了所有人的归途。"
        }
    },

    //---------------- 结局 6：远航（乘船离开）------------------------------
    Ending{
        .id = "ending_voyage",
        .title = "结局 · 远航",
        .paragraphs =
        {
            "你握紧船钥匙，发动远星号。",
            "老船长在码头挥手，他没有上船，只是大声喊：“替我，看看雾外面的世界！”",
            "货轮缓缓驶离港口，灯塔的光在身后一圈圈扫过，像在为你送行。",
            "你在海上航行了很多天，灰雾，被远远地甩在身后。",
            "终于，在一个清晨，你看见海平面上，亮起了金色的、真实的阳光。",
            "你活了下来，带着这座城市的真相，驶向了远方。",
            "只是你常常会在夜里惊醒，想起那些没能上船的人，想起锚点，想起那座",
            "还在循环里的城市。",
            "你把真相写进书里，让全世界知道。很多年后，一支舰队，载着解药和希望，",
            "驶向了那片灰雾——",
            "而这一切，始于你当年，没有选择同归于尽，而是选择了，活下去。",
            "【达成结局：远航】——逃离不是懦弱，是把火种，带到了风吹得到的地方。"
        }
    },

    //---------------- 结局 7：虚假黎明（假真结局）--------------------------
    Ending{
        .id = "ending_false_dawn",
        .title = "结局 · 虚假黎明",
        .paragraphs =
        {
            "你广播了真相，你关闭了锚点，你看见灰雾散去，看见阳光，看见人群欢呼。",
            "你如释重负，闭上眼，心想：终于，结束了。",
            "……",
            "“……第 100 次。”",
            "你猛地睁开眼，回到了避难所，回到了最初的那张床。",
            "墙上的刻字，比你记忆里，多了一行：",
            "“第 100 次。别相信‘阳光’，那是锚点，给放弃者的奖赏。”",
            "你以为走到了终点，其实只是走进了锚点为你准备好的、最温柔的一层梦。",
            "真正的终结，从来不是‘看见黎明’，而是在没有黎明的承诺里，依然选择，",
            "不再醒来。",
            "【达成结局：虚假黎明】——最亮的光，往往藏着最深的雾。"
        }
    },

    //---------------- 结局 8：传承（把希望交给孩子）------------------------
    Ending{
        .id = "ending_legacy",
        .title = "结局 · 传承",
        .paragraphs =
        {
            "你没有独自关闭锚点，也没有选择远航。",
            "你回到第一小学，把全部真相、坐标、密码，还有那瓶抗体，",
            "一起，放进了小雨的画夹里。",
            "你蹲下来，看着她的眼睛：",
            "“叔叔要去一个很远的地方。这些，交给你。等你长大了，",
            "你会知道，该怎么用它们，救回这座城市。”",
            "小雨似懂非懂，却用力点头，把画夹，紧紧抱在怀里。",
            "然后你走向锚点，用自己的停止，为这一代，画上了句号。",
            "你没能看见黎明，但你把看见黎明的眼睛，留给了下一代。",
            "很多年后，一位年轻的女科学家，带着一本画夹和一瓶抗体，",
            "走进了重建的城市。她的名字，叫小雨。",
            "【达成结局：传承】——希望从不是一个人走完，而是一代，传给一代。"
        }
    },

    //---------------- 结局 9：永恒旁观者（拒绝介入）------------------------
    Ending{
        .id = "ending_watcher",
        .title = "结局 · 永恒旁观者",
        .paragraphs =
        {
            "你站在锚点前，手里握着全部真相，却在最后一刻，",
            "轻轻地，放下了。",
            "你想起陈教授的话：“终结，是让那个意识，心甘情愿地松手。”",
            "而你，既不愿成为他，也不愿杀死他，更不愿，替他决定。",
            "你选择，不介入。",
            "你转身，走出了光，走出了锚点，走到了一个，时间线之外的地方。",
            "在那里，你成了一个永恒的旁观者：你看见城市在循环里沦陷，又在循环里重生，",
            "看见有人同归于尽，有人远航，有人传承，有人，终于走到了真结局。",
            "你为他们每一个人，在心里，轻轻地鼓掌。",
            "你没有拯救任何人，但你也没有，再伤害任何人。",
            "你只是，安静地看着，看着这场名为《末日回声》的戏，",
            "在你面前，演了一遍，又一遍。",
            "【达成结局：永恒旁观者】——不入轮回，亦不渡人，是为旁观。"
        }
    },

    //---------------- 结局 10：共生（与病毒共存）---------------------------
    Ending{
        .id = "ending_symbiosis",
        .title = "结局 · 共生",
        .paragraphs =
        {
            "你握着那瓶来自植物园的抗体，做出了一个，没有人想过的选择。",
            "你没有炸毁锚点，没有成为锚点，也没有关闭它。",
            "你把抗体，注入了锚点。",
            "青黑的细丝没有死亡，而是在抗体的歌声里，缓缓沉睡，",
            "像一头被安抚的野兽，在你脚边，安静地蜷伏。",
            "你没有终结循环，而是让病毒，与时间，达成了和解。",
            "锚点依然在，回溯依然可能，但沉睡的病毒，不再被“播种”。",
            "你走出核心，看见灰雾里，人们开始苏醒，",
            "他们的身上，带着沉睡的印记，却好好地，活着。",
            "城市没有被拯救，也没有被毁灭，它只是，学会了，与末日共生。",
            "多年以后，孩子们生下来，胸口就有一道淡淡的青黑纹路，",
            "他们与它一起长大，一起，把这座城市，建成了雾里，最温柔的家园。",
            "【达成结局：共生】——不是所有的病，都要被治愈；有时，和解，才是出路。"
        }
    },

    //---------------- 结局 11：完美真结局（15 条全齐）---------------------
    Ending{
        .id = "ending_true_complete",
        .title = "完美真结局 · 黎明回声",
        .paragraphs =
        {
            "你带着全部 15 条真相，站在锚点前。",
            "这一次，你不再是一个人。",
            "你想起林晚的广播、苏敏的坚守、老船长的等待、老赵的地图、",
            "陈教授的手稿、小雨的壁画、植物园的抗体，",
            "还有那个在舱里，沉了 30 年的，第 0 次的你。",
            "你没有炸毁，没有取代，没有沉睡，也没有旁观。",
            "你走到光前，轻轻地，把手贴了上去，像在安抚一个，哭了很久的孩子。",
            "“我都知道了。”你轻声说，",
            "“你想救晚晚，林晚想救你，你们都，尽力了。”",
            "“现在，把一切，交给我。你们，都可以，回家了。”",
            "光里，你看见那个男人和他的女儿，终于在 30 年后，重逢。",
            "他们向你深深鞠躬，然后，化作两缕温暖的光，消散在，第一缕真实的晨风里。",
            "锚点没有被毁，也没有沉睡，它在你完整的“看见”里，",
            "轻轻地，圆满地，合上了。",
            "这一次，你没有死，也没有再醒来——因为你，已经，走到了梦的尽头。",
            "你在真实的阳光里睁开眼，城市完好，人群欢呼，",
            "林晚、苏敏、老船长、老赵、陈教授、小雨，",
            "他们都在，他们都，活着。",
            "多年以后，重建的城市中央，建起了一座回声广场。",
            "广场上没有碑，只有一圈永远流动的、清澈的水，",
            "孩子们在水边追逐，笑声，在阳光里，荡出一圈又一圈的回声。",
            "“献给那个，看见一切，也原谅一切的人。”",
            "“是你让我们，终于，和明天，重逢。”",
            "【达成完美真结局：黎明回声】——你已集齐全部真相，《末日回声》，至此圆满。"
        }
    },
};

} // anonymous namespace

//==============================================================================
// 四、只读查询接口实现
//==============================================================================
const std::vector<Clue>& clues() { return kClues; }
const std::vector<Scene>& scenes() { return kScenes; }
const std::vector<Ending>& endings() { return kEndings; }

const Clue* findClue(std::string_view id)
{
    auto it = std::find_if(kClues.begin(), kClues.end(),
        [&](const Clue& c) { return c.id == id; });
    return it == kClues.end() ? nullptr : &*it;
}

const Scene* findScene(std::string_view id)
{
    auto it = std::find_if(kScenes.begin(), kScenes.end(),
        [&](const Scene& s) { return s.id == id; });
    return it == kScenes.end() ? nullptr : &*it;
}

const Ending* findEnding(std::string_view id)
{
    auto it = std::find_if(kEndings.begin(), kEndings.end(),
        [&](const Ending& e) { return e.id == id; });
    return it == kEndings.end() ? nullptr : &*it;
}

std::size_t clueCountOfTier(ClueTier tier)
{
    return static_cast<std::size_t>(
        std::count_if(kClues.begin(), kClues.end(),
            [&](const Clue& c) { return c.tier == tier; }));
}

} // namespace story

