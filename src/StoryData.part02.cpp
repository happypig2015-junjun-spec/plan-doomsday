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
