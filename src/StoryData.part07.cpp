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
