#define _USE_MATH_DEFINES
#define _GNU_SOURCE
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define MAX_STEPS_PER_FRAME 10000
#define SPRITE_EDIT_WIDTH 180
#define SPRITE_EDIT_HEIGHT 120
SDL_Window* gWindow = NULL;
struct Application* gApp = nullptr;
//Value System
struct Value {
    enum Type { VAL_NUMBER, VAL_STRING } type;
    float num;
    string str;

    Value() : type(VAL_NUMBER), num(0) {}
};

Value make_number(float f) {
    Value v;
    v.type = Value::VAL_NUMBER;
    v.num = f;
    return v;
}

Value make_string(const string& s) {
    Value v;
    v.type = Value::VAL_STRING;
    v.str = s;
    return v;
}

float value_to_number(const Value& v) {
    if (v.type == Value::VAL_NUMBER) return v.num;
    return (float)atof(v.str.c_str());
}

string value_to_string(const Value& v) {
    if (v.type == Value::VAL_STRING) return v.str;
    return to_string(v.num);
}

// Forward declarations
struct Application;
struct Project;
struct ExecutionEngine;
struct SpriteManagerUI;
struct BackdropManagerUI;
struct SoundManagerUI;
struct PenToolUI;
struct BlockPaletteUI;
struct CodeAreaUI;
struct Sprite;
struct Backdrop;
struct Sound;
struct Variable;
struct ExecutionContext;
struct Block;
struct Script;
struct Costume;

struct LoopInfo {
    int start;
    int end;
    int count;
};

struct IfInfo {
    int elseStart;
    int endifPos;
    bool trueBranch;
};

enum BlockType {
    BLOCK_MOVE,
    BLOCK_TURN,
    BLOCK_GOTO,
    BLOCK_CHANGE_X,
    BLOCK_CHANGE_Y,
    BLOCK_SET_DIRECTION,
    BLOCK_SAY,
    BLOCK_THINK,
    BLOCK_SWITCH_COSTUME,
    BLOCK_NEXT_COSTUME,
    BLOCK_SWITCH_BACKDROP,
    BLOCK_NEXT_BACKDROP,
    BLOCK_CHANGE_SIZE,
    BLOCK_SET_SIZE,
    BLOCK_CHANGE_COLOR,
    BLOCK_SET_COLOR,
    BLOCK_CLEAR_EFFECTS,
    BLOCK_SHOW,
    BLOCK_HIDE,
    BLOCK_GO_TO_LAYER,
    BLOCK_CHANGE_LAYER,
    BLOCK_PLAY_SOUND,
    BLOCK_PLAY_SOUND_UNTIL_DONE,
    BLOCK_STOP_ALL_SOUNDS,
    BLOCK_CHANGE_VOLUME,
    BLOCK_SET_VOLUME,
    BLOCK_WHEN_FLAG_CLICKED,
    BLOCK_WHEN_KEY_PRESSED,
    BLOCK_WHEN_SPRITE_CLICKED,
    BLOCK_BROADCAST,
    BLOCK_BROADCAST_AND_WAIT,
    BLOCK_WHEN_I_RECEIVE,
    BLOCK_WAIT,
    BLOCK_REPEAT,
    BLOCK_FOREVER,
    BLOCK_IF,
    BLOCK_IF_ELSE,
    BLOCK_WAIT_UNTIL,
    BLOCK_REPEAT_UNTIL,
    BLOCK_STOP_ALL,
    BLOCK_CUSTOM_CALL,
    BLOCK_CUSTOM_DEFINE,
    BLOCK_ADD,
    BLOCK_SUBTRACT,
    BLOCK_MULTIPLY,
    BLOCK_DIVIDE,
    BLOCK_RANDOM,
    BLOCK_LT,
    BLOCK_GT,
    BLOCK_EQUALS,
    BLOCK_AND,
    BLOCK_OR,
    BLOCK_NOT,
    BLOCK_JOIN,
    BLOCK_LETTER_OF,
    BLOCK_LENGTH,
    BLOCK_MOD,
    BLOCK_ROUND,
    BLOCK_ABS,
    BLOCK_SQRT,
    BLOCK_SIN,
    BLOCK_COS,
    BLOCK_TAN,
    BLOCK_ASIN,
    BLOCK_ACOS,
    BLOCK_ATAN,
    BLOCK_LN,
    BLOCK_LOG,
    BLOCK_POW,
    BLOCK_SET_VARIABLE,
    BLOCK_CHANGE_VARIABLE,
    BLOCK_VARIABLE_GET,
    BLOCK_NUMBER,
    BLOCK_STRING,
    BLOCK_TOUCHING_EDGE,
    BLOCK_MOUSE_X,
    BLOCK_MOUSE_Y,
    BLOCK_KEY_PRESSED,
    BLOCK_PEN_DOWN,
    BLOCK_PEN_UP,
    BLOCK_SET_PEN_COLOR,
    BLOCK_CHANGE_PEN_COLOR,
    BLOCK_SET_PEN_BRIGHTNESS,
    BLOCK_CHANGE_PEN_BRIGHTNESS,
    BLOCK_SET_PEN_SATURATION,
    BLOCK_CHANGE_PEN_SATURATION,
    BLOCK_SET_PEN_SIZE,
    BLOCK_CHANGE_PEN_SIZE,
    BLOCK_ERASE_ALL,
    BLOCK_STAMP,
    BLOCK_CHANGE_BRIGHTNESS,
    BLOCK_SET_BRIGHTNESS,
    BLOCK_CHANGE_SATURATION,
    BLOCK_SET_SATURATION,
    BLOCK_COSTUME_NUMBER,
    BLOCK_COSTUME_NAME,
    BLOCK_BACKDROP_NUMBER,
    BLOCK_BACKDROP_NAME,
    BLOCK_SIZE,
    BLOCK_TOUCHING_MOUSEPOINTER,
    BLOCK_TOUCHING_SPRITE,
    BLOCK_TOUCHING_COLOR,
    BLOCK_COLOR_TOUCHING_COLOR,
    BLOCK_DISTANCE_TO,
    BLOCK_ASK_AND_WAIT,
    BLOCK_ANSWER,
    BLOCK_MOUSE_DOWN,
    BLOCK_SET_DRAG_MODE,
    BLOCK_TIMER,
    BLOCK_RESET_TIMER,
    BLOCK_GO_TO_RANDOM,
    BLOCK_GO_TO_MOUSE,
    BLOCK_IF_ON_EDGE_BOUNCE,
    BLOCK_ELSE,
    BLOCK_ENDIF,
    BLOCK_ENDLOOP
};
struct Block {
    BlockType type;
    float numParam1;
    float numParam2;
    int intParam;
    string strParam;
    vector<Block*> children;
    int bodyEnd;
    int elseStart;
};

struct Script {
    vector<Block*> blocks;
};

struct Costume {
    string name;
    SDL_Texture* texture;
};

struct Sprite {
    string name;
    float x;
    float y;
    float direction;
    float size;
    int visible;
    int currentCostume;
    int layer;
    bool draggable;
    vector<Costume*> costumes;
    vector<Script*> scripts;
    bool penDown;
    float penHue;
    float penSaturation;
    float penBrightness;
    int penSize;
    SDL_Texture* penCanvas;
    string sayText;
    string thinkText;
    Uint32 sayUntil;
    Uint32 thinkUntil;
    float colorEffect;
    float brightnessEffect;
    float saturationEffect;
};

struct Backdrop {
    string name;
    SDL_Texture* texture;
};

struct Sound {
    string name;
    float volume;
    int muted;
    Mix_Chunk* chunk;
};

struct Variable {
    string name;
    Value value;
};

struct Project {
    vector<Sprite*> sprites;
    vector<Backdrop*> backdrops;
    vector<Sound*> sounds;
    vector<Variable*> globalVariables;
    int currentBackdrop;
    string answer;
    Uint32 timerStart;
};

struct ExecutionContext {
    int spriteId;
    int scriptId;
    int pc;
    Uint32 waitUntil;
    vector<LoopInfo> loopStack;
    vector<IfInfo> ifStack;
    vector<int> callStack;
    int repeatCount;
    int ifElseBranch;
    int waitingForSoundChannel;
    bool waitingForAnswer;
    ExecutionContext* parent;
    int childrenLeft;
    bool waitingForChildren;
};

struct ExecutionEngine {
    Project* project;
    vector<ExecutionContext*> contexts;
    bool stepMode;
};

struct Application {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool paused;
    bool executing;
    Project* currentProject;
    ExecutionEngine* engine;
    SpriteManagerUI* spriteManagerUI;
    BackdropManagerUI* backdropManagerUI;
    SoundManagerUI* soundManagerUI;
    PenToolUI* penToolUI;
    BlockPaletteUI* blockPalette;
    CodeAreaUI* codeArea;
    TTF_Font* speechFont;
    SDL_Rect menuRect;
    SDL_Rect varPanelRect;
    SDL_Rect paletteRect;
    SDL_Rect codeRect;
    SDL_Rect sceneRect;
    SDL_Rect spritePanelRect;
    SDL_Rect penPanelRect;
    SDL_Rect backdropPanelRect;
    SDL_Rect soundPanelRect;
    int dragSpriteIndex;
    int dragStartX, dragStartY;
    float dragSpriteOrigX, dragSpriteOrigY;
    bool answerReady;
    char pendingAnswer[256];
    char textInputBuffer[256];
    char lastError[256];
    Uint32 errorTime;
};

struct SpriteManagerUI {
    SDL_Renderer* renderer;
    Project* project;
    int selectedSpriteIndex;
    int scrollOffset;
    SDL_Rect rect;
    TTF_Font* font;
    int editingName;
    string nameEditBuffer;
    int editingField;
    int editingSpriteIndex;
    string editBuffer;
    Uint32 lastClickTime;
    int lastClickedIndex;
};

struct BackdropManagerUI {
    SDL_Renderer* renderer;
    Project* project;
    int selectedBackdropIndex;
    int scrollOffset;
    SDL_Rect rect;
    TTF_Font* font;
};

struct SoundManagerUI {
    SDL_Renderer* renderer;
    Project* project;
    int selectedSoundIndex;
    int scrollOffset;
    SDL_Rect rect;
    TTF_Font* font;
};

struct PenToolUI {
    SDL_Renderer* renderer;
    Project* project;
    SDL_Rect rect;
    TTF_Font* font;
    bool active;
    bool drawing;
    int lastMouseX, lastMouseY;
    SDL_Rect colorPreview;
    SDL_Rect hueSlider;
    SDL_Rect satSlider;
    SDL_Rect brightSlider;
    SDL_Rect sizeSlider;
    float hue;
    float saturation;
    float brightness;
    int penSize;
    bool draggingHue;
    bool draggingSat;
    bool draggingBright;
    bool draggingSize;
};

struct BlockPaletteUI {
    SDL_Renderer* renderer;
    Project* project;
    CodeAreaUI* codeArea;
    int currentCategory;
    int blockScrollOffset;
    SDL_Rect rect;
    int dragging;
    int dragBlockType;
    int dragStartX;
    int dragStartY;
    int dragOffsetX;
    int dragOffsetY;
    int previewX;
    int previewY;
    TTF_Font* font;
};

struct CodeAreaUI {
    SDL_Renderer* renderer;
    Project* project;
    ExecutionEngine* engine;
    int selectedSpriteIndex;
    int selectedScriptIndex;
    int selectedBlockIndex;
    int scrollX;
    int scrollY;
    SDL_Rect rect;
    int editingScript;
    int editingBlock;
    int editingParam;
    string editBuffer;
    TTF_Font* font;
};
// Function declarations
void free_block(Block* b);
bool Application_init(Application* app);
void Application_run(Application* app);
void Application_handleEvents(Application* app);
void Application_update(Application* app);
void Application_render(Application* app);
void Application_shutdown(Application* app);
Project* Project_create();
void Project_destroy(Project* proj);
bool Project_save(Project* proj, const char* filename);
bool Project_load(Project* proj, const char* filename);
void Project_addDefaultSprite(Project* proj, const char* name);
void Project_addDefaultBackdrop(Project* proj, const char* name);
void Project_addDefaultSound(Project* proj, const char* name);
void Project_addSoundFromFile(Project* proj, const char* name, const char* filepath);
void Sprite_addDefaultCostume(Sprite* sprite, const char* name);
void Sprite_addCostumeFromFile(Sprite* sprite, SDL_Renderer* renderer, const char* filepath);
ExecutionEngine* ExecutionEngine_create(Project* proj);
void ExecutionEngine_destroy(ExecutionEngine* eng);
void ExecutionEngine_step(ExecutionEngine* eng, Uint32 currentTime);
void ExecutionEngine_run(ExecutionEngine* eng);
void ExecutionEngine_stop(ExecutionEngine* eng);
void ExecutionEngine_addContext(ExecutionEngine* eng, int spriteId, int scriptId);
void ExecutionEngine_addChildContext(ExecutionEngine* eng, int spriteId, int scriptId, ExecutionContext* parent);
void ExecutionEngine_removeContext(ExecutionEngine* eng, int index);
void ExecutionEngine_startKeyScripts(ExecutionEngine* eng, SDL_Keycode key);
int ExecutionEngine_startSpriteClickScripts(ExecutionEngine* eng, int mouseX, int mouseY, SDL_Rect stageRect);
SpriteManagerUI* SpriteManagerUI_create(SDL_Renderer* ren, Project* proj);
void SpriteManagerUI_destroy(SpriteManagerUI* ui);
void SpriteManagerUI_render(SpriteManagerUI* ui);
void SpriteManagerUI_handleEvent(SpriteManagerUI* ui, SDL_Event* e);
void SpriteManagerUI_uploadCostume(SpriteManagerUI* ui, const char* filepath);
BackdropManagerUI* BackdropManagerUI_create(SDL_Renderer* ren, Project* proj);
void BackdropManagerUI_destroy(BackdropManagerUI* ui);
void BackdropManagerUI_render(BackdropManagerUI* ui);
void BackdropManagerUI_handleEvent(BackdropManagerUI* ui, SDL_Event* e);
void BackdropManagerUI_uploadBackdrop(BackdropManagerUI* ui, const char* filepath);
SoundManagerUI* SoundManagerUI_create(SDL_Renderer* ren, Project* proj);
void SoundManagerUI_destroy(SoundManagerUI* ui);
void SoundManagerUI_render(SoundManagerUI* ui);
void SoundManagerUI_handleEvent(SoundManagerUI* ui, SDL_Event* e);
void SoundManagerUI_uploadSound(SoundManagerUI* ui, const char* filepath);
PenToolUI* PenToolUI_create(SDL_Renderer* ren, Project* proj);
void PenToolUI_destroy(PenToolUI* ui);
void PenToolUI_render(PenToolUI* ui);
void PenToolUI_handleEvent(PenToolUI* ui, SDL_Event* e, Application* app);
BlockPaletteUI* BlockPaletteUI_create(SDL_Renderer* ren, Project* proj, CodeAreaUI* codeArea);
void BlockPaletteUI_destroy(BlockPaletteUI* ui);
void BlockPaletteUI_render(BlockPaletteUI* ui);
void BlockPaletteUI_handleEvent(BlockPaletteUI* ui, SDL_Event* e);
CodeAreaUI* CodeAreaUI_create(SDL_Renderer* ren, Project* proj, ExecutionEngine* engine);
void CodeAreaUI_destroy(CodeAreaUI* ui);
void CodeAreaUI_render(CodeAreaUI* ui);
void CodeAreaUI_handleEvent(CodeAreaUI* ui, SDL_Event* e);
void CodeAreaUI_addBlockAt(CodeAreaUI* ui, int blockType, int screenX, int screenY);
void Application_createPenCanvasForSprite(Application* app, Sprite* sprite);
SDL_Color hslToRgb(float h, float s, float l);
void drawLineOnCanvas(SDL_Renderer* renderer, SDL_Texture* canvas, int x1, int y1, int x2, int y2, SDL_Color color, int size);
int compareSpritesByLayer(const void* a, const void* b);
void preprocess_script(Script* script);
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path);
int findSoundByName(Project* proj, const char* name);
Variable* findVariable(Project* proj, const string& name);
void setVariable(Project* proj, const string& name, const Value& val);
Value getVariable(Project* proj, const string& name);
Value evaluateBlock(Block* b, ExecutionContext* ctx, Project* proj);
SDL_Scancode keyNameToScancode(const char* name);
bool findBlockAt(CodeAreaUI* ui, int mouseX, int mouseY, int* outScriptIndex, int* outBlockIndex);

void setError(Application* app, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(app->lastError, sizeof(app->lastError), format, args);
    va_end(args);
    app->errorTime = SDL_GetTicks();
    printf("Error: %s\n", app->lastError);
}

void clearError(Application* app) {
    app->lastError[0] = '\0';
    app->errorTime = 0;
}
