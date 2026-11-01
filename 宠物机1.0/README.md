# ZhiYi ESP32 Tamagotchi

一个运行在 ESP32-S3 N16R8 与 240×320 ST7789 显示屏上的中文电子宠物项目。它使用横屏显示、双按键操作，并围绕角色“吱咿”提供宠物养成、抽卡、房间互动与投硬币技能小游戏。

## 当前功能

- 饱食度、困倦值、洁净度与心情值养成系统
- 喂食、睡觉、洗澡、狂气奖励与宠物礼物
- 吱咿专属标志“吱咿的记忆”、服装解锁与持久化存档
- 170 张人格卡池、单抽/十连、60 抽三星保底与卡册
- 房间互动与技能硬币投掷：正面概率由当前心情决定
- 金色、红色、绿色三种硬币材质
- 吱咿开机动画、白天/夜晚场景、室内场景与吃饭/睡觉动画

## 硬件连接

| 模块引脚 | ESP32-S3 N16R8 |
| --- | --- |
| TFT VCC | 3V3 |
| TFT GND | GND |
| TFT SCL / SCK | GPIO12 |
| TFT SDA / MOSI | GPIO11 |
| TFT DC | GPIO13 |
| TFT CS | GPIO10 |
| TFT RST | GPIO14 |
| 按键 A | GPIO4，另一端接 GND |
| 按键 B | GPIO5，另一端接 GND |

按键使用内部上拉，按下为低电平。

## 构建

在 VS Code 中安装 PlatformIO IDE 扩展后，打开本 `firmware` 目录。

```powershell
pio run -e tamagotchi
```

硬件检查程序：

```powershell
pio run -e hardware_check
```

项目面向 16 MB Flash / 8 MB OPI PSRAM 的 ESP32-S3。当前分区配置提供约 12 MB 应用空间。

## 资源生成

`src/zhiyi_assets.h`、`src/zhiyi_boot.h`、`src/coin_assets.h`、`src/zhiyi_badge.h` 与中文字库都是构建所需的已生成资源，因此已被提交。原始素材包和中间帧默认不纳入仓库。

如需重新生成，参考 `scripts/` 下的工具脚本，并将其中的本地资源路径改为你的实际路径。

## 开源说明

本项目为个人非官方同人/学习项目，与原作品权利方无关联。请仅在拥有相应素材使用权的前提下分发或替换素材。

项目沿用仓库中的 GNU GPL v2 许可证。
