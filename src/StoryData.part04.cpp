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
