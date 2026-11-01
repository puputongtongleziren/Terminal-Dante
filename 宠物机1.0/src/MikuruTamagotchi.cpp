#include <SPI.h>
#include <TFT_eSPI.h>
#include <Preferences.h>
#include "zhiyi_cjk_font.h"
#include "zhiyi_badge.h"
#include "zhiyi_boot.h"
#include "coin_assets.h"
#include "gacha_pool.h"
//#include <WiFi.h>
#include "pest.h"
#include "weber.h"
#include "baby.h"
#include "freg.h"
#include "shop.h"
#include "door.h"
#include "box.h"
#include "computer.h"
#include "warn.h"
#include "roomwhite.h"
#include "roomblack.h"
#include "f01.h"
#include "f02.h"
#include "f03.h"
#include "f04.h"
#include "f05.h"
#include "f06.h"
#include "f07.h"
#include "f08.h"
#include "f09.h"
#include "f10.h"
#include "f11.h"
#include "f12.h"
#include "f13.h"
#include "f14.h"
#include "f15.h"
#include "f16.h"
#include "f17.h"
#include "f18.h"
#include "f19.h"
#include "f20.h"
#include "f21.h"
#include "f22.h"

#include "zhiyi_assets.h"
#include "lunacy_icon.h"

// The original feeding code repeats these symbols in many branches. Keep its
// gameplay intact while routing every frame to ZhiYi's new eating animation.
#define mikurueat   zhiyi_eat[0]
#define mikurueat2  zhiyi_eat[1]
#define mikurueat3  zhiyi_eat[2]
#define mikurueat4  zhiyi_eat[3]
#define mikurueat5  zhiyi_eat[4]
#define mikurueat6  zhiyi_eat[5]
#define mikurueat7  zhiyi_eat[6]
#define mikurueat8  zhiyi_eat[7]
#define mikurueat9  zhiyi_eat[8]
#define mikurueat10 zhiyi_eat[9]
#define mikurueat11 zhiyi_eat[11]

void mainicon();
void Generalmenu();
void state_count();
void Food();
void maingif();
void Sleep();
void Roomfood();
void Roomsleep();
void Roomshower();
void Shower();
void Shop();
void Box();
void peace();
void Rooms();
void CoinFlipGame();
void Time();
void mainselect();
void drawCurrentRoom();
void setRoomScene(uint8_t scene);
const uint16_t* currentSleepFrame(uint8_t frame);
void waitForButtonsReleased();
bool runSleepMenu();
void playBootAnimation();
void addLunacy(int amount);
void captureActionRewards();
void updatePeriodicLunacy();
void loadPersistentState();

enum RoomScene : uint8_t {
  ROOM_WHITE,
  ROOM_GRAY,
  ROOM_RED,
  ROOM_BLUE,
  ROOM_GREEN
};


class LandscapeDisplay
{
public:
  void begin()
  {
    panel.begin();
    scaledImage = static_cast<uint16_t*>(ps_malloc(240 * 240 * sizeof(uint16_t)));
    if (!scaledImage) scaledImage = static_cast<uint16_t*>(malloc(240 * 240 * sizeof(uint16_t)));
  }
  void setRotation(uint8_t) { panel.setRotation(1); }
  void setSwapBytes(bool enabled) { panel.setSwapBytes(enabled); }
  void setViewport(int32_t, int32_t, int32_t, int32_t, bool) { panel.setViewport(40, 0, 240, 240, true); }
  int32_t width() const { return 128; }
  int32_t height() const { return 128; }
  void setTextColor(uint16_t fg, uint16_t bg) { panel.setTextColor(fg, bg); }
  void setTextColor(uint16_t fg) { panel.setTextColor(fg); }
  void setTextDatum(uint8_t datum) { panel.setTextDatum(datum); }
  void setTextSize(uint8_t size) { panel.setTextSize(size); }
  void setCursor(int32_t x, int32_t y) { panel.setCursor(mapX(x), mapY(y)); }
  size_t println(const char* text) { return panel.println(text); }
  void fillScreen(uint32_t color) { panel.fillScreen(color); }
  void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
  {
    const int32_t x0 = mapX(x), y0 = mapY(y);
    panel.fillRect(x0, y0, mapEdge(x + w) - x0, mapEdge(y + h) - y0, color);
  }
  void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
  {
    const int32_t x0 = mapX(x), y0 = mapY(y);
    panel.drawRect(x0, y0, mapEdge(x + w) - x0, mapEdge(y + h) - y0, color);
  }
  void drawCircle(int32_t x, int32_t y, int32_t radius, uint32_t color)
  {
    panel.drawCircle(mapX(x), mapY(y), (radius * 15 + 4) / 8, color);
  }
  void drawPixel(int32_t x, int32_t y, uint32_t color)
  {
    fillRect(x, y, 1, 1, color);
  }
  void drawString(const String& text, int32_t x, int32_t y, uint8_t font = 1)
  {
    const uint8_t scaledFont = (font == 1) ? 2 : ((font == 2) ? 4 : ((font == 4) ? 6 : font));
    panel.drawString(text, mapX(x), mapY(y), scaledFont);
  }
  void drawString(const char* text, int32_t x, int32_t y, uint8_t font = 1)
  {
    drawString(String(text), x, y, font);
  }
  void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t* source)
  {
    if (!source || w <= 0 || h <= 0 || w > 128 || h > 128 || !scaledImage) return;
    const int32_t outW = mapEdge(w), outH = mapEdge(h);
    for (int32_t dy = 0; dy < outH; ++dy) {
      const int32_t sy = (dy * h) / outH;
      for (int32_t dx = 0; dx < outW; ++dx) {
        const int32_t sx = (dx * w) / outW;
        scaledImage[dy * outW + dx] = source[sy * w + sx];
      }
    }
    panel.pushImage(mapX(x), mapY(y), outW, outH, scaledImage);
  }
private:
  TFT_eSPI panel = TFT_eSPI();
  uint16_t* scaledImage = nullptr;
  static int32_t mapEdge(int32_t value) { return (value * 15 + 4) / 8; }
  static int32_t mapX(int32_t x) { return mapEdge(x); }
  static int32_t mapY(int32_t y) { return mapEdge(y); }
};

LandscapeDisplay tft;
Preferences statePrefs;

int frame1 = 0;
int buttonbefore47 = 1, buttonstate47 = 1;
int buttonbefore0 = 1, buttonstate0 = 1;
int mainx1 = 1, mainy1 = 0, mainx2 = 30, mainy2 = 30;
int mainnum = 0;
int foodx1 = 22, foody1 = 22, foodx2 = 30, foody2 = 30;
int sleepx1 = 5, sleepy1 = 13, sleepx2 = 76, sleepy2 = 21;
int timex1 = 5, timey1 = 18, timex2 = 76, timey2 = 21;
int shopx1 = 0, shopy1 = 0, shopx2 = 128, shopy2 = 100;
int modex1 = 5, modey1 = 23, modex2 = 76, modey2 = 21;
int gamex1 = 5, gamey1 = 23, gamex2 = 76, gamey2 = 21;
int n_main = 32;
int n_food = 52;
int food_page = 0;
int sleeptime = 0, sleep_page = 0;
float Hun1, Fat1, Cle1;
int Hun, Fat, Cle, Exp;
int Hun_mas = 0, Fat_mas = 0, Cle_mas = 0;
int Hun_s = 0, Fat_s = 0, Cle_s = 0; //开始吃之前判断需要补充多少饥饿值所用的变量
int Exp_mas = 0;
int gachaPity = 0;
uint32_t gachaTotal = 0;
uint32_t highAttributeMs = 0;
uint32_t lastRewardTick = 0;
uint32_t lastMoodTick = 0;
uint32_t moodDecayMs = 0;
uint32_t moodHighMs = 0;
uint32_t badgeCooldownMs = 3600000UL;
uint32_t lastMoodSaveMs = 0;
int Mood = 60;
uint8_t badgeCount = 0;
bool clothingUnlocked = false;
bool badgePending = false;
int rewardHunMasSeen = 0, rewardCleMasSeen = 0;
bool persistentStateLoaded = false;
static constexpr uint16_t GACHA_TOTAL_CARDS = gacha1StarCount + gacha2StarCount + gacha3StarCount;
static constexpr uint16_t GACHA_SEEN_BYTES = (GACHA_TOTAL_CARDS + 7) / 8;
uint8_t gachaSeen[GACHA_SEEN_BYTES] = {};
uint16_t gachaSeenCount = 0;
int roomwhitex = 83, roomwhitey = 16;
int boxx1 = 101, boxy1 = 99, boxx2 = 26, boxy2 = 18;
bool darkmode = 0;//主屏幕模式0为亮，1为暗；
bool capeEnabled = false;
uint8_t coinMaterial = 0;
RoomScene currentRoomScene = ROOM_WHITE;
uint32_t lastMainFrameMs = 0;

int gamenum_left;
int gamenum_right;
int gamex = 9;
char buff[512];

const CjkGlyph12* findCjkGlyph(uint16_t code)
{
  size_t low = 0, high = ZHIYI_CJK_GLYPH_COUNT;
  while (low < high) {
    const size_t mid = low + (high - low) / 2;
    const uint16_t candidate = pgm_read_word(&zhiyiCjkGlyphs[mid].code);
    if (candidate == code) return &zhiyiCjkGlyphs[mid];
    if (candidate < code) low = mid + 1;
    else high = mid;
  }
  return nullptr;
}

void drawCjkText(const char* text, int x, int y, uint16_t color = TFT_WHITE, int maxX = 128, uint8_t maxLines = 1)
{
  const uint8_t* p = reinterpret_cast<const uint8_t*>(text);
  uint8_t line = 0;
  while (*p && line < maxLines) {
    if (*p < 0x80) {
      if (*p == '\n') { ++p; x = 30; y += 13; ++line; continue; }
      if (x + 6 > maxX) { x = 30; y += 13; ++line; if (line >= maxLines) break; }
      char ascii[2] = {static_cast<char>(*p++), 0};
      tft.setTextColor(color, TFT_BLACK);
      tft.drawString(ascii, x, y, 1);
      x += 6;
      continue;
    }
    uint16_t code = 0;
    if ((*p & 0xE0) == 0xC0) { code = (*p & 0x1F) << 6; code |= p[1] & 0x3F; p += 2; }
    else if ((*p & 0xF0) == 0xE0) { code = (*p & 0x0F) << 12; code |= (p[1] & 0x3F) << 6; code |= p[2] & 0x3F; p += 3; }
    else if ((*p & 0xF8) == 0xF0) { p += 4; x += 12; continue; }
    else { ++p; continue; }

    if (x + 12 > maxX) { x = 30; y += 13; ++line; if (line >= maxLines) break; }
    const CjkGlyph12* glyph = findCjkGlyph(code);
    if (!glyph) { x += 12; continue; }
    for (uint8_t row = 0; row < 12; ++row) {
      const uint16_t bits = (pgm_read_byte(&glyph->bitmap[row * 2]) << 8) |
                            pgm_read_byte(&glyph->bitmap[row * 2 + 1]);
      for (uint8_t col = 0; col < 12; ++col) {
        if (bits & (1 << (11 - col))) tft.drawPixel(x + col, y + row, color);
      }
    }
    x += 12;
  }
}

void drawZhiyiBadge(int x, int y, int width = 24, int height = 26)
{
  for (int dy = 0; dy < height; ++dy) {
    const int sy = (dy * ZHIYI_BADGE_H) / height;
    for (int dx = 0; dx < width; ++dx) {
      const int sx = (dx * ZHIYI_BADGE_W) / width;
      const uint8_t alpha = pgm_read_byte(&zhiyiBadgeAlpha[sy * ZHIYI_BADGE_W + sx]);
      if (alpha >= 96) {
        const uint16_t color = pgm_read_word(&zhiyiBadgeRgb565[sy * ZHIYI_BADGE_W + sx]);
        tft.drawPixel(x + dx, y + dy, color);
      }
    }
  }
}

void showLunacyReward(int amount)
{
  if (amount <= 0) return;
  tft.fillScreen(TFT_BLACK);
  String reward = "+" + String(amount) + " 狂气获得";
  drawCjkText(reward.c_str(), 14, 52, TFT_WHITE);
  delay(1000);
}

void drawLunacyToast(int amount)
{
  if (amount <= 0) return;
  tft.fillRect(0, 106, 128, 18, TFT_BLACK);
  String toast = "+" + String(amount) + " 狂气";
  drawCjkText(toast.c_str(), 26, 107, TFT_WHITE);
}

void savePersistentState()
{
  if (!persistentStateLoaded) return;
  statePrefs.putInt("lunacy", Exp);
  statePrefs.putInt("pity", gachaPity);
  statePrefs.putUInt("pulls", gachaTotal);
  statePrefs.putUInt("highms", highAttributeMs);
  statePrefs.putInt("mood", Mood);
  statePrefs.putUChar("badges", badgeCount);
  statePrefs.putBool("clothes", clothingUnlocked);
  statePrefs.putUChar("coinMat", coinMaterial);
  statePrefs.putBool("badgePending", badgePending);
  statePrefs.putUInt("badgeCd", badgeCooldownMs);
  statePrefs.putBytes("seen", gachaSeen, sizeof(gachaSeen));
  statePrefs.putUShort("seenCount", gachaSeenCount);
}

void loadPersistentState()
{
  statePrefs.begin("zhiyi", false);
  Exp = statePrefs.getInt("lunacy", 0);
  gachaPity = statePrefs.getInt("pity", 0);
  gachaTotal = statePrefs.getUInt("pulls", 0);
  highAttributeMs = statePrefs.getUInt("highms", 0);
  Mood = statePrefs.getInt("mood", 60);
  badgeCount = statePrefs.getUChar("badges", 0);
  if (badgeCount > 5) badgeCount = 5;
  clothingUnlocked = statePrefs.getBool("clothes", badgeCount >= 5);
  coinMaterial = min<uint8_t>(statePrefs.getUChar("coinMat", 0), 2);
  badgePending = statePrefs.getBool("badgePending", false);
  badgeCooldownMs = min<uint32_t>(statePrefs.getUInt("badgeCd", 3600000UL), 3600000UL);
  if (statePrefs.getBytesLength("seen") == sizeof(gachaSeen)) {
    statePrefs.getBytes("seen", gachaSeen, sizeof(gachaSeen));
    gachaSeenCount = 0;
    for (uint16_t i = 0; i < GACHA_TOTAL_CARDS; ++i)
      if (gachaSeen[i / 8] & (1 << (i % 8))) ++gachaSeenCount;
  }
  rewardHunMasSeen = Hun_mas;
  rewardCleMasSeen = Cle_mas;
  lastRewardTick = millis();
  lastMoodTick = millis();
  lastMoodSaveMs = millis();
  persistentStateLoaded = true;
}

void addLunacy(int amount)
{
  Exp = max(0, Exp + amount);
  savePersistentState();
}

void addMood(int amount)
{
  const int previous = Mood;
  Mood = constrain(Mood + amount, 0, 100);
  if (previous < 100 && Mood == 100 && badgeCount < 5) badgePending = true;
  savePersistentState();
}

void showGiftReward(int amount)
{
  tft.fillScreen(TFT_BLACK);
  drawCjkText("得到吱咿的礼物！", 12, 42, TFT_WHITE);
  String reward = "狂气 +" + String(amount);
  drawCjkText(reward.c_str(), 28, 68, TFT_WHITE);
  tft.pushImage(101, 66, 20, 18, lunacy_icon);
  delay(1200);
}

void updateMoodAndBadges()
{
  const uint32_t now = millis();
  const uint32_t delta = now - lastMoodTick;
  lastMoodTick = now;
  if (badgeCount < 5)
    badgeCooldownMs = min<uint32_t>(3600000UL, badgeCooldownMs + delta);
  moodDecayMs += delta;
  while (moodDecayMs >= 900000UL) {
    moodDecayMs -= 900000UL;
    const uint8_t lowStats = (Hun <= 30) + (Fat <= 30) + (Cle <= 30);
    Mood = max(0, Mood - 1 - 5 * lowStats);
  }

  bool rewardOverlayShown = false;
  if (Mood >= 75) {
    moodHighMs += delta;
    while (moodHighMs >= 1800000UL) {
      moodHighMs -= 1800000UL;
      const int gift = random(1, 11);
      addLunacy(gift);
      showGiftReward(gift);
      rewardOverlayShown = true;
    }
  } else {
    moodHighMs = 0;
  }

  if (badgePending) {
    // A full-mood event only pays out if its one-hour reward window is ready.
    // Events that occur during the cooldown are not queued for later.
    badgePending = false;
    if (badgeCooldownMs >= 3600000UL) {
      badgeCooldownMs = 0;
      if (badgeCount < 5) {
        ++badgeCount;
        tft.fillScreen(TFT_BLACK);
        drawZhiyiBadge(52, 20, 24, 26);
        drawCjkText("吱咿的记忆", 30, 54, TFT_WHITE);
        delay(1000);
        rewardOverlayShown = true;
      } else {
        const int fullMoodGift = random(1, 11);
        addLunacy(fullMoodGift);
        showGiftReward(fullMoodGift);
        rewardOverlayShown = true;
      }
    }
    if (badgeCount >= 5 && !clothingUnlocked) {
      clothingUnlocked = true;
      tft.fillScreen(TFT_BLACK);
      drawCjkText("已获得新时装", 22, 55, TFT_WHITE);
      delay(1200);
      rewardOverlayShown = true;
    }
  }
  if (now - lastMoodSaveMs >= 60000UL) {
    savePersistentState();
    lastMoodSaveMs = now;
  }
  if (rewardOverlayShown) mainicon();
}

void captureActionRewards() { rewardHunMasSeen = Hun_mas; rewardCleMasSeen = Cle_mas; }

void updatePeriodicLunacy()
{
  const uint32_t now = millis();
  const uint32_t delta = now - lastRewardTick;
  lastRewardTick = now;
  if (((Hun + Fat + Cle) / 3) > 75) {
    highAttributeMs += delta;
    while (highAttributeMs >= 3600000UL) {
      highAttributeMs -= 3600000UL;
      addLunacy(5);
    }
    savePersistentState();
  }
}

const char* gachaName(uint8_t rarity, uint8_t index)
{
  if (rarity == 3) return gacha3Star[index % gacha3StarCount];
  if (rarity == 2) return gacha2Star[index % gacha2StarCount];
  return gacha1Star[index % gacha1StarCount];
}

uint16_t gachaGlobalIndex(uint8_t rarity, uint8_t index)
{
  if (rarity == 1) return index % gacha1StarCount;
  if (rarity == 2) return gacha1StarCount + (index % gacha2StarCount);
  return gacha1StarCount + gacha2StarCount + (index % gacha3StarCount);
}

void markGachaSeen(uint16_t index)
{
  if (index >= GACHA_TOTAL_CARDS) return;
  const uint8_t mask = 1 << (index % 8);
  if (!(gachaSeen[index / 8] & mask)) {
    gachaSeen[index / 8] |= mask;
    ++gachaSeenCount;
  }
}

uint8_t rollGachaRarity(uint8_t index, uint8_t count)
{
  if (gachaPity >= 59) return 3;
  const long roll = random(100);
  if (count == 10 && index == 9) return (roll < 84) ? 2 : 3;
  if (roll < 3) return 3;
  if (roll < 16) return 2;
  return 1;
}

void showGachaResult(const uint8_t* rarities, const char* const* names, uint8_t count)
{
  const uint8_t pages = (count + 3) / 4;
  for (uint8_t page = 0; page < pages; ++page) {
    tft.fillScreen(TFT_BLACK);
    drawCjkText("抽卡结果", 35, 1, TFT_WHITE);
    for (uint8_t row = 0; row < 4; ++row) {
      const uint8_t i = page * 4 + row;
      if (i >= count) break;
      const uint16_t color = rarities[i] == 3 ? 0xFEA0 : (rarities[i] == 2 ? 0xC100 : 0xB208);
      String rarity = String(i + 1) + ". " + String(rarities[i]) + "*";
      tft.setTextColor(color, TFT_BLACK);
      tft.drawString(rarity, 2, 18 + row * 26, 1);
      drawCjkText(names[i], 30, 16 + row * 26, color, 127, 2);
    }
    drawCjkText("按键翻页/返回", 25, 118, TFT_WHITE);
    while (digitalRead(4) == HIGH && digitalRead(5) == HIGH) delay(10);
    waitForButtonsReleased();
  }
}

void performGacha(uint8_t count)
{
  const int cost = (count == 10) ? 9 : 1;
  if (Exp < cost) {
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Not enough Lunacy", 4, 48, 2);
    delay(900);
    return;
  }

  addLunacy(-cost);
  uint8_t rarities[10] = {};
  const char* names[10] = {};
  for (uint8_t i = 0; i < count; ++i) {
    rarities[i] = rollGachaRarity(i, count);
    const uint8_t poolSize = rarities[i] == 3 ? gacha3StarCount : (rarities[i] == 2 ? gacha2StarCount : gacha1StarCount);
    const uint8_t poolIndex = random(poolSize);
    names[i] = gachaName(rarities[i], poolIndex);
    markGachaSeen(gachaGlobalIndex(rarities[i], poolIndex));
    ++gachaTotal;
    if (rarities[i] == 3) gachaPity = 0;
    else ++gachaPity;
  }
  savePersistentState();
  showGachaResult(rarities, names, count);
}

void drawCurrentRoom()
{
  const uint16_t* frame = nullptr;

  switch (currentRoomScene)
  {
    case ROOM_WHITE:
      tft.pushImage(0, 12, 128, 104, roomwhite);
      tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_WHITE);
      return;
    case ROOM_GRAY:
      if (roomwhitex == 45) frame = capeEnabled ? zhiyi_roomgray_computer_cape[0] : zhiyi_roomgray_computer_off[0];
      else if (roomwhitex == 100) frame = capeEnabled ? zhiyi_roomgray_bed_cape[0] : zhiyi_roomgray_bed_off[0];
      else frame = capeEnabled ? zhiyi_roomgray_exit_cape[0] : zhiyi_roomgray_exit_off[0];
      break;
    case ROOM_RED:
      frame = (roomwhitex == 96)
        ? (capeEnabled ? zhiyi_roomred_food_cape[0] : zhiyi_roomred_food_off[0])
        : (capeEnabled ? zhiyi_roomred_exit_cape[0] : zhiyi_roomred_exit_off[0]);
      break;
    case ROOM_BLUE:
      frame = (roomwhitex == 20)
        ? (capeEnabled ? zhiyi_roomblue_bath_cape[0] : zhiyi_roomblue_bath_off[0])
        : (capeEnabled ? zhiyi_roomblue_exit_cape[0] : zhiyi_roomblue_exit_off[0]);
      break;
    case ROOM_GREEN:
      frame = (roomwhitex == 119)
        ? (capeEnabled ? zhiyi_roomgreen_swing_cape[0] : zhiyi_roomgreen_swing_off[0])
        : (capeEnabled ? zhiyi_roomgreen_exit_cape[0] : zhiyi_roomgreen_exit_off[0]);
      break;
  }

  tft.pushImage(0, 12, 128, 104, frame);
}

void setRoomScene(uint8_t scene)
{
  currentRoomScene = static_cast<RoomScene>(scene);
  drawCurrentRoom();
}

const uint16_t* currentSleepFrame(uint8_t frame)
{
  const uint8_t index = frame % 8;
  return capeEnabled ? zhiyi_sleep_cape[index] : zhiyi_sleep_off[index];
}

void playBootAnimation()
{
  tft.fillScreen(TFT_BLACK);
  for (uint8_t frame = 0; frame < ZHIYI_BOOT_FRAME_COUNT; ++frame) {
    tft.pushImage(0, 0, 128, 128, zhiyiBootFrames[frame]);
    delay(333);
  }
}

void waitForButtonsReleased()
{
  while (digitalRead(4) == LOW || digitalRead(5) == LOW)
  {
    delay(10);
  }
}

void playSleepAnimation(uint32_t durationMs)
{
  const uint32_t startedAt = millis();
  uint8_t sleepFrame = 0;
  tft.fillScreen(TFT_BLACK);

  while (millis() - startedAt < durationMs)
  {
    tft.pushImage(0, 0, 128, 128, currentSleepFrame(sleepFrame));
    sleepFrame = (sleepFrame + 1) % 8;
    delay(300);
  }
}

bool runSleepMenu()
{
  static const char* const labels[] = {"5m (30s)", "20m (2m)", "1h (6m)", "8h (48m)", "Exit"};
  static const uint32_t durations[] = {30000UL, 120000UL, 360000UL, 2880000UL};
  static const int recovery[] = {2, 10, 20, 50};
  uint8_t selection = 0;

  while (true)
  {
    tft.fillScreen(TFT_BLACK);
    for (uint8_t i = 0; i < 5; ++i)
    {
      tft.drawString(labels[i], 10, 15 + i * 20, 2);
    }
    tft.drawRect(5, 13 + selection * 20, 95, 19, TFT_WHITE);

    while (digitalRead(4) != LOW && digitalRead(5) != LOW)
    {
      delay(10);
    }

    if (digitalRead(5) == LOW)
    {
      selection = (selection + 1) % 5;
      waitForButtonsReleased();
      continue;
    }

    waitForButtonsReleased();
    if (selection == 4) return false;

    playSleepAnimation(durations[selection]);
    Fat_mas += recovery[selection];
    addMood(10);
    // Sleep rewards use real elapsed minutes (the animation is scaled 10:1).
    const int sleepLunacy = static_cast<int>(durations[selection] / 60000UL);
    addLunacy(sleepLunacy);
    showLunacyReward(sleepLunacy);
    return true;
  }
}

//void wifi_scan()
//{
//  tft.setTextColor(TFT_WHITE, TFT_BLACK);
//  tft.fillScreen(TFT_BLACK);
//  tft.setTextDatum(MC_DATUM);
//  tft.setTextSize(1);
//
//  tft.drawString("Scan Network", tft.width() / 2, tft.height() / 2);
//  
//  WiFi.mode(WIFI_STA);
//  WiFi.disconnect();
//  delay(100);
//  int16_t wifi_n = WiFi.scanNetworks();//有一定延迟
//
//  tft.fillScreen(TFT_BLACK);
//  if (wifi_n == 0) 
//  {
//    tft.drawString("no networks found", tft.width() / 2, tft.height() / 2);
//  } 
//  else 
//  {
//    tft.setTextDatum(TL_DATUM);
//    tft.setCursor(0, 0);
//    Serial.printf("Found %d net\n", wifi_n);
//    for (int i = 0; i < wifi_n; ++i) 
//    {
//      sprintf(buff,
//              "[%d]:%s(%d)",
//              i + 1,
//              WiFi.SSID(i).c_str(),
//              WiFi.RSSI(i));
//      tft.println(buff);
//    }
//  }
//  //WiFi.mode( WIFI_OFF );
//  tft.setTextDatum(TL_DATUM);
//}

void mainicon()
{
  tft.fillScreen(TFT_BLACK);
  if(Hun < 60 || Fat < 60 || Cle < 60)
  {
    tft.pushImage(2, 1, 28, 28, warn);
  }
  else
  {
    tft.pushImage(2, 1, 28, 28, pest);
  }
  tft.pushImage(34, 1, 29, 29, weber);
  tft.pushImage(66, 1, 29, 29, baby);
  tft.pushImage(98, 1, 29, 29, freg);
  tft.pushImage(2, 99, 29, 29, shop);
  tft.pushImage(34, 99, 29, 29, door);
  tft.pushImage(66, 99, 29, 29, box);
  tft.pushImage(98, 99, 29, 29, computer);
}

void drawLunacyGain(int amount)
{
  tft.drawString((amount >= 0 ? "+" : "") + String(amount), 18, 70, 2);
  tft.pushImage(55, 68, 20, 18, lunacy_icon);
  tft.drawString("Lunacy", 80, 70, 2);
}

void Generalmenu()
{
  if(digitalRead(4) == 0)
  {
    delay(300);
    tft.fillScreen(TFT_BLACK);
    while(digitalRead(4) == 1 && digitalRead(5) == 1)
    {
      drawCjkText("饱食度", 9, 9, TFT_WHITE);
      tft.drawString(String(Hun), 84, 9, 2);
      tft.drawString("%", 110, 9, 2);
    
      drawCjkText("困倦值", 9, 39, TFT_WHITE);
      tft.drawString(String(Fat), 84, 39, 2);
      tft.drawString("%", 110, 39, 2);
    
      drawCjkText("洁净度", 9, 69, TFT_WHITE);
      tft.drawString(String(Cle), 84, 69, 2);
      tft.drawString("%", 110, 69, 2);
    
    drawCjkText("心情", 9, 96, TFT_WHITE);
    tft.drawString(String(Mood), 84, 96, 2);
    tft.drawString("%", 110, 96, 2);
    drawCjkText("狂气", 9, 113, TFT_WHITE);
    tft.pushImage(55, 111, 20, 18, lunacy_icon);
    tft.drawString(String(Exp), 80, 113, 1);
    }
    delay(300);
    mainicon();
  }
}

void state_count()
{
  Hun1 = 50.0f - (millis() / 720000.0f);
  Fat1 = 70.0f - (millis() / 259200000.0f);
  Cle1 = 50.0f - (millis() / 1500000.0f);
  Hun = round(Hun1) + Hun_mas;
  Fat = round(Fat1) + Fat_mas;
  Cle = round(Cle1) + Cle_mas;
  Hun = constrain(Hun, 0, 100);
  Fat = constrain(Fat, 0, 100);
  Cle = constrain(Cle, 0, 100);
  if(Fat > 100)
  {
    Fat = 100;
  }

  else if(Cle > 100)
  {
    Cle = 100;
  }
}

void Food()
{
  if(digitalRead(4) == 0)
  {
    delay(300);
    tft.fillScreen(TFT_BLACK);
    food_page = 0;
    tft.pushImage(23, 23, 29, 29, f01);
    tft.pushImage(75, 23, 29, 29, f02);
    tft.pushImage(23, 75, 29, 29, f03);
    tft.pushImage(75, 75, 29, 29, f22);

    Hun_s = Hun; //把饥饿值Hun赋值给判断用的Hun_s
    tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
    while(!(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 74))
    {
      if(digitalRead(5) == 0 && foodx1 == 22 && foody1 == 22)
      {
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_BLACK);
        foodx1 += n_food;
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && foodx1 == 74 && foody1 == 22)
      {
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_BLACK);
        foodx1 -= n_food;
        foody1 += n_food;
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && foodx1 == 22 && foody1 == 74)
      {
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_BLACK);
        foodx1 += n_food;
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && foodx1 == 74 && foody1 == 74)
      {
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_BLACK);
        foodx1 -= n_food;
        foody1 -= n_food;
        if(food_page < 6)
        {
          food_page += 1;
          if(food_page == 1)
          {
            tft.pushImage(23, 23, 29, 29, f04);
            tft.pushImage(75, 23, 29, 29, f05);
            tft.pushImage(23, 75, 29, 29, f06);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 2)
          {
            tft.pushImage(23, 23, 29, 29, f07);
            tft.pushImage(75, 23, 29, 29, f08);
            tft.pushImage(23, 75, 29, 29, f09);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 3)
          {
            tft.pushImage(23, 23, 29, 29, f10);
            tft.pushImage(75, 23, 29, 29, f11);
            tft.pushImage(23, 75, 29, 29, f12);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 4)
          {
            tft.pushImage(23, 23, 29, 29, f13);
            tft.pushImage(75, 23, 29, 29, f14);
            tft.pushImage(23, 75, 29, 29, f15);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 5)
          {
            tft.pushImage(23, 23, 29, 29, f16);
            tft.pushImage(75, 23, 29, 29, f17);
            tft.pushImage(23, 75, 29, 29, f18);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 6)
          {
            tft.pushImage(23, 23, 29, 29, f19);
            tft.pushImage(75, 23, 29, 29, f20);
            tft.pushImage(23, 75, 29, 29, f21);
            tft.pushImage(75, 75, 29, 29, f22);
          }

        }
        else if(food_page >= 6)
        {
          food_page = 0;
          tft.pushImage(23, 23, 29, 29, f01);
          tft.pushImage(75, 23, 29, 29, f02);
          tft.pushImage(23, 75, 29, 29, f03);
          tft.pushImage(75, 75, 29, 29, f22);
        }
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
        delay(300);
      }
      
      
      //以下为点击确认键（0）时的反应 00
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 0)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f01);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f01);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f01);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f01);
          tft.pushImage(75, 23, 29, 29, f02);
          tft.pushImage(23, 75, 29, 29, f03);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 0)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f02);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f02);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f02);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f01);
          tft.pushImage(75, 23, 29, 29, f02);
          tft.pushImage(23, 75, 29, 29, f03);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 0)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f03);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f03);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f03);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f01);
          tft.pushImage(75, 23, 29, 29, f02);
          tft.pushImage(23, 75, 29, 29, f03);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 00

      //以下为点击确认键（0）时的反应 01
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 1)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f04);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f04);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f04);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f04);
          tft.pushImage(75, 23, 29, 29, f05);
          tft.pushImage(23, 75, 29, 29, f06);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 1)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f05);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f05);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f05);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f04);
          tft.pushImage(75, 23, 29, 29, f05);
          tft.pushImage(23, 75, 29, 29, f06);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 1)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f06);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f06);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f06);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f04);
          tft.pushImage(75, 23, 29, 29, f05);
          tft.pushImage(23, 75, 29, 29, f06);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 01

      //以下为点击确认键（0）时的反应 02
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 2)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f07);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f07);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f07);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f07);
          tft.pushImage(75, 23, 29, 29, f08);
          tft.pushImage(23, 75, 29, 29, f09);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 2)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f08);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f08);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f08);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f07);
          tft.pushImage(75, 23, 29, 29, f08);
          tft.pushImage(23, 75, 29, 29, f09);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 2)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f09);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f09);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f09);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f07);
          tft.pushImage(75, 23, 29, 29, f08);
          tft.pushImage(23, 75, 29, 29, f09);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 02

      //以下为点击确认键（0）时的反应 03
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 3)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f10);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f10);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f10);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f10);
          tft.pushImage(75, 23, 29, 29, f11);
          tft.pushImage(23, 75, 29, 29, f12);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 3)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f11);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f11);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f11);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f10);
          tft.pushImage(75, 23, 29, 29, f11);
          tft.pushImage(23, 75, 29, 29, f12);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 3)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f12);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f12);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f12);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f10);
          tft.pushImage(75, 23, 29, 29, f11);
          tft.pushImage(23, 75, 29, 29, f12);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 03

      //以下为点击确认键（0）时的反应 04
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 4)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f13);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f13);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f13);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f13);
          tft.pushImage(75, 23, 29, 29, f14);
          tft.pushImage(23, 75, 29, 29, f15);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 4)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f14);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f14);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f14);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f13);
          tft.pushImage(75, 23, 29, 29, f14);
          tft.pushImage(23, 75, 29, 29, f15);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 4)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f15);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f15);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f15);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f13);
          tft.pushImage(75, 23, 29, 29, f14);
          tft.pushImage(23, 75, 29, 29, f15);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 04

      //以下为点击确认键（0）时的反应 05
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 5)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f16);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f16);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f16);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f16);
          tft.pushImage(75, 23, 29, 29, f17);
          tft.pushImage(23, 75, 29, 29, f18);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 5)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f17);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f17);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f17);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f16);
          tft.pushImage(75, 23, 29, 29, f17);
          tft.pushImage(23, 75, 29, 29, f18);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 5)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f18);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f18);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f18);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f16);
          tft.pushImage(75, 23, 29, 29, f17);
          tft.pushImage(23, 75, 29, 29, f18);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 05

      //以下为点击确认键（0）时的反应 06
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 6)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f19);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f19);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f19);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f19);
          tft.pushImage(75, 23, 29, 29, f20);
          tft.pushImage(23, 75, 29, 29, f21);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 6)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f20);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f20);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f20);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f19);
          tft.pushImage(75, 23, 29, 29, f20);
          tft.pushImage(23, 75, 29, 29, f21);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 6)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f21);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f21);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f21);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f19);
          tft.pushImage(75, 23, 29, 29, f20);
          tft.pushImage(23, 75, 29, 29, f21);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 06
    }
    foodx1 = 22, foody1 = 22, foodx2 = 30, foody2 = 30;
    delay(300);
    mainicon();
  }
}

void maingif()
{
  const uint32_t interval = darkmode ? 170 : 280;
  if (millis() - lastMainFrameMs < interval) return;

  lastMainFrameMs = millis();
  if (darkmode)
  {
    tft.pushImage(0, 35, 128, 58,
                  capeEnabled ? zhiyi_forest2_cape[frame1] : zhiyi_forest2_off[frame1]);
    frame1 = (frame1 + 1) % 20;
  }
  else
  {
    tft.pushImage(0, 34, 128, 60,
                  capeEnabled ? zhiyi_forest1_cape[frame1] : zhiyi_forest1_off[frame1]);
    frame1 = (frame1 + 1) % 72;
  }
}

void Sleep()
{
  if (digitalRead(4) != LOW) return;
  waitForButtonsReleased();
  runSleepMenu();
  mainicon();
}

void Roomfood()
{
  if(digitalRead(4) == 0)
  {
    delay(300);
    tft.fillScreen(TFT_BLACK);
    food_page = 0;
    tft.pushImage(23, 23, 29, 29, f01);
    tft.pushImage(75, 23, 29, 29, f02);
    tft.pushImage(23, 75, 29, 29, f03);
    tft.pushImage(75, 75, 29, 29, f22);

    Hun_s = Hun; //把饥饿值Hun赋值给判断用的Hun_s
    tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
    while(!(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 74))
    {
      if(digitalRead(5) == 0 && foodx1 == 22 && foody1 == 22)
      {
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_BLACK);
        foodx1 += n_food;
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && foodx1 == 74 && foody1 == 22)
      {
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_BLACK);
        foodx1 -= n_food;
        foody1 += n_food;
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && foodx1 == 22 && foody1 == 74)
      {
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_BLACK);
        foodx1 += n_food;
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && foodx1 == 74 && foody1 == 74)
      {
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_BLACK);
        foodx1 -= n_food;
        foody1 -= n_food;
        if(food_page < 6)
        {
          food_page += 1;
          if(food_page == 1)
          {
            tft.pushImage(23, 23, 29, 29, f04);
            tft.pushImage(75, 23, 29, 29, f05);
            tft.pushImage(23, 75, 29, 29, f06);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 2)
          {
            tft.pushImage(23, 23, 29, 29, f07);
            tft.pushImage(75, 23, 29, 29, f08);
            tft.pushImage(23, 75, 29, 29, f09);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 3)
          {
            tft.pushImage(23, 23, 29, 29, f10);
            tft.pushImage(75, 23, 29, 29, f11);
            tft.pushImage(23, 75, 29, 29, f12);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 4)
          {
            tft.pushImage(23, 23, 29, 29, f13);
            tft.pushImage(75, 23, 29, 29, f14);
            tft.pushImage(23, 75, 29, 29, f15);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 5)
          {
            tft.pushImage(23, 23, 29, 29, f16);
            tft.pushImage(75, 23, 29, 29, f17);
            tft.pushImage(23, 75, 29, 29, f18);
            tft.pushImage(75, 75, 29, 29, f22);
          }

          else if(food_page == 6)
          {
            tft.pushImage(23, 23, 29, 29, f19);
            tft.pushImage(75, 23, 29, 29, f20);
            tft.pushImage(23, 75, 29, 29, f21);
            tft.pushImage(75, 75, 29, 29, f22);
          }

        }
        else if(food_page >= 6)
        {
          food_page = 0;
          tft.pushImage(23, 23, 29, 29, f01);
          tft.pushImage(75, 23, 29, 29, f02);
          tft.pushImage(23, 75, 29, 29, f03);
          tft.pushImage(75, 75, 29, 29, f22);
        }
        tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
        delay(300);
      }
      
      
      //以下为点击确认键（0）时的反应 00
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 0)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f01);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f01);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f01);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f01);
          tft.pushImage(75, 23, 29, 29, f02);
          tft.pushImage(23, 75, 29, 29, f03);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 0)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f02);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f02);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f02);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f01);
          tft.pushImage(75, 23, 29, 29, f02);
          tft.pushImage(23, 75, 29, 29, f03);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 0)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f03);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f03);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f03);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f01);
          tft.pushImage(75, 23, 29, 29, f02);
          tft.pushImage(23, 75, 29, 29, f03);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 00

      //以下为点击确认键（0）时的反应 01
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 1)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f04);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f04);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f04);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f04);
          tft.pushImage(75, 23, 29, 29, f05);
          tft.pushImage(23, 75, 29, 29, f06);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 1)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f05);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f05);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f05);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f04);
          tft.pushImage(75, 23, 29, 29, f05);
          tft.pushImage(23, 75, 29, 29, f06);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 1)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f06);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f06);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f06);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f04);
          tft.pushImage(75, 23, 29, 29, f05);
          tft.pushImage(23, 75, 29, 29, f06);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 01

      //以下为点击确认键（0）时的反应 02
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 2)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f07);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f07);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f07);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f07);
          tft.pushImage(75, 23, 29, 29, f08);
          tft.pushImage(23, 75, 29, 29, f09);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 2)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f08);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f08);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f08);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f07);
          tft.pushImage(75, 23, 29, 29, f08);
          tft.pushImage(23, 75, 29, 29, f09);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 2)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f09);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f09);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f09);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f07);
          tft.pushImage(75, 23, 29, 29, f08);
          tft.pushImage(23, 75, 29, 29, f09);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 02

      //以下为点击确认键（0）时的反应 03
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 3)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f10);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f10);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f10);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f10);
          tft.pushImage(75, 23, 29, 29, f11);
          tft.pushImage(23, 75, 29, 29, f12);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 3)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f11);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f11);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f11);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f10);
          tft.pushImage(75, 23, 29, 29, f11);
          tft.pushImage(23, 75, 29, 29, f12);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 3)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f12);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f12);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f12);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f10);
          tft.pushImage(75, 23, 29, 29, f11);
          tft.pushImage(23, 75, 29, 29, f12);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 03

      //以下为点击确认键（0）时的反应 04
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 4)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f13);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f13);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f13);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f13);
          tft.pushImage(75, 23, 29, 29, f14);
          tft.pushImage(23, 75, 29, 29, f15);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 4)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f14);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f14);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f14);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f13);
          tft.pushImage(75, 23, 29, 29, f14);
          tft.pushImage(23, 75, 29, 29, f15);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 4)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f15);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f15);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f15);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f13);
          tft.pushImage(75, 23, 29, 29, f14);
          tft.pushImage(23, 75, 29, 29, f15);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 04

      //以下为点击确认键（0）时的反应 05
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 5)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f16);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f16);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f16);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f16);
          tft.pushImage(75, 23, 29, 29, f17);
          tft.pushImage(23, 75, 29, 29, f18);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 5)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f17);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f17);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f17);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f16);
          tft.pushImage(75, 23, 29, 29, f17);
          tft.pushImage(23, 75, 29, 29, f18);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 5)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f18);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f18);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f18);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f16);
          tft.pushImage(75, 23, 29, 29, f17);
          tft.pushImage(23, 75, 29, 29, f18);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 05

      //以下为点击确认键（0）时的反应 06
      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 22 && food_page == 6)
      {
        if(Hun_s <= 85)
        {
          Hun_mas += 15;
          Cle_mas -= 5;
          addMood(15);
          addLunacy(2);
          Hun_s += 15; //在饥饿值增加的同时，判断用的饥饿值也增加

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f19);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f19);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f19);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f19);
          tft.pushImage(75, 23, 29, 29, f20);
          tft.pushImage(23, 75, 29, 29, f21);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +15%", 26, 4, 2);
          drawLunacyToast(1);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 74 && foody1 == 22 && food_page == 6)
      {
        if(Hun_s <= 95)
        {
          Hun_mas += 5;
          Cle_mas -= 2;
          addMood(5);
          addLunacy(1);
          Hun_s += 5;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f20);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f20);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f20);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f19);
          tft.pushImage(75, 23, 29, 29, f20);
          tft.pushImage(23, 75, 29, 29, f21);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +5%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }

      else if(digitalRead(4) == 0 && foodx1 == 22 && foody1 == 74 && food_page == 6)
      {
        if(Hun_s <= 98)
        {
          Hun_mas += 2;
          Cle_mas -= 1;
          addMood(2);
          addLunacy(1);
          Hun_s += 2;

          //以下为吃东西的动画
          tft.fillScreen(TFT_BLACK);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          tft.pushImage(49, 49, 29, 29, f21);
          delay(500);
          tft.fillRect(49, 49, 29, 29, TFT_BLACK);
          tft.pushImage(49, 74, 29, 29, f21);
          delay(500);
          tft.fillRect(49, 74, 29, 29, TFT_BLACK);
          tft.pushImage(49, 92, 29, 29, f21);
          tft.drawRect(0, 88, 128, 40, TFT_WHITE);
          delay(500);
          tft.pushImage(0, 0, 128, 88, mikurueat);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat2);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat3);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat4);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat5);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat6);
          delay(200);
          tft.fillRect(49, 92, 29, 29, TFT_BLACK);
          tft.pushImage(0, 0, 128, 88, mikurueat7);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat8);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat9);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat10);
          delay(200);
          tft.pushImage(0, 0, 128, 88, mikurueat11);
          delay(200);
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(23, 23, 29, 29, f19);
          tft.pushImage(75, 23, 29, 29, f20);
          tft.pushImage(23, 75, 29, 29, f21);
          tft.pushImage(75, 75, 29, 29, f22);
          tft.drawRect(foodx1, foody1, foodx2, foody2, TFT_WHITE);
          //以上为吃东西的动画
          
          tft.drawString("Hunger +2%", 26, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
        else
        {
          tft.drawString("Can't eat that much", 4, 4, 2);
          delay(1000);
          tft.fillRect(0, 106, 128, 18, TFT_BLACK);
          tft.fillRect(2, 2, 125, 18, TFT_BLACK);
        }
      }
      //以上为点击确认键（0）时的反应 06
    }
    foodx1 = 22, foody1 = 22, foodx2 = 30, foody2 = 30;
    delay(300);
    currentRoomScene = ROOM_RED;
    roomwhitex = 9;
    roomwhitey = 93;
    drawCurrentRoom();
    delay(300);
  }
}

void Roomsleep()
{
  if (digitalRead(4) != LOW) return;
  waitForButtonsReleased();
  runSleepMenu();
  currentRoomScene = ROOM_GRAY;
  roomwhitex = 58;
  roomwhitey = 117;
  drawCurrentRoom();
  return;

  if(digitalRead(4) == 0)
  {
    delay(300);
    tft.fillScreen(TFT_BLACK);
    tft.drawString("5 minutes", 10, 15, 2);
    tft.drawString("20 minutes", 10, 40, 2);
    tft.drawString("1 hour", 10, 65, 2);
    tft.drawString("8 hours", 10, 90, 2);
    sleepx1 = 5, sleepy1 = 13, sleepx2 = 76, sleepy2 = 21;
    sleep_page = 0;
    tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_WHITE);
    
    while(!(sleeptime == 150 || sleeptime == 600 || sleeptime == 1800 || sleeptime == 14400))
    {
      if(digitalRead(5) == 0 && sleepy1 == 13 && sleep_page == 0)
      {
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_BLACK);
        sleepy1 += 25;
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && sleepy1 == 38 && sleep_page == 0)
      {
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_BLACK);
        sleepy1 += 25;
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && sleepy1 == 63 && sleep_page == 0)
      {
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_BLACK);
        sleepy1 += 25;
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_WHITE);
        delay(300);
      }
  
      else if(digitalRead(5) == 0 && sleepy1 == 88 && sleep_page == 0)
      {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Exit", 10, 15, 2);
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_BLACK);
        sleepy1 = 13;
        sleep_page += 1;
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_WHITE);
        delay(300);
      }

      else if(digitalRead(5) == 0 && sleepy1 == 13 && sleep_page == 1)
      {
        tft.fillScreen(TFT_BLACK);
        tft.drawString("5 minutes", 10, 15, 2);
        tft.drawString("20 minutes", 10, 40, 2);
        tft.drawString("1 hour", 10, 65, 2);
        tft.drawString("8 hours", 10, 90, 2);
        sleepy1 = 13;
        tft.drawRect(sleepx1, sleepy1, sleepx2, sleepy2, TFT_WHITE);
        sleep_page -= 1;
        delay(300);
      }

      else if(digitalRead(4) == 0 && sleepy1 == 13 && sleep_page == 0)
      {
        frame1 = 0;
        tft.pushImage(0, 0, 128, 128, currentSleepFrame(frame1));
        delay(300);
        for(int i = 0; i < 6; ++i)
        {
          tft.pushImage(0, 0, 128, 128, currentSleepFrame(frame1));
          frame1++;
          delay(300);
        }
        for(int i = 0; i < 150; ++i)
        {
          tft.fillScreen(TFT_BLACK);
          delay(500);
          tft.drawString("Z", 60, 60, 4);
          delay(500);
          tft.drawString("z", 75, 45, 2);
          delay(500);
          tft.drawString("z", 83, 37, 2);
          delay(500);
          sleeptime += 1;
        }
        Fat_mas += 2;
      }

      else if(digitalRead(4) == 0 && sleepy1 == 38 && sleep_page == 0)
      {
        frame1 = 0;
        tft.pushImage(0, 0, 128, 128, currentSleepFrame(frame1));
        delay(300);
        for(int i = 0; i < 6; ++i)
        {
          tft.pushImage(0, 0, 128, 128, currentSleepFrame(frame1));
          frame1++;
          delay(300);
        }
        for(int i = 0; i < 600; ++i)
        {
          tft.fillScreen(TFT_BLACK);
          delay(500);
          tft.drawString("Z", 60, 60, 4);
          delay(500);
          tft.drawString("z", 75, 45, 2);
          delay(500);
          tft.drawString("z", 83, 37, 2);
          delay(500);
          sleeptime += 1;
        }
        Fat_mas += 10;
      }

      else if(digitalRead(4) == 0 && sleepy1 == 63 && sleep_page == 0)
      {
        frame1 = 0;
        tft.pushImage(0, 0, 128, 128, currentSleepFrame(frame1));
        delay(300);
        for(int i = 0; i < 6; ++i)
        {
          tft.pushImage(0, 0, 128, 128, currentSleepFrame(frame1));
          frame1++;
          delay(300);
        }
        for(int i = 0; i < 1800; ++i)
        {
          tft.fillScreen(TFT_BLACK);
          delay(500);
          tft.drawString("Z", 60, 60, 4);
          delay(500);
          tft.drawString("z", 75, 45, 2);
          delay(500);
          tft.drawString("z", 83, 37, 2);
          delay(500);
          sleeptime += 1;
        }
        Fat_mas += 20;
      }

      else if(digitalRead(4) == 0 && sleepy1 == 88 && sleep_page == 0)
      {
        frame1 = 0;
        tft.pushImage(0, 0, 128, 128, currentSleepFrame(frame1));
        delay(300);
        for(int i = 0; i < 6; ++i)
        {
          tft.pushImage(0, 0, 128, 128, currentSleepFrame(frame1));
          frame1++;
          delay(300);
        }
        for(int i = 0; i < 3; ++i)//////////////////////////////////这里改回14400
        {
          tft.fillScreen(TFT_BLACK);
          delay(500);
          tft.drawString("Z", 60, 60, 4);
          delay(500);
          tft.drawString("z", 75, 45, 2);
          delay(500);
          tft.drawString("z", 83, 37, 2);
          delay(500);
          sleeptime += 1;
        }
        Fat_mas += 50;
      }

      else if(digitalRead(4) == 0 && sleepy1 == 13 && sleep_page == 1)
      {
        sleeptime = 600;//假装睡觉，实则退出
        delay(300);
      }
    }
    sleeptime = 0;
    currentRoomScene = ROOM_GRAY;
    roomwhitex = 58;
    roomwhitey = 117;
    drawCurrentRoom();
    delay(300);
  }
}

void Roomshower()
{
  if(digitalRead(4) == 0)
  {
    delay(300);
    if(Cle <= 90)
    {
      for(int i = 0; i < 3; ++i)
      {
        tft.fillScreen(TFT_BLACK);
        delay(500);
        tft.drawString(".", 50, 60, 2);
        delay(500);
        tft.drawString(".", 60, 60, 2);
        delay(500);
        tft.drawString(".", 70, 60, 2);
        delay(500);
      }
      Cle_mas += 50;
      addMood(10);
      addLunacy(2);
      showLunacyReward(2);
    }
    else
    {
      tft.fillScreen(TFT_BLACK);
      tft.drawString("No need to shower", 7, 50, 2);
      delay(1000);
    }
    currentRoomScene = ROOM_BLUE;
    roomwhitex = 107;
    roomwhitey = 27;
    drawCurrentRoom();
    delay(300);
  }
}

void Shower()
{
  if(digitalRead(4) == 0)
  {
    tft.fillScreen(TFT_BLACK);
    delay(300);
    if(Cle <= 90)
    {
      for(int i = 0; i < 3; ++i)
      {
        tft.fillScreen(TFT_BLACK);
        delay(500);
        tft.drawString(".", 50, 60, 2);
        delay(500);
        tft.drawString(".", 60, 60, 2);
        delay(500);
        tft.drawString(".", 70, 60, 2);
        delay(500);
      }
      Cle_mas += 50;
      addMood(10);
      addLunacy(2);
      showLunacyReward(2);
    }
    else
    {
      tft.fillScreen(TFT_BLACK);
      tft.drawString("No need to shower", 7, 50, 2);
      delay(1000);
    }
    mainicon();
  }
}

void Shop()
{
  if (digitalRead(4) != LOW) return;
  waitForButtonsReleased();
  uint8_t selection = 0;
  while (true) {
    tft.fillScreen(TFT_BLACK);
    drawCjkText("抽卡模拟", 36, 2, TFT_WHITE);
    drawCjkText("单抽 1 狂气", 8, 27, TFT_WHITE);
    drawCjkText("十连 9 狂气", 8, 49, TFT_WHITE);
    drawCjkText("退出", 8, 71, TFT_WHITE);
    tft.drawRect(4, 26 + selection * 22, 118, 20, TFT_WHITE);
    tft.drawString("Lunacy: " + String(Exp), 8, 96, 2);
    drawCjkText("保底", 8, 115, TFT_WHITE);
    tft.drawString(String(gachaPity) + "/60", 44, 115, 1);
    while (digitalRead(4) == HIGH && digitalRead(5) == HIGH) delay(10);
    if (digitalRead(5) == LOW) {
      selection = (selection + 1) % 3;
      waitForButtonsReleased();
      continue;
    }
    waitForButtonsReleased();
    if (selection == 0) performGacha(1);
    else if (selection == 1) performGacha(10);
    else {
      mainicon();
      return;
    }
  }
}

void Box()
{
  if (digitalRead(4) != LOW) return;
  waitForButtonsReleased();
  uint16_t page = 0;
  const uint16_t pageCount = (GACHA_TOTAL_CARDS + 1) / 2;
  while (true) {
    tft.fillScreen(TFT_BLACK);
    drawCjkText("卡册", 50, 2, TFT_WHITE);
    String summary = "";
    summary += "已获得 " + String(gachaSeenCount) + "/" + String(GACHA_TOTAL_CARDS);
    drawCjkText(summary.c_str(), 3, 18, TFT_WHITE);
    String remaining = "剩余 " + String(GACHA_TOTAL_CARDS - gachaSeenCount);
    drawCjkText(remaining.c_str(), 3, 32, TFT_WHITE);
    tft.drawString("Total: " + String(gachaTotal), 3, 46, 1);
    for (uint8_t row = 0; row < 2; ++row) {
      const uint16_t index = page * 2 + row;
      if (index >= GACHA_TOTAL_CARDS) break;
      uint8_t rarity = 1;
      uint8_t poolIndex = index;
      if (index >= gacha1StarCount + gacha2StarCount) {
        rarity = 3;
        poolIndex = index - gacha1StarCount - gacha2StarCount;
      } else if (index >= gacha1StarCount) {
        rarity = 2;
        poolIndex = index - gacha1StarCount;
      }
      const uint16_t color = rarity == 3 ? 0xFEA0 : (rarity == 2 ? 0xC100 : 0xB208);
      const bool owned = gachaSeen[index / 8] & (1 << (index % 8));
      String marker = owned ? "[√] " : "[ ] ";
      tft.setTextColor(color, TFT_BLACK);
      tft.drawString(marker + String(rarity) + "*", 2, 62 + row * 27, 1);
      if (owned) drawCjkText(gachaName(rarity, poolIndex), 34, 60 + row * 27, color, 127, 2);
      else drawCjkText("未获得", 34, 60 + row * 27, TFT_DARKGREY);
    }
    drawCjkText("B翻页 A返回", 29, 119, TFT_WHITE);
    while (digitalRead(4) == HIGH && digitalRead(5) == HIGH) delay(10);
    if (digitalRead(4) == LOW) {
      waitForButtonsReleased();
      break;
    }
    page = (page + 1) % pageCount;
    waitForButtonsReleased();
  }
  mainicon();
}

const uint16_t* coinFace(uint8_t material, bool tails)
{
  if (material == 1) return tails ? coin_rtails : coin_rheads;
  if (material == 2) return tails ? coin_gtails : coin_gheads;
  return tails ? coin_tails : coin_heads;
}

void drawCoinAt(int centerX, int topY, int size, float scaleX)
{
  const int drawWidth = max(1, static_cast<int>(size * fabsf(scaleX)));
  if (drawWidth <= 1) return;
  const int left = centerX - drawWidth / 2;
  const bool tails = scaleX < 0.0f;
  const uint16_t* source = coinFace(coinMaterial, tails);
  for (int y = 0; y < size; ++y) {
    const int srcY = y * COIN_ASSET_SIZE / size;
    for (int x = 0; x < drawWidth; ++x) {
      int srcX = x * (COIN_ASSET_SIZE - 1) / max(1, drawWidth - 1);
      if (tails) srcX = (COIN_ASSET_SIZE - 1) - srcX;
      const uint16_t color = pgm_read_word(&source[srcY * COIN_ASSET_SIZE + srcX]);
      if (color != 0x0000) tft.drawPixel(left + x, topY + y, color);
    }
  }
}

bool hasGachaCardByName(const char* wanted)
{
  for (size_t i = 0; i < gacha1StarCount; ++i)
    if (strcmp(gacha1Star[i], wanted) == 0 && (gachaSeen[i / 8] & (1 << (i % 8)))) return true;
  for (size_t i = 0; i < gacha2StarCount; ++i) {
    const uint16_t index = gacha1StarCount + i;
    if (strcmp(gacha2Star[i], wanted) == 0 && (gachaSeen[index / 8] & (1 << (index % 8)))) return true;
  }
  for (size_t i = 0; i < gacha3StarCount; ++i) {
    const uint16_t index = gacha1StarCount + gacha2StarCount + i;
    if (strcmp(gacha3Star[i], wanted) == 0 && (gachaSeen[index / 8] & (1 << (index % 8)))) return true;
  }
  return false;
}

void runCoinSkill(uint8_t coinCount, const char* skillName)
{
  const uint8_t count = min<uint8_t>(coinCount, 9);
  const int size = count == 1 ? 52 : (count <= 3 ? 34 : (count <= 6 ? 24 : 18));
  const int topY = count == 1 ? 28 : 38;
  bool tails[9] = {};

  while (true) {
    for (uint8_t i = 0; i < count; ++i) {
      tails[i] = random(100) >= constrain(Mood, 0, 100);
    }
    for (int frame = 0; frame < 28; ++frame) {
      tft.fillScreen(TFT_BLACK);
      drawCjkText(skillName, 8, 3, TFT_WHITE, 120, 1);
      for (uint8_t i = 0; i < count; ++i) {
        const int centerX = count == 1 ? 64 : 10 + (100 * i) / (count - 1);
        drawCoinAt(centerX, topY, size, cosf(frame * 0.55f + i * 0.35f));
      }
      delay(35);
    }

    tft.fillScreen(TFT_BLACK);
    drawCjkText(skillName, 8, 3, TFT_WHITE, 120, 1);
    uint8_t heads = 0;
    for (uint8_t i = 0; i < count; ++i) {
      const int centerX = count == 1 ? 64 : 10 + (100 * i) / (count - 1);
      drawCoinAt(centerX, topY, size, tails[i] ? -1.0f : 1.0f);
      if (!tails[i]) ++heads;
    }
    String result = "正面 " + String(heads) + "/" + String(count);
    drawCjkText(result.c_str(), 38, 91, TFT_WHITE);
    drawCjkText("A再投  B选技能", 22, 113, TFT_WHITE);

    while (digitalRead(4) == HIGH && digitalRead(5) == HIGH) delay(10);
    const bool reroll = digitalRead(4) == LOW;
    waitForButtonsReleased();
    if (!reroll) return;
  }
}

void CoinFlipGame()
{
  // The computer-room confirm key may still be held when this screen opens.
  // Wait for release so entering the computer does not auto-select skill 1.
  waitForButtonsReleased();
  const char* skillNames[6] = {"握住我的手吧", "赤红之钉的洗礼", nullptr, nullptr, nullptr, "返回"};
  uint8_t skillCoins[6] = {1, 3, 0, 0, 0, 0};
  uint8_t skillCount = 2;
  if (hasGachaCardByName("李箱蜘蛛巢食指父辈")) { skillNames[skillCount] = "Furioso-Replica"; skillCoins[skillCount++] = 9; }
  if (hasGachaCardByName("默尔索拇指东部指挥官IIII")) { skillNames[skillCount] = "超绝猛虎杀击乱斩"; skillCoins[skillCount++] = 6; }
  if (hasGachaCardByName("希斯克利夫脑叶公司E.G.O:狐雨")) { skillNames[skillCount] = "踏入泥泞"; skillCoins[skillCount++] = 4; }
  // Return must follow the actual visible skill list; never draw a null slot.
  skillNames[skillCount] = "返回";
  uint8_t selection = 0;

  while (true) {
    tft.fillScreen(TFT_BLACK);
    drawCjkText("选择技能", 42, 5, TFT_WHITE);
    for (uint8_t i = 0; i <= skillCount; ++i)
      drawCjkText(skillNames[i], 8, 24 + i * 15, TFT_WHITE, 124, 1);
    tft.drawRect(4, 21 + selection * 15, 120, 15, TFT_WHITE);
    drawCjkText("心情命中率", 4, 111, TFT_WHITE);
    tft.drawString(String(Mood) + "%", 90, 111, 1);

    while (digitalRead(4) == HIGH && digitalRead(5) == HIGH) delay(10);
    if (digitalRead(5) == LOW) {
      selection = (selection + 1) % (skillCount + 1);
      waitForButtonsReleased();
      continue;
    }
    waitForButtonsReleased();
    if (selection == skillCount) break;
    runCoinSkill(skillCoins[selection], skillNames[selection]);
  }
  tft.fillScreen(TFT_BLACK);
  currentRoomScene = ROOM_GRAY;
  roomwhitex = 45;
  roomwhitey = 16;
  drawCurrentRoom();
}

void peace()
{
  if(Hun < 0 || Fat < 0 || Cle < 0)
  {
    tft.fillScreen(TFT_BLACK);
    tft.drawString("GameOver", 3, 30, 4);
    delay(800);
    tft.drawString("Experience", 33, 65, 2);
    delay(800);
    tft.drawString(String(Exp), 33, 85, 2);
    while(1);
  }
}

void Rooms()
{
  if(digitalRead(4) == 0)
  {
    delay(300);
    tft.fillScreen(TFT_BLACK);
    roomwhitex = 83, roomwhitey = 16;
    currentRoomScene = ROOM_WHITE;
    drawCurrentRoom();//上左
    while(!(roomwhitex == 95 && roomwhitey == 105 && digitalRead(4) == 0))
    {
      if(digitalRead(5) == 0 && roomwhitex == 83)
      {
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        roomwhitex =107 , roomwhitey = 28;
        drawCurrentRoom();
        delay(300);
      }
      else if(digitalRead(5) == 0 && roomwhitex == 107)
      {
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        roomwhitex =95 , roomwhitey = 105;
        drawCurrentRoom();
        delay(300);
      }
      else if(digitalRead(5) == 0 && roomwhitex == 95)
      {
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        roomwhitex =45 , roomwhitey = 110;
        drawCurrentRoom();
        delay(300);
      }
      else if(digitalRead(5) == 0 && roomwhitex == 45)
      {
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        roomwhitex =21 , roomwhitey = 98;
        drawCurrentRoom();
        delay(300);
      }
      else if(digitalRead(5) == 0 && roomwhitex == 21)
      {
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        roomwhitex =83 , roomwhitey = 16;
        drawCurrentRoom();
        delay(300);
      }
  //    tft.drawCircle(107, 28, 2, TFT_WHITE);//上右
  //    tft.drawCircle(21, 99, 2, TFT_WHITE);//下左
  //    tft.drawCircle(45, 111, 2, TFT_WHITE);//下中
  //    tft.drawCircle(95, 105, 2, TFT_WHITE);//下右
      //以下为按下确认（0）时的反应
      else if(digitalRead(4) == 0 && roomwhitex == 83)
      {
        currentRoomScene = ROOM_GRAY;
        //tft.drawCircle(45, 16, 2, TFT_WHITE);
        roomwhitex = 58;
        roomwhitey = 117;
        drawCurrentRoom();
        delay(300);
        while(!(digitalRead(4) == 0 && roomwhitex == 58 && roomwhitey == 117))
        {
          drawCurrentRoom();
          if(digitalRead(5) == 0 && roomwhitex == 58 && roomwhitey == 117)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 45;
            roomwhitey = 16;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(5) == 0 && roomwhitex == 45 && roomwhitey == 16)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 100;
            roomwhitey = 102;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(5) == 0 && roomwhitex == 100 && roomwhitey == 102)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 58;
            roomwhitey = 117;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(4) == 0 && roomwhitex == 100 && roomwhitey == 102)
          {
            Roomsleep();
          }
          
          else if(digitalRead(4) == 0 && roomwhitex == 45 && roomwhitey == 16)//////////////////这里（游戏）
          {
            CoinFlipGame();
            continue;
            //以下开始写电脑游戏
              tft.fillScreen(TFT_BLACK);
              tft.drawString("Sic bo", 10, 25, 2);
              //tft.drawString("WiFi scan", 10, 50, 2);//这个功能太占地方还发热，暂时不用
              tft.drawString("Exit", 10, 75, 2);
              gamex1 = 5, gamey1 = 23, gamex2 = 76, gamey2 = 21;
              tft.drawRect(gamex1, gamey1, gamex2, gamey2, TFT_WHITE);

              while(!(gamey1 == 73 && digitalRead(4) == 0))
              {
                tft.drawRect(gamex1, gamey1, gamex2, gamey2, TFT_WHITE);
                if(gamey1 == 23 && digitalRead(5) == 0)
                {
                  tft.drawRect(gamex1, gamey1, gamex2, gamey2, TFT_BLACK);
                  gamey1 = 73;
                  tft.drawRect(gamex1, gamey1, gamex2, gamey2, TFT_WHITE);
                  delay(300);
                }
            
//                else if(gamey1 == 48 && digitalRead(5) == 0)
//                {
//                  tft.drawRect(gamex1, gamey1, gamex2, gamey2, TFT_BLACK);
//                  gamey1 += 25;
//                  tft.drawRect(gamex1, gamey1, gamex2, gamey2, TFT_WHITE);
//                  delay(300);
//                }
            
                else if(gamey1 == 73 && digitalRead(5) == 0)
                {
                  tft.drawRect(gamex1, gamey1, gamex2, gamey2, TFT_BLACK);
                  gamey1 = 23;
                  tft.drawRect(gamex1, gamey1, gamex2, gamey2, TFT_WHITE);
                  delay(300);
                }

                //以下为确认游戏
                else if(gamey1 == 23 && digitalRead(4) == 0)//第一个游戏sic_bo
                {
                  tft.fillScreen(TFT_BLACK);
                  tft.drawString("Sic bo", 30, 30, 4);
                  int gamex = 9;
                  delay(1500);
                  while(!(gamex == 83 && digitalRead(4) == 0))
                  {
                    tft.fillScreen(TFT_BLACK); 
                    gamenum_left = random(1, 7);
                    tft.drawString(String(gamenum_left), 33, 30, 4);
                    delay(400);
                    gamenum_left = random(1, 7);
                    tft.drawString(String(gamenum_left), 33, 30, 4);
                    delay(400);
                    gamenum_left = random(1, 7);
                    tft.drawString(String(gamenum_left), 33, 30, 4);
                    delay(400);
                    tft.drawString("?", 33, 30, 4);//左面数字结束
                    delay(700);
      
                    gamenum_right = random(1, 7);
                    tft.drawString(String(gamenum_right), 83, 30, 4);
                    delay(400);
                    gamenum_right = random(1, 7);
                    tft.drawString(String(gamenum_right), 83, 30, 4);
                    delay(400);
                    gamenum_right = random(1, 7);
                    tft.drawString(String(gamenum_right), 83, 30, 4);
                    delay(400);
                    tft.drawString("?", 83, 30, 4);//右面数字结束
                    delay(400);
      
                    gamenum_left = random(1, 7);
                    gamenum_right = random(1, 7);
      
                    while(digitalRead(4) != 0)
                    {
                      tft.drawString("Big", 18, 90, 2);
                      tft.drawString("Small", 50, 90, 2);
                      tft.drawString("Exit", 90, 90, 2);
                      tft.drawRect(gamex, 89, 37, 20, TFT_WHITE);
                      if(gamex == 9 && digitalRead(5) == 0)
                      {
                        tft.drawRect(gamex, 89, 37, 20, TFT_BLACK);
                        gamex += 37;
                        tft.drawRect(gamex, 89, 37, 20, TFT_WHITE);
                        delay(300);
                      }
        
                      else if(gamex == 46 && digitalRead(5) == 0)
                      {
                        tft.drawRect(gamex, 89, 37, 20, TFT_BLACK);
                        gamex += 37;
                        tft.drawRect(gamex, 89, 37, 20, TFT_WHITE);
                        delay(300);
                      }
      
                      else if(gamex == 83 && digitalRead(5) == 0)
                      {
                        tft.drawRect(gamex, 89, 37, 20, TFT_BLACK);
                        gamex = 9;
                        tft.drawRect(gamex, 89, 37, 20, TFT_WHITE);
                        delay(300);
                      }
                    }
      
                    if(gamex == 9 && digitalRead(4) == 0)
                    {
                      tft.drawString(String(gamenum_left), 33, 30, 4);
                      tft.drawString(String(gamenum_right), 83, 30, 4);
                      delay(1200);
                      if(gamenum_left + gamenum_right >= 7)
                      {
                        tft.fillScreen(TFT_BLACK);
                        tft.drawString("Win", 40, 30, 4);
                        Exp_mas -= 20;
                        delay(500);
                        drawLunacyGain(2);
                        delay(1000);
                        tft.fillScreen(TFT_BLACK);
                      }
                      else
                      {
                        tft.fillScreen(TFT_BLACK);
                        tft.drawString("Lose", 35, 30, 4);
                        Exp_mas += 20;
                        delay(500);
                        drawLunacyGain(-2);
                        delay(1000);
                        tft.fillScreen(TFT_BLACK);
                      }
                    }
      
                    else if(gamex == 46 && digitalRead(4) == 0)
                    {
                      tft.drawString(String(gamenum_left), 33, 30, 4);
                      tft.drawString(String(gamenum_right), 83, 30, 4);
                      delay(1200);
                      if(gamenum_left + gamenum_right <= 6)
                      {
                        tft.fillScreen(TFT_BLACK);
                        tft.drawString("Win", 40, 30, 4);
                        Exp_mas -= 20;
                        delay(500);
                        drawLunacyGain(2);
                        delay(1000);
                        tft.fillScreen(TFT_BLACK);
                      }
                      else
                      {
                        tft.fillScreen(TFT_BLACK);
                        tft.drawString("Lose", 35, 30, 4);
                        Exp_mas += 20;
                        delay(500);
                        drawLunacyGain(-2);
                        delay(1000);
                        tft.fillScreen(TFT_BLACK);
                      }
                    }
                  }
                  delay(300);
                  tft.fillScreen(TFT_BLACK);
                  tft.drawString("Sic bo", 10, 25, 2);
                  //tft.drawString("WiFi scan", 10, 50, 2);
                  tft.drawString("Exit", 10, 75, 2);
                }

//                else if(gamey1 == 48 && digitalRead(4) == 0)//第二个游戏wifi_scan
//                {
//                  delay(300);
//                  int wifitime = 0;
//                  while(!(digitalRead(4) == 0 || digitalRead(5) == 0))//这个括号可以删掉
//                  {
//                    if(wifitime == 0)//让wifi_scan只执行一次
//                    {
//                      wifi_scan();
//                      ++wifitime;
//                    }
//                  }
//                  delay(300);
//                  tft.fillScreen(TFT_BLACK);
//                  tft.drawString("Sic bo", 10, 25, 2);
//                  tft.drawString("WiFi scan", 10, 50, 2);
//                  tft.drawString("Exit", 10, 75, 2);
//                }
              }
            currentRoomScene = ROOM_GRAY;
            delay(300);
          }
        }
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        currentRoomScene = ROOM_WHITE;
        roomwhitex = 83;
        roomwhitey = 16;
        drawCurrentRoom();
        delay(300);
      }
      else if(digitalRead(4) == 0 && roomwhitex == 107)
      {

//          roomwhitex = 96;
//          roomwhitey = 23;
//          roomwhitex = 9;
//          roomwhitey = 93;
        currentRoomScene = ROOM_RED;
        roomwhitex = 9;
        roomwhitey = 93;
        drawCurrentRoom();
        delay(300);
        while(!(digitalRead(4) == 0 && roomwhitex == 9 && roomwhitey == 93))
        {
          if(digitalRead(5) == 0 && roomwhitex == 9 && roomwhitey == 93)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 96;
            roomwhitey = 23;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(5) == 0 && roomwhitex == 96 && roomwhitey == 23)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 9;
            roomwhitey = 93;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(4) == 0 && roomwhitex == 96 && roomwhitey == 23)
          {
            Roomfood();
          }
        }
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        currentRoomScene = ROOM_WHITE;
        roomwhitex = 107;
        roomwhitey = 28;
        drawCurrentRoom();
        delay(300);     
      }

      else if(digitalRead(4) == 0 && roomwhitex == 45)
      {
//          roomwhitex = 119;
//          roomwhitey = 34;
//          roomwhitex = 49;
//          roomwhitey = 114;
        currentRoomScene = ROOM_GREEN;
        roomwhitex = 119;
        roomwhitey = 34;
        drawCurrentRoom();
        delay(300);
        while(!(digitalRead(4) == 0 && roomwhitex == 119 && roomwhitey == 34))
        {
          if(digitalRead(5) == 0 && roomwhitex == 119 && roomwhitey == 34)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 49;
            roomwhitey = 114;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(5) == 0 && roomwhitex == 49 && roomwhitey == 114)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 119;
            roomwhitey = 34;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(4) == 0 && roomwhitex == 49 && roomwhitey == 114)
          {
            delay(300);
//            frame1 = 0;
//            for(int i = 0; i < 19; ++i)
//            {
//              tft.pushImage(0, 12, 128, 104, gardengif[frame1]);
//              frame1++;
//              delay(100);
//            }
            while(digitalRead(4) != 0)
            {
              tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
              tft.pushImage(0, 12, 128, 104, roomblack);
            }
            currentRoomScene = ROOM_GREEN;
            roomwhitex = 49;
            roomwhitey = 114;
            drawCurrentRoom();
            delay(300);
          }
        }
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        currentRoomScene = ROOM_WHITE;
        roomwhitex = 45;
        roomwhitey = 111;
        drawCurrentRoom();
        delay(300);
      }
      
      else if(digitalRead(4) == 0 && roomwhitex == 21)
      {
//          roomwhitex = 20;
//          roomwhitey = 27;
//          roomwhitex = 107;
//          roomwhitey = 27;
        currentRoomScene = ROOM_BLUE;
        roomwhitex = 107;
        roomwhitey = 27;
        drawCurrentRoom();
        delay(300);
        while(!(digitalRead(4) == 0 && roomwhitex == 107 && roomwhitey == 27))
        {
          if(digitalRead(5) == 0 && roomwhitex == 107 && roomwhitey == 27)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 20;
            roomwhitey = 27;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(5) == 0 && roomwhitex == 20 && roomwhitey == 27)
          {
            tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
            roomwhitex = 107;
            roomwhitey = 27;
            drawCurrentRoom();
            delay(300);
          }
          else if(digitalRead(4) == 0 && roomwhitex == 20 && roomwhitey == 27)
          {
            Roomshower();
          }
        }
        tft.drawCircle(roomwhitex, roomwhitey, 2, TFT_BLACK);
        currentRoomScene = ROOM_WHITE;
        roomwhitex = 21;
        roomwhitey = 99;
        drawCurrentRoom();
        delay(300);  
      }
    }
    mainicon();
    delay(300);
  }
}

void Time()
{
  if (digitalRead(4) != 0) return;

  delay(300);
  int selection = 0;
  const int itemY[] = {8, 30, 52, 74, 96};
  static const char* const coinColors[] = {"金色", "红色", "绿色"};

  while (true)
  {
    tft.fillScreen(TFT_BLACK);
    drawCjkText("昼夜模式", 10, itemY[0], TFT_WHITE);
    drawCjkText(!clothingUnlocked ? "服装：未解锁" : (capeEnabled ? "服装：2" : "服装：1"), 10, itemY[1], TFT_WHITE);
    String coinLabel = "硬币颜色：" + String(coinColors[coinMaterial]);
    drawCjkText(coinLabel.c_str(), 10, itemY[2], TFT_WHITE);
    drawCjkText("背光设置", 10, itemY[3], TFT_WHITE);
    drawCjkText("返回", 10, itemY[4], TFT_WHITE);
    tft.drawRect(5, itemY[selection] - 1, 118, 19, TFT_WHITE);

    while (digitalRead(4) != 0 && digitalRead(5) != 0)
    {
      delay(10);
    }

    if (digitalRead(5) == 0)
    {
      selection = (selection + 1) % 5;
      waitForButtonsReleased();
      continue;
    }

    waitForButtonsReleased();

    if (selection == 0)
    {
      int modeSelection = darkmode ? 1 : 0;
      while (true)
      {
        tft.fillScreen(TFT_BLACK);
        drawCjkText("白天", 10, 25, TFT_WHITE);
        drawCjkText("夜晚", 10, 50, TFT_WHITE);
        tft.drawRect(5, modeSelection == 0 ? 23 : 48, 76, 19, TFT_WHITE);

        while (digitalRead(4) != 0 && digitalRead(5) != 0)
        {
          delay(10);
        }
        if (digitalRead(5) == 0)
        {
          modeSelection = 1 - modeSelection;
          waitForButtonsReleased();
          continue;
        }

        darkmode = modeSelection == 1;
        frame1 = 0;
        lastMainFrameMs = 0;
        waitForButtonsReleased();
        break;
      }
    }
    else if (selection == 1)
    {
      if (!clothingUnlocked) {
        tft.fillScreen(TFT_BLACK);
        drawCjkText("服装尚未解锁", 20, 50, TFT_WHITE);
        delay(1000);
        continue;
      }
      capeEnabled = !capeEnabled;
      frame1 = 0;
      lastMainFrameMs = 0;
    }
    else if (selection == 2)
    {
      uint8_t colorSelection = coinMaterial;
      while (true)
      {
        tft.fillScreen(TFT_BLACK);
        drawCjkText("选择硬币颜色", 28, 12, TFT_WHITE);
        for (uint8_t i = 0; i < 3; ++i)
          drawCjkText(coinColors[i], 42, 40 + i * 23, i == colorSelection ? TFT_YELLOW : TFT_WHITE);
        tft.drawRect(32, 37 + colorSelection * 23, 64, 19, TFT_WHITE);
        while (digitalRead(4) != 0 && digitalRead(5) != 0) delay(10);
        if (digitalRead(5) == 0) {
          colorSelection = (colorSelection + 1) % 3;
          waitForButtonsReleased();
          continue;
        }
        coinMaterial = colorSelection;
        savePersistentState();
        waitForButtonsReleased();
        break;
      }
    }
    else if (selection == 3)
    {
      tft.fillScreen(TFT_BLACK);
      drawCjkText("背光固定开启", 8, 50, TFT_WHITE);
      delay(1200);
    }
    else
    {
      delay(300);
      mainicon();
      return;
    }
  }
}

void mainselect()
{
  tft.drawRect(mainx1, mainy1, mainx2, mainy2, TFT_WHITE); 
  buttonstate47 = digitalRead(5);
  if(buttonstate47 == LOW && buttonbefore47 == HIGH)
  {
    tft.drawRect(mainx1, mainy1, mainx2, mainy2, TFT_BLACK); 
    mainx1 += n_main;
    mainnum += 1;
    if(mainx1 == 129 && mainy1 == 0)
    {
      mainx1 = 1; //mainx1有1，33，65，97四种。
      mainy1 = 98;
    }

    else if(mainx1 == 129 && mainy1 == 98)
    {
      mainx1 = 1;
      mainy1 = 0;
      mainnum = 0;
    }
  }
  buttonbefore47 = buttonstate47;
  tft.drawRect(mainx1, mainy1, mainx2, mainy2, TFT_WHITE);

  switch(mainnum)
  {
    case 0: 
    {
      Generalmenu(); 
    }
    break;

    case 1:
    {
      Food();
    }
    break;

    case 2:
    {
      Sleep();
    }
    break;

    case 3:
    {
      Shower();
    }
    break;

    case 4:
    {
      Shop();
    }
    break;

    case 5:
    {
      Rooms();
    }
    break;

    case 6:
    {
      Box();
    }
    break;

    case 7:
    {
      Time();
    }
    break;
  }
}


void setup() 
{
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setViewport(56, 96, 128, 128, true);
  tft.setSwapBytes(true);
  randomSeed(micros());
  loadPersistentState();

  //tft.drawRect(0, 0, 128, 128, TFT_WHITE); 

  playBootAnimation();
  mainicon();

}


void loop()
{  
  mainselect();

  captureActionRewards();

  if(Hun < 60 || Fat < 60 || Cle < 60)
  {
    tft.pushImage(2, 1, 28, 28, warn);
  }
  else
  {
    tft.pushImage(2, 1, 28, 28, pest);
  }

  state_count();
  updatePeriodicLunacy();
  updateMoodAndBadges();
  
  maingif();

  peace();
  
}
