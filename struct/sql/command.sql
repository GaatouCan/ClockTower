CREATE TABLE `command`
(
    `id` BIGINT UNSIGNED NOT NULL COMMENT '玩家ID',
    `creator` TEXT COMMENT '头像框',
    `type` VARCHAR(255) COMMENT '更新时间',
    `param` VARCHAR(255) COMMENT '更新时间',
    `create_time` BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '更新时间',
    `finish_time` BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '更新时间',
    `extend` TEXT COMMENT '扩展字段',
    PRIMARY KEY (`id`)
) ENGINE = InnoDB CHARACTER SET = utf16 COMMENT '运营命令';