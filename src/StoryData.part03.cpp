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
