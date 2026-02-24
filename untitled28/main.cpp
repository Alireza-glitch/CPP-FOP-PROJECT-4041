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
char* findFontFile(const char* filename) {
    char* basePath = SDL_GetBasePath();
    if (basePath) {
        size_t len = strlen(basePath) + strlen(filename) + 1;
        char* fullPath = (char*)malloc(len);
        snprintf(fullPath, len, "%s%s", basePath, filename);
        SDL_free(basePath);
        FILE* f = fopen(fullPath, "r");
        if (f) {
            fclose(f);
            return fullPath;
        }
        free(fullPath);
    }
#ifdef _WIN32
    char* windir = getenv("WINDIR");
    if (windir) {
        size_t len = strlen(windir) + 20 + strlen(filename);
        char* winPath = (char*)malloc(len);
        snprintf(winPath, len, "%s\\Fonts\\%s", windir, filename);
        FILE* f = fopen(winPath, "r");
        if (f) {
            fclose(f);
            return winPath;
        }
        free(winPath);
    }
#endif
    return strdup(filename);
}

const char* block_type_names[] = {
        "move", "turn", "go to", "change x", "change y", "set direction",
        "say", "think", "switch costume", "next costume", "switch backdrop", "next backdrop",
        "change size", "set size", "change color", "set color", "clear effects",
        "show", "hide", "go to layer", "change layer",
        "play sound", "play sound until done", "stop all sounds", "change volume", "set volume",
        "when flag clicked", "when key pressed", "when sprite clicked",
        "broadcast", "broadcast and wait", "when I receive",
        "wait", "repeat", "forever", "if", "if else", "wait until", "repeat until", "stop all",
        "custom call", "custom define",
        "add", "subtract", "multiply", "divide", "random",
        "<", ">", "=", "and", "or", "not",
        "join", "letter of", "length", "mod", "round",
        "abs", "sqrt", "sin", "cos", "tan", "asin", "acos", "atan", "ln", "log", "pow",
        "set var", "change var", "var get", "number", "string",
        "touching edge?", "mouse x", "mouse y", "key pressed?",
        "pen down", "pen up", "set pen color", "change pen color",
        "set pen brightness", "change pen brightness",
        "set pen saturation", "change pen saturation",
        "set pen size", "change pen size", "erase all", "stamp",
        "change brightness", "set brightness",
        "change saturation", "set saturation",
        "costume number", "costume name",
        "backdrop number", "backdrop name",
        "size",
        "touching mouse-pointer?", "touching sprite?", "touching color?", "color is touching color?",
        "distance to", "ask and wait", "answer", "mouse down?",
        "set drag mode", "timer", "reset timer",
        "go to random position", "go to mouse-pointer", "if on edge, bounce",
        "else", "endif", "endloop"
};

SDL_Color get_block_color(BlockType type) {
    if (type >= BLOCK_MOVE && type <= BLOCK_SET_DIRECTION) return {100, 150, 255, 255};
    if (type == BLOCK_GO_TO_RANDOM || type == BLOCK_GO_TO_MOUSE || type == BLOCK_IF_ON_EDGE_BOUNCE)
        return {100, 150, 255, 255};
    if (type >= BLOCK_SAY && type <= BLOCK_CHANGE_LAYER) return {150, 100, 255, 255};
    if (type >= BLOCK_PLAY_SOUND && type <= BLOCK_SET_VOLUME) return {200, 100, 200, 255};
    if (type >= BLOCK_WHEN_FLAG_CLICKED && type <= BLOCK_WHEN_I_RECEIVE) return {255, 200, 100, 255};
    if (type >= BLOCK_WAIT && type <= BLOCK_STOP_ALL) return {255, 150, 100, 255};
    if (type >= BLOCK_CUSTOM_CALL && type <= BLOCK_CUSTOM_DEFINE) return {150, 150, 150, 255};
    if (type >= BLOCK_ADD && type <= BLOCK_POW) return {100, 255, 100, 255};
    if (type >= BLOCK_SET_VARIABLE && type <= BLOCK_STRING) return {255, 255, 100, 255};
    if (type >= BLOCK_TOUCHING_EDGE && type <= BLOCK_KEY_PRESSED) return {100, 200, 255, 255};
    if (type >= BLOCK_PEN_DOWN && type <= BLOCK_STAMP) return {100, 255, 100, 255};
    if (type == BLOCK_CHANGE_BRIGHTNESS || type == BLOCK_SET_BRIGHTNESS ||
        type == BLOCK_CHANGE_SATURATION || type == BLOCK_SET_SATURATION ||
        type == BLOCK_COSTUME_NUMBER || type == BLOCK_COSTUME_NAME ||
        type == BLOCK_BACKDROP_NUMBER || type == BLOCK_BACKDROP_NAME ||
        type == BLOCK_SIZE) {
        return {150, 100, 255, 255};
    }
    if (type == BLOCK_TOUCHING_MOUSEPOINTER || type == BLOCK_TOUCHING_SPRITE ||
        type == BLOCK_TOUCHING_COLOR || type == BLOCK_COLOR_TOUCHING_COLOR ||
        type == BLOCK_DISTANCE_TO || type == BLOCK_ASK_AND_WAIT ||
        type == BLOCK_ANSWER || type == BLOCK_MOUSE_DOWN ||
        type == BLOCK_SET_DRAG_MODE || type == BLOCK_TIMER ||
        type == BLOCK_RESET_TIMER) {
        return {100, 200, 255, 255};
    }
    return {160, 160, 160, 255};
}

void DrawRoundedRect(SDL_Renderer* renderer, SDL_Rect rect, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect inner = {rect.x + radius, rect.y, rect.w - 2*radius, rect.h};
    SDL_RenderFillRect(renderer, &inner);
    inner = {rect.x, rect.y + radius, rect.w, rect.h - 2*radius};
    SDL_RenderFillRect(renderer, &inner);
    for (int dx = -radius; dx <= radius; dx++) {
        for (int dy = -radius; dy <= radius; dy++) {
            if (dx*dx + dy*dy <= radius*radius) {
                SDL_RenderDrawPoint(renderer, rect.x + radius + dx, rect.y + radius + dy);
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + dx, rect.y + radius + dy);
                SDL_RenderDrawPoint(renderer, rect.x + radius + dx, rect.y + rect.h - radius + dy);
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + dx, rect.y + rect.h - radius + dy);
            }
        }
    }
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
        }
    }
    return newTexture;
}

int findSoundByName(Project* proj, const char* name) {
    for (size_t i = 0; i < proj->sounds.size(); i++) {
        if (proj->sounds[i]->name == name) return i;
    }
    return -1;
}

Variable* findVariable(Project* proj, const string& name) {
    for (size_t i = 0; i < proj->globalVariables.size(); i++) {
        if (proj->globalVariables[i]->name == name)
            return proj->globalVariables[i];
    }
    return nullptr;
}

void setVariable(Project* proj, const string& name, const Value& val) {
    Variable* var = findVariable(proj, name);
    if (var) {
        var->value = val;
    } else {
        Variable* newVar = new Variable;
        newVar->name = name;
        newVar->value = val;
        proj->globalVariables.push_back(newVar);
    }
}

Value getVariable(Project* proj, const string& name) {
    Variable* var = findVariable(proj, name);
    if (var) return var->value;
    return make_number(0);
}

SDL_Scancode keyNameToScancode(const char* name) {
    if (!name) return SDL_SCANCODE_UNKNOWN;
    string n = name;
    if (n == "space") return SDL_SCANCODE_SPACE;
    if (n == "up arrow") return SDL_SCANCODE_UP;
    if (n == "down arrow") return SDL_SCANCODE_DOWN;
    if (n == "left arrow") return SDL_SCANCODE_LEFT;
    if (n == "right arrow") return SDL_SCANCODE_RIGHT;
    if (n == "a") return SDL_SCANCODE_A;
    if (n == "b") return SDL_SCANCODE_B;
    if (n == "c") return SDL_SCANCODE_C;
    if (n == "d") return SDL_SCANCODE_D;
    if (n == "e") return SDL_SCANCODE_E;
    if (n == "f") return SDL_SCANCODE_F;
    if (n == "g") return SDL_SCANCODE_G;
    if (n == "h") return SDL_SCANCODE_H;
    if (n == "i") return SDL_SCANCODE_I;
    if (n == "j") return SDL_SCANCODE_J;
    if (n == "k") return SDL_SCANCODE_K;
    if (n == "l") return SDL_SCANCODE_L;
    if (n == "m") return SDL_SCANCODE_M;
    if (n == "n") return SDL_SCANCODE_N;
    if (n == "o") return SDL_SCANCODE_O;
    if (n == "p") return SDL_SCANCODE_P;
    if (n == "q") return SDL_SCANCODE_Q;
    if (n == "r") return SDL_SCANCODE_R;
    if (n == "s") return SDL_SCANCODE_S;
    if (n == "t") return SDL_SCANCODE_T;
    if (n == "u") return SDL_SCANCODE_U;
    if (n == "v") return SDL_SCANCODE_V;
    if (n == "w") return SDL_SCANCODE_W;
    if (n == "x") return SDL_SCANCODE_X;
    if (n == "y") return SDL_SCANCODE_Y;
    if (n == "z") return SDL_SCANCODE_Z;
    if (n == "0") return SDL_SCANCODE_0;
    if (n == "1") return SDL_SCANCODE_1;
    if (n == "2") return SDL_SCANCODE_2;
    if (n == "3") return SDL_SCANCODE_3;
    if (n == "4") return SDL_SCANCODE_4;
    if (n == "5") return SDL_SCANCODE_5;
    if (n == "6") return SDL_SCANCODE_6;
    if (n == "7") return SDL_SCANCODE_7;
    if (n == "8") return SDL_SCANCODE_8;
    if (n == "9") return SDL_SCANCODE_9;
    return SDL_SCANCODE_UNKNOWN;
}
// evaluateBlock function
Value evaluateBlock(Block* b, ExecutionContext* ctx, Project* proj) {
    switch (b->type) {
        case BLOCK_NUMBER:
            return make_number(b->numParam1);
        case BLOCK_STRING:
            return make_string(b->strParam);
        case BLOCK_VARIABLE_GET:
            return getVariable(proj, b->strParam);
        case BLOCK_ADD: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            float result = value_to_number(left) + value_to_number(right);
            return make_number(result);
        }
        case BLOCK_SUBTRACT: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            float result = value_to_number(left) - value_to_number(right);
            return make_number(result);
        }
        case BLOCK_MULTIPLY: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            float result = value_to_number(left) * value_to_number(right);
            return make_number(result);
        }
        case BLOCK_DIVIDE: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            float divisor = value_to_number(right);
            if (divisor == 0) {
                setError(gApp, "⚠️ تقسیم بر صفر!");
                return make_number(0);
            }
            float result = value_to_number(left) / divisor;
            return make_number(result);
        }
        case BLOCK_RANDOM: {
            Value low = evaluateBlock(b->children[0], ctx, proj);
            Value high = evaluateBlock(b->children[1], ctx, proj);
            float l = value_to_number(low);
            float h = value_to_number(high);
            float r = l + (rand() / (float)RAND_MAX) * (h - l);
            return make_number(r);
        }
        case BLOCK_LT: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            return make_number(value_to_number(left) < value_to_number(right) ? 1 : 0);
        }
        case BLOCK_GT: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            return make_number(value_to_number(left) > value_to_number(right) ? 1 : 0);
        }
        case BLOCK_EQUALS: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            if (left.type == Value::VAL_NUMBER && right.type == Value::VAL_NUMBER)
                return make_number(left.num == right.num ? 1 : 0);
            string s1 = value_to_string(left);
            string s2 = value_to_string(right);
            return make_number(s1 == s2 ? 1 : 0);
        }
        case BLOCK_AND: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            return make_number((value_to_number(left) != 0 && value_to_number(right) != 0) ? 1 : 0);
        }
        case BLOCK_OR: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            return make_number((value_to_number(left) != 0 || value_to_number(right) != 0) ? 1 : 0);
        }
        case BLOCK_NOT: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            return make_number(value_to_number(val) == 0 ? 1 : 0);
        }
        case BLOCK_JOIN: {
            Value left = evaluateBlock(b->children[0], ctx, proj);
            Value right = evaluateBlock(b->children[1], ctx, proj);
            string s1 = value_to_string(left);
            string s2 = value_to_string(right);
            return make_string(s1 + s2);
        }
        case BLOCK_LETTER_OF: {
            Value strVal = evaluateBlock(b->children[0], ctx, proj);
            Value idxVal = evaluateBlock(b->children[1], ctx, proj);
            string s = value_to_string(strVal);
            int idx = (int)value_to_number(idxVal);
            if (idx >= 1 && idx <= (int)s.size()) {
                return make_string(string(1, s[idx-1]));
            }
            return make_string("");
        }
        case BLOCK_LENGTH: {
            Value strVal = evaluateBlock(b->children[0], ctx, proj);
            string s = value_to_string(strVal);
            return make_number((float)s.size());
        }
        case BLOCK_MOD: {
            Value aVal = evaluateBlock(b->children[0], ctx, proj);
            Value bVal = evaluateBlock(b->children[1], ctx, proj);
            float a = value_to_number(aVal);
            float b = value_to_number(bVal);
            if (b == 0) {
                setError(gApp, "⚠️ پیمانه با مقسوم‌علیه صفر.");
                return make_number(0);
            }
            return make_number(fmod(a, b));
        }
        case BLOCK_ROUND: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            return make_number(roundf(value_to_number(val)));
        }
        case BLOCK_ABS: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            return make_number(fabs(value_to_number(val)));
        }
        case BLOCK_SQRT: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            float v = value_to_number(val);
            if (v < 0) {
                setError(gApp, "⚠️ جذر عدد منفی (%.2f) نامعتبر است.", v);
                return make_number(0);
            }
            return make_number(sqrtf(v));
        }
        case BLOCK_SIN: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            return make_number(sinf(value_to_number(val) * M_PI / 180.0f));
        }
        case BLOCK_COS: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            return make_number(cosf(value_to_number(val) * M_PI / 180.0f));
        }
        case BLOCK_TAN: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            return make_number(tanf(value_to_number(val) * M_PI / 180.0f));
        }
        case BLOCK_ASIN: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            float v = value_to_number(val);
            if (v < -1 || v > 1) {
                setError(gApp, "⚠️ آرکسینوس خارج از محدوده [-1,1] (%.2f) نامعتبر است.", v);
                return make_number(0);
            }
            return make_number(asinf(v) * 180.0f / M_PI);
        }
        case BLOCK_ACOS: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            float v = value_to_number(val);
            if (v < -1 || v > 1) {
                setError(gApp, "⚠️ آرککسینوس خارج از محدوده [-1,1] (%.2f) نامعتبر است.", v);
                return make_number(0);
            }
            return make_number(acosf(v) * 180.0f / M_PI);
        }
        case BLOCK_ATAN: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            return make_number(atanf(value_to_number(val)) * 180.0f / M_PI);
        }
        case BLOCK_LN: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            float v = value_to_number(val);
            if (v <= 0) {
                setError(gApp, "⚠️ لگاریتم طبیعی عدد غیرمثبت (%.2f) نامعتبر است.", v);
                return make_number(0);
            }
            return make_number(logf(v));
        }
        case BLOCK_LOG: {
            Value val = evaluateBlock(b->children[0], ctx, proj);
            float v = value_to_number(val);
            if (v <= 0) {
                setError(gApp, "⚠️ لگاریتم عدد غیرمثبت (%.2f) نامعتبر است.", v);
                return make_number(0);
            }
            return make_number(log10f(v));
        }
        case BLOCK_POW: {
            Value base = evaluateBlock(b->children[0], ctx, proj);
            Value exp = evaluateBlock(b->children[1], ctx, proj);
            return make_number(powf(value_to_number(base), value_to_number(exp)));
        }
        case BLOCK_TOUCHING_EDGE: {
            Sprite* s = proj->sprites[ctx->spriteId];
            return make_number((s->x > 240 || s->x < -240 || s->y > 180 || s->y < -180) ? 1 : 0);
        }
        case BLOCK_MOUSE_X: {
            int x, y;
            SDL_GetMouseState(&x, &y);
            SDL_Rect stageRect = {0, 40, 400, 600};
            return make_number((float)(x - stageRect.w/2));
        }
        case BLOCK_MOUSE_Y: {
            int x, y;
            SDL_GetMouseState(&x, &y);
            SDL_Rect stageRect = {0, 40, 400, 600};
            return make_number((float)(stageRect.h/2 - y));
        }
        case BLOCK_KEY_PRESSED: {
            const Uint8* state = SDL_GetKeyboardState(NULL);
            SDL_Scancode sc = keyNameToScancode(b->strParam.c_str());
            return make_number((sc != SDL_SCANCODE_UNKNOWN && state[sc]) ? 1 : 0);
        }
        case BLOCK_COSTUME_NUMBER: {
            Sprite* s = proj->sprites[ctx->spriteId];
            return make_number((float)(s->currentCostume + 1));
        }
        case BLOCK_COSTUME_NAME: {
            Sprite* s = proj->sprites[ctx->spriteId];
            if (s->currentCostume >= 0 && s->currentCostume < (int)s->costumes.size()) {
                return make_string(s->costumes[s->currentCostume]->name);
            }
            return make_string("");
        }
        case BLOCK_BACKDROP_NUMBER: {
            return make_number((float)(proj->currentBackdrop + 1));
        }
        case BLOCK_BACKDROP_NAME: {
            if (proj->currentBackdrop >= 0 && proj->currentBackdrop < (int)proj->backdrops.size()) {
                return make_string(proj->backdrops[proj->currentBackdrop]->name);
            }
            return make_string("");
        }
        case BLOCK_SIZE: {
            Sprite* s = proj->sprites[ctx->spriteId];
            return make_number(s->size);
        }
        case BLOCK_TOUCHING_MOUSEPOINTER: {
            Sprite* s = proj->sprites[ctx->spriteId];
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int winW, winH;
            SDL_GetWindowSize(gWindow, &winW, &winH);
            int paletteWidth = 200, codeWidth = 400;
            int sceneWidth = winW - paletteWidth - codeWidth;
            int bottomHeight = 128;
            int rightPanelHeight = winH - 40 - bottomHeight;
            int backdropPanelHeight = 150, soundPanelHeight = 150;
            int sceneHeight = rightPanelHeight - backdropPanelHeight - soundPanelHeight;
            if (sceneHeight < 200) sceneHeight = 200;
            SDL_Rect stageRect = {paletteWidth + codeWidth, 40, sceneWidth, sceneHeight};
            float stageX = mouseX - (stageRect.x + stageRect.w/2);
            float stageY = (stageRect.y + stageRect.h/2) - mouseY;
            int spriteW = (int)(50 * s->size / 100.0f);
            int spriteH = (int)(50 * s->size / 100.0f);
            int touching = (stageX >= s->x - spriteW/2 && stageX <= s->x + spriteW/2 &&
                            stageY >= s->y - spriteH/2 && stageY <= s->y + spriteH/2);
            return make_number(touching ? 1 : 0);
        }
        case BLOCK_TOUCHING_SPRITE: {
            Sprite* s = proj->sprites[ctx->spriteId];
            string otherName = b->strParam;
            if (otherName.empty()) return make_number(0);
            for (size_t i = 0; i < proj->sprites.size(); i++) {
                if ((int)i == ctx->spriteId) continue;
                Sprite* other = proj->sprites[i];
                if (other->name != otherName) continue;
                if (!other->visible) continue;
                int w1 = (int)(50 * s->size / 100.0f);
                int h1 = (int)(50 * s->size / 100.0f);
                int w2 = (int)(50 * other->size / 100.0f);
                int h2 = (int)(50 * other->size / 100.0f);
                if (abs(s->x - other->x) < (w1/2 + w2/2) && abs(s->y - other->y) < (h1/2 + h2/2)) {
                    return make_number(1);
                }
                break;
            }
            return make_number(0);
        }
        case BLOCK_TOUCHING_COLOR:
        case BLOCK_COLOR_TOUCHING_COLOR:
            // فعلاً پیاده‌سازی نشده
            return make_number(0);
        case BLOCK_DISTANCE_TO: {
            Sprite* s = proj->sprites[ctx->spriteId];
            string target = b->strParam;
            if (target.empty()) return make_number(0);
            float dx = 0, dy = 0;
            if (target == "mouse-pointer") {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int winW, winH;
                SDL_GetWindowSize(gWindow, &winW, &winH);
                int paletteWidth = 200, codeWidth = 400;
                int sceneWidth = winW - paletteWidth - codeWidth;
                int bottomHeight = 128;
                int rightPanelHeight = winH - 40 - bottomHeight;
                int backdropPanelHeight = 150, soundPanelHeight = 150;
                int sceneHeight = rightPanelHeight - backdropPanelHeight - soundPanelHeight;
                if (sceneHeight < 200) sceneHeight = 200;
                SDL_Rect stageRect = {paletteWidth + codeWidth, 40, sceneWidth, sceneHeight};
                float stageX = mouseX - (stageRect.x + stageRect.w/2);
                float stageY = (stageRect.y + stageRect.h/2) - mouseY;
                dx = stageX - s->x;
                dy = stageY - s->y;
            } else {
                for (size_t i = 0; i < proj->sprites.size(); i++) {
                    if (proj->sprites[i]->name == target) {
                        dx = proj->sprites[i]->x - s->x;
                        dy = proj->sprites[i]->y - s->y;
                        break;
                    }
                }
            }
            return make_number(sqrtf(dx*dx + dy*dy));
        }
        case BLOCK_ANSWER: {
            return make_string(proj->answer);
        }
        case BLOCK_MOUSE_DOWN: {
            Uint32 buttons = SDL_GetMouseState(NULL, NULL);
            return make_number((buttons & SDL_BUTTON_LMASK) ? 1 : 0);
        }
        case BLOCK_TIMER: {
            return make_number((SDL_GetTicks() - proj->timerStart) / 1000.0f);
        }
        default:
            return make_number(0);
    }
}
// Utility functions
SDL_Color hslToRgb(float h, float s, float l) {
    float hue = h * 360.0f / 200.0f;
    float saturation = s / 100.0f;
    float lightness = l / 100.0f;
    float c = (1 - fabs(2 * lightness - 1)) * saturation;
    float x = c * (1 - fabs(fmod(hue / 60.0f, 2) - 1));
    float m = lightness - c/2;
    float r, g, b;
    if (hue < 60) { r = c; g = x; b = 0; }
    else if (hue < 120) { r = x; g = c; b = 0; }
    else if (hue < 180) { r = 0; g = c; b = x; }
    else if (hue < 240) { r = 0; g = x; b = c; }
    else if (hue < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    return {(Uint8)((r + m)*255), (Uint8)((g + m)*255), (Uint8)((b + m)*255), 255};
}

void drawLineOnCanvas(SDL_Renderer* renderer, SDL_Texture* canvas, int x1, int y1, int x2, int y2, SDL_Color color, int size) {
    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, canvas);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_SetRenderTarget(renderer, oldTarget);
}

void Application_createPenCanvasForSprite(Application* app, Sprite* sprite) {
    int winW, winH;
    SDL_GetWindowSize(app->window, &winW, &winH);
    if (sprite->penCanvas) SDL_DestroyTexture(sprite->penCanvas);
    sprite->penCanvas = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, winW, winH);
    SDL_SetTextureBlendMode(sprite->penCanvas, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(app->renderer, sprite->penCanvas);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 0);
    SDL_RenderClear(app->renderer);
    SDL_SetRenderTarget(app->renderer, NULL);
}

int compareSpritesByLayer(const void* a, const void* b) {
    Sprite* sa = *(Sprite**)a;
    Sprite* sb = *(Sprite**)b;
    return sa->layer - sb->layer;
}

int main(int argc, char* argv[]) {
    Application app;
    gApp = &app;
    if (!Application_init(&app)) {
        return 1;
    }
    Application_run(&app);
    Application_shutdown(&app);
    return 0;
}
// Application functions
bool Application_init(Application* app) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;
    if (TTF_Init() == -1) return false;
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! %s\n", IMG_GetError());
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! %s\n", Mix_GetError());
        return false;
    }
    app->window = SDL_CreateWindow("Scratch-like IDE",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   0, 0,
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!app->window) return false;
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    if (!app->renderer) return false;

    gWindow = app->window;

    app->currentProject = Project_create();
    app->engine = ExecutionEngine_create(app->currentProject);
    app->spriteManagerUI = SpriteManagerUI_create(app->renderer, app->currentProject);
    app->backdropManagerUI = BackdropManagerUI_create(app->renderer, app->currentProject);
    app->soundManagerUI = SoundManagerUI_create(app->renderer, app->currentProject);
    app->penToolUI = PenToolUI_create(app->renderer, app->currentProject);
    app->codeArea = CodeAreaUI_create(app->renderer, app->currentProject, app->engine);
    app->blockPalette = BlockPaletteUI_create(app->renderer, app->currentProject, app->codeArea);

    char* fontPath = findFontFile("arial.ttf");
    app->speechFont = TTF_OpenFont(fontPath, 16);
    if (!app->speechFont) {
        printf("Warning: could not load speech font\n");
    }
    free(fontPath);

    Project_addDefaultSprite(app->currentProject, "Sprite1");
    Project_addDefaultBackdrop(app->currentProject, "Backdrop1");
    Project_addDefaultSound(app->currentProject, "Sound1");

    for (size_t i = 0; i < app->currentProject->sprites.size(); i++) {
        Application_createPenCanvasForSprite(app, app->currentProject->sprites[i]);
    }

    app->running = true;
    app->paused = false;
    app->executing = false;
    app->dragSpriteIndex = -1;
    app->answerReady = false;
    app->textInputBuffer[0] = '\0';
    app->lastError[0] = '\0';
    app->errorTime = 0;
    return true;
}

void Application_run(Application* app) {
    while (app->running) {
        Application_handleEvents(app);
        if (!app->paused) {
            Application_update(app);
        }
        Application_render(app);
        SDL_Delay(16);
    }
}

void Application_handleEvents(Application* app) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            app->running = false;
        }
        SpriteManagerUI_handleEvent(app->spriteManagerUI, &e);
        BackdropManagerUI_handleEvent(app->backdropManagerUI, &e);
        SoundManagerUI_handleEvent(app->soundManagerUI, &e);
        PenToolUI_handleEvent(app->penToolUI, &e, app);
        BlockPaletteUI_handleEvent(app->blockPalette, &e);
        CodeAreaUI_handleEvent(app->codeArea, &e);

        if (app->spriteManagerUI->selectedSpriteIndex != app->codeArea->selectedSpriteIndex) {
            app->codeArea->selectedSpriteIndex = app->spriteManagerUI->selectedSpriteIndex;
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_RETURN && (e.key.keysym.mod & KMOD_ALT)) {
                Uint32 flags = SDL_GetWindowFlags(app->window);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                    SDL_SetWindowFullscreen(app->window, 0);
                } else {
                    SDL_SetWindowFullscreen(app->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
            }
        }

        if (e.type == SDL_TEXTINPUT) {
            size_t len = strlen(app->textInputBuffer);
            size_t newLen = len + strlen(e.text.text);
            if (newLen < sizeof(app->textInputBuffer) - 1) {
                strcat(app->textInputBuffer, e.text.text);
            }
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE) {
            size_t len = strlen(app->textInputBuffer);
            if (len > 0) {
                app->textInputBuffer[len - 1] = '\0';
            }
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && app->answerReady == false) {
            SDL_StopTextInput();
            strncpy(app->pendingAnswer, app->textInputBuffer, sizeof(app->pendingAnswer)-1);
            app->pendingAnswer[sizeof(app->pendingAnswer)-1] = '\0';
            app->answerReady = true;
            app->textInputBuffer[0] = '\0';
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x, y = e.button.y;
            int winW, winH;
            SDL_GetWindowSize(app->window, &winW, &winH);

            int startY = 70;
            int paletteWidth = 200;
            int codeWidth = 400;
            int sceneWidth = winW - paletteWidth - codeWidth;
            int bottomHeight = 128;
            int rightPanelHeight = winH - startY - bottomHeight;
            int backdropPanelHeight = 150;
            int soundPanelHeight = 150;
            int sceneHeight = rightPanelHeight - backdropPanelHeight - soundPanelHeight;
            if (sceneHeight < 200) sceneHeight = 200;
            SDL_Rect sceneRect = {paletteWidth + codeWidth, startY, sceneWidth, sceneHeight};

            if (y >= startY && y < winH - bottomHeight && x >= sceneRect.x && x < sceneRect.x + sceneRect.w) {
                int clickedSprite = ExecutionEngine_startSpriteClickScripts(app->engine, x, y, sceneRect);
                if (clickedSprite >= 0) {
                    app->spriteManagerUI->selectedSpriteIndex = clickedSprite;
                    app->codeArea->selectedSpriteIndex = clickedSprite;
                    Sprite* s = app->currentProject->sprites[clickedSprite];
                    if (s->draggable) {
                        app->dragSpriteIndex = clickedSprite;
                        app->dragStartX = x;
                        app->dragStartY = y;
                        app->dragSpriteOrigX = s->x;
                        app->dragSpriteOrigY = s->y;
                    }
                }
            }

            if (y < 40) {
                int btnX = 5;
                const char* buttons[] = {"New", "Save", "Load", "Start", "Stop", "Step", "Sprite", "Backdrop", "Sound"};
                int numButtons = 9;
                for (int i = 0; i < numButtons; i++) {
                    if (x >= btnX && x <= btnX + 70) {
                        switch (i) {
                            case 0:
                                Project_destroy(app->currentProject);
                                app->currentProject = Project_create();
                                Project_addDefaultSprite(app->currentProject, "Sprite1");
                                Project_addDefaultBackdrop(app->currentProject, "Backdrop1");
                                Project_addDefaultSound(app->currentProject, "Sound1");
                                app->spriteManagerUI->project = app->currentProject;
                                app->backdropManagerUI->project = app->currentProject;
                                app->soundManagerUI->project = app->currentProject;
                                app->penToolUI->project = app->currentProject;
                                app->blockPalette->project = app->currentProject;
                                app->codeArea->project = app->currentProject;
                                app->engine->project = app->currentProject;
                                app->engine->contexts.clear();
                                app->spriteManagerUI->scrollOffset = 0;
                                app->backdropManagerUI->scrollOffset = 0;
                                app->soundManagerUI->scrollOffset = 0;
                                app->blockPalette->blockScrollOffset = 0;
                                app->codeArea->scrollX = 0;
                                app->codeArea->scrollY = 0;
                                app->spriteManagerUI->selectedSpriteIndex = 0;
                                app->codeArea->selectedSpriteIndex = 0;
                                app->codeArea->editingScript = -1;
                                app->codeArea->editingBlock = -1;
                                app->codeArea->editingParam = -1;
                                app->codeArea->editBuffer.clear();
                                SDL_StopTextInput();
                                for (size_t j = 0; j < app->currentProject->sprites.size(); j++) {
                                    Application_createPenCanvasForSprite(app, app->currentProject->sprites[j]);
                                }
                                printf("New project created\n");
                                break;
                            case 1:
                                if (Project_save(app->currentProject, "project.txt")) {
                                    printf("Project saved to project.txt\n");
                                } else {
                                    printf("Failed to save project\n");
                                }
                                break;
                            case 2:
                                if (Project_load(app->currentProject, "project.txt")) {
                                    printf("Project loaded from project.txt\n");
                                    for (size_t j = 0; j < app->currentProject->sprites.size(); j++) {
                                        Application_createPenCanvasForSprite(app, app->currentProject->sprites[j]);
                                    }
                                    app->spriteManagerUI->selectedSpriteIndex = (app->currentProject->sprites.size() > 0) ? 0 : -1;
                                    app->codeArea->selectedSpriteIndex = app->spriteManagerUI->selectedSpriteIndex;
                                    app->spriteManagerUI->scrollOffset = 0;
                                    app->backdropManagerUI->scrollOffset = 0;
                                    app->soundManagerUI->scrollOffset = 0;
                                    app->blockPalette->blockScrollOffset = 0;
                                    app->codeArea->scrollX = 0;
                                    app->codeArea->scrollY = 0;
                                    app->codeArea->editingScript = -1;
                                    app->codeArea->editingBlock = -1;
                                    app->codeArea->editingParam = -1;
                                    app->codeArea->editBuffer.clear();
                                    SDL_StopTextInput();
                                } else {
                                    printf("Failed to load project\n");
                                }
                                break;
                            case 3:
                                app->executing = true;
                                app->paused = false;
                                ExecutionEngine_run(app->engine);
                                printf("Start\n");
                                break;
                            case 4:
                                app->executing = false;
                                ExecutionEngine_stop(app->engine);
                                printf("Stop\n");
                                break;
                            case 5:
                                app->engine->stepMode = true;
                                ExecutionEngine_step(app->engine, SDL_GetTicks());
                                app->engine->stepMode = false;
                                printf("Step\n");
                                break;
                            case 6:
                                Project_addDefaultSprite(app->currentProject, "Sprite");
                                Application_createPenCanvasForSprite(app, app->currentProject->sprites.back());
                                app->spriteManagerUI->selectedSpriteIndex = app->currentProject->sprites.size() - 1;
                                app->codeArea->selectedSpriteIndex = app->currentProject->sprites.size() - 1;
                                printf("Add sprite\n");
                                break;
                            case 7:
                                Project_addDefaultBackdrop(app->currentProject, "Backdrop");
                                printf("Add backdrop\n");
                                break;
                            case 8:
                                Project_addDefaultSound(app->currentProject, "Sound");
                                app->soundManagerUI->selectedSoundIndex = app->currentProject->sounds.size() - 1;
                                printf("Add sound\n");
                                break;
                        }
                        break;
                    }
                    btnX += 80;
                }
                continue;
            }
        }

        if (e.type == SDL_MOUSEMOTION && app->dragSpriteIndex >= 0) {
            int x = e.motion.x, y = e.motion.y;
            int winW, winH;
            SDL_GetWindowSize(app->window, &winW, &winH);
            int startY = 70;
            int paletteWidth = 200, codeWidth = 400;
            int sceneWidth = winW - paletteWidth - codeWidth;
            int bottomHeight = 128, rightPanelHeight = winH - startY - bottomHeight;
            int backdropPanelHeight = 150, soundPanelHeight = 150;
            int sceneHeight = rightPanelHeight - backdropPanelHeight - soundPanelHeight;
            if (sceneHeight < 200) sceneHeight = 200;
            SDL_Rect sceneRect = {paletteWidth + codeWidth, startY, sceneWidth, sceneHeight};

            float stageX = (x - (sceneRect.x + sceneRect.w/2));
            float stageY = (sceneRect.y + sceneRect.h/2 - y);
            if (stageX < -240) stageX = -240;
            if (stageX > 240) stageX = 240;
            if (stageY < -180) stageY = -180;
            if (stageY > 180) stageY = 180;

            Sprite* s = app->currentProject->sprites[app->dragSpriteIndex];
            s->x = stageX;
            s->y = stageY;
        }

        if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT && app->dragSpriteIndex >= 0) {
            app->dragSpriteIndex = -1;
        }

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_SPACE:
                    app->paused = !app->paused;
                    break;
                case SDLK_F5:
                    app->executing = true;
                    app->paused = false;
                    ExecutionEngine_run(app->engine);
                    break;
                case SDLK_F6:
                    app->engine->stepMode = true;
                    ExecutionEngine_step(app->engine, SDL_GetTicks());
                    app->engine->stepMode = false;
                    break;
                case SDLK_F7:
                    if (app->spriteManagerUI->selectedSpriteIndex >= 0) {
                        SpriteManagerUI_uploadCostume(app->spriteManagerUI, "sprite.png");
                    } else {
                        printf("No sprite selected.\n");
                    }
                    break;
                case SDLK_F8:
                    if (app->backdropManagerUI->selectedBackdropIndex >= 0) {
                        BackdropManagerUI_uploadBackdrop(app->backdropManagerUI, "backdrop.png");
                    } else {
                        printf("Please select a backdrop first from the left panel.\n");
                    }
                    break;
                case SDLK_F9:
                    if (app->soundManagerUI->selectedSoundIndex >= 0) {
                        SoundManagerUI_uploadSound(app->soundManagerUI, "sound.wav");
                    } else {
                        printf("No sound selected.\n");
                    }
                    break;
            }
            ExecutionEngine_startKeyScripts(app->engine, e.key.keysym.sym);
        }
    }
}

void Application_update(Application* app) {
    if (app->executing && !app->paused) {
        ExecutionEngine_step(app->engine, SDL_GetTicks());
    }
}

void Application_render(Application* app) {
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderClear(app->renderer);

    int winW, winH;
    SDL_GetWindowSize(app->window, &winW, &winH);

    app->menuRect = {0, 0, winW, 40};
    SDL_SetRenderDrawColor(app->renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(app->renderer, &app->menuRect);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(app->renderer, &app->menuRect);

    const char* buttons[] = {"New", "Save", "Load", "Start", "Stop", "Step", "Sprite", "Backdrop", "Sound"};
    int numButtons = 9;
    int x = 5;
    for (int i = 0; i < numButtons; i++) {
        SDL_Rect btnRect = {x, 5, 70, 30};
        SDL_SetRenderDrawColor(app->renderer, 220, 220, 220, 255);
        SDL_RenderFillRect(app->renderer, &btnRect);
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(app->renderer, &btnRect);

        if (app->spriteManagerUI->font) {
            SDL_Surface* surf = TTF_RenderText_Blended(app->spriteManagerUI->font, buttons[i], {0,0,0,255});
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(app->renderer, surf);
                SDL_Rect textRect = {x + (70 - surf->w)/2, 5 + (30 - surf->h)/2, surf->w, surf->h};
                SDL_RenderCopy(app->renderer, tex, NULL, &textRect);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }
        x += 80;
    }

    if (app->lastError[0] != '\0' && SDL_GetTicks() - app->errorTime < 5000) {
        SDL_Rect errorBar = {0, 40, winW, 30};
        SDL_SetRenderDrawColor(app->renderer, 255, 100, 100, 255);
        SDL_RenderFillRect(app->renderer, &errorBar);
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(app->renderer, &errorBar);

        if (app->spriteManagerUI->font) {
            SDL_Surface* surf = TTF_RenderText_Blended(app->spriteManagerUI->font, app->lastError, {255,255,255,255});
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(app->renderer, surf);
                SDL_Rect textRect = {10, 40 + (30 - surf->h)/2, surf->w, surf->h};
                SDL_RenderCopy(app->renderer, tex, NULL, &textRect);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }
    }

    app->varPanelRect = {0, 70, winW, 30};
    SDL_SetRenderDrawColor(app->renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(app->renderer, &app->varPanelRect);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(app->renderer, &app->varPanelRect);

    Project* proj = app->currentProject;
    if (app->spriteManagerUI->font) {
        int textX = 10;
        int textY = app->varPanelRect.y + (app->varPanelRect.h - 16) / 2;
        for (size_t i = 0; i < proj->globalVariables.size(); i++) {
            Variable* var = proj->globalVariables[i];
            string buffer;
            if (var->value.type == Value::VAL_NUMBER) {
                buffer = var->name + " = " + to_string(var->value.num);
            } else {
                buffer = var->name + " = " + var->value.str;
            }
            SDL_Surface* surf = TTF_RenderText_Blended(app->spriteManagerUI->font, buffer.c_str(), {0,0,0,255});
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(app->renderer, surf);
                SDL_Rect textRect = {textX, textY, surf->w, surf->h};
                SDL_RenderCopy(app->renderer, tex, NULL, &textRect);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
                textX += surf->w + 20;
            }
        }
    }

    int startY = 100;
    int paletteWidth = 200;
    int codeWidth = 400;
    int sceneWidth = winW - paletteWidth - codeWidth;
    int bottomHeight = 128;
    int rightPanelHeight = winH - startY - bottomHeight;
    int backdropPanelHeight = 150;
    int soundPanelHeight = 150;
    int sceneHeight = rightPanelHeight - backdropPanelHeight - soundPanelHeight;
    if (sceneHeight < 200) sceneHeight = 200;

    app->paletteRect = {0, startY, paletteWidth, sceneHeight};
    app->codeRect = {paletteWidth, startY, codeWidth, sceneHeight};
    app->sceneRect = {paletteWidth + codeWidth, startY, sceneWidth, sceneHeight};
    app->backdropPanelRect = {paletteWidth + codeWidth, startY + sceneHeight, sceneWidth, backdropPanelHeight};
    app->soundPanelRect = {paletteWidth + codeWidth, startY + sceneHeight + backdropPanelHeight, sceneWidth, soundPanelHeight};

    int penPanelWidth = 200;
    app->spritePanelRect = {0, winH - bottomHeight, winW - penPanelWidth, bottomHeight};
    app->penPanelRect = {winW - penPanelWidth, winH - bottomHeight, penPanelWidth, bottomHeight};

    app->blockPalette->rect = app->paletteRect;
    app->codeArea->rect = app->codeRect;
    app->spriteManagerUI->rect = app->spritePanelRect;
    app->penToolUI->rect = app->penPanelRect;
    app->backdropManagerUI->rect = app->backdropPanelRect;
    app->soundManagerUI->rect = app->soundPanelRect;

    if (proj->currentBackdrop >= 0 && proj->currentBackdrop < (int)proj->backdrops.size()) {
        Backdrop* b = proj->backdrops[proj->currentBackdrop];
        if (b->texture) {
            SDL_RenderCopy(app->renderer, b->texture, NULL, &app->sceneRect);
        } else {
            SDL_SetRenderDrawColor(app->renderer, 200, 200, 255, 255);
            SDL_RenderFillRect(app->renderer, &app->sceneRect);
        }
    } else {
        SDL_SetRenderDrawColor(app->renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(app->renderer, &app->sceneRect);
    }

    for (size_t i = 0; i < proj->sprites.size(); i++) {
        Sprite* s = proj->sprites[i];
        if (s->penCanvas) {
            SDL_RenderCopy(app->renderer, s->penCanvas, NULL, &app->sceneRect);
        }
    }

    vector<Sprite*> sortedSprites = proj->sprites;
    sort(sortedSprites.begin(), sortedSprites.end(), [](Sprite* a, Sprite* b) { return a->layer < b->layer; });

    Uint32 now = SDL_GetTicks();

    for (Sprite* s : sortedSprites) {
        if (!s->visible) continue;

        int screenX = app->sceneRect.x + app->sceneRect.w / 2 + (int)s->x;
        int screenY = app->sceneRect.y + app->sceneRect.h / 2 - (int)s->y;
        int spriteW = (int)(50 * s->size / 100.0f);
        int spriteH = (int)(50 * s->size / 100.0f);
        if (spriteW <= 0 || spriteH <= 0) continue;

        SDL_Rect destRect = {screenX - spriteW/2, screenY - spriteH/2, spriteW, spriteH};

        SDL_Texture* texture = NULL;
        if (!s->costumes.empty() && s->currentCostume < (int)s->costumes.size()) {
            texture = s->costumes[s->currentCostume]->texture;
        }

        if (texture) {
            Uint8 origR, origG, origB, origA;
            SDL_GetTextureColorMod(texture, &origR, &origG, &origB);
            SDL_GetTextureAlphaMod(texture, &origA);

            Uint8 r = origR, g = origG, b = origB;

            if (s->colorEffect != 0) {
                float factor = s->colorEffect / 200.0f;
                r = (Uint8)(origR * (1.0f - factor) + 255 * factor);
                g = (Uint8)(origG * (1.0f - factor));
                b = (Uint8)(origB * (1.0f - factor) + 255 * factor);
            }

            float brightFactor = s->brightnessEffect / 100.0f;
            r = (Uint8)(r * brightFactor);
            g = (Uint8)(g * brightFactor);
            b = (Uint8)(b * brightFactor);

            float satFactor = s->saturationEffect / 100.0f;
            Uint8 gray = (Uint8)(0.3f * r + 0.59f * g + 0.11f * b);
            r = (Uint8)(r * satFactor + gray * (1.0f - satFactor));
            g = (Uint8)(g * satFactor + gray * (1.0f - satFactor));
            b = (Uint8)(b * satFactor + gray * (1.0f - satFactor));

            SDL_SetTextureColorMod(texture, r, g, b);
            SDL_RenderCopy(app->renderer, texture, NULL, &destRect);
            SDL_SetTextureColorMod(texture, origR, origG, origB);
            SDL_SetTextureAlphaMod(texture, origA);
        } else {
            Uint8 r = (s->currentCostume * 50) % 256;
            Uint8 g = (s->currentCostume * 80) % 256;
            Uint8 b = (s->currentCostume * 110) % 256;
            float brightFactor = s->brightnessEffect / 100.0f;
            r = (Uint8)(r * brightFactor);
            g = (Uint8)(g * brightFactor);
            b = (Uint8)(b * brightFactor);
            SDL_SetRenderDrawColor(app->renderer, r, g, b, 255);
            SDL_RenderFillRect(app->renderer, &destRect);
            SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(app->renderer, &destRect);
        }

        if (app->speechFont) {
            if (!s->sayText.empty() && (s->sayUntil == 0 || now < s->sayUntil)) {
                int textW, textH;
                TTF_SizeText(app->speechFont, s->sayText.c_str(), &textW, &textH);
                int bubbleW = textW + 20;
                int bubbleH = textH + 10;
                int bubbleX = screenX - bubbleW/2;
                int bubbleY = screenY - spriteH/2 - bubbleH - 5;
                SDL_Rect bubbleRect = {bubbleX, bubbleY, bubbleW, bubbleH};
                SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(app->renderer, &bubbleRect);
                SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(app->renderer, &bubbleRect);
                SDL_Surface* surf = TTF_RenderText_Blended(app->speechFont, s->sayText.c_str(), {0,0,0,255});
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(app->renderer, surf);
                    SDL_Rect textRect = {bubbleX + 10, bubbleY + 5, surf->w, surf->h};
                    SDL_RenderCopy(app->renderer, tex, NULL, &textRect);
                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(surf);
                }
            }
            if (!s->thinkText.empty() && (s->thinkUntil == 0 || now < s->thinkUntil)) {
                int textW, textH;
                TTF_SizeText(app->speechFont, s->thinkText.c_str(), &textW, &textH);
                int bubbleW = textW + 20;
                int bubbleH = textH + 10;
                int bubbleX = screenX - bubbleW/2;
                int bubbleY = screenY - spriteH/2 - bubbleH - 5;
                SDL_Rect bubbleRect = {bubbleX, bubbleY, bubbleW, bubbleH};
                SDL_SetRenderDrawColor(app->renderer, 220, 220, 220, 255);
                SDL_RenderFillRect(app->renderer, &bubbleRect);
                SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(app->renderer, &bubbleRect);
                SDL_Surface* surf = TTF_RenderText_Blended(app->speechFont, s->thinkText.c_str(), {0,0,0,255});
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(app->renderer, surf);
                    SDL_Rect textRect = {bubbleX + 10, bubbleY + 5, surf->w, surf->h};
                    SDL_RenderCopy(app->renderer, tex, NULL, &textRect);
                    SDL_DestroyTexture(tex);
                    SDL_FreeSurface(surf);
                }
            }
        }
    }

    SpriteManagerUI_render(app->spriteManagerUI);
    PenToolUI_render(app->penToolUI);
    BackdropManagerUI_render(app->backdropManagerUI);
    SoundManagerUI_render(app->soundManagerUI);
    BlockPaletteUI_render(app->blockPalette);
    CodeAreaUI_render(app->codeArea);

    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(app->renderer, paletteWidth, startY, paletteWidth, winH - bottomHeight);
    SDL_RenderDrawLine(app->renderer, paletteWidth + codeWidth, startY, paletteWidth + codeWidth, winH - bottomHeight);
    SDL_RenderDrawLine(app->renderer, 0, winH - bottomHeight, winW, winH - bottomHeight);
    SDL_RenderDrawLine(app->renderer, paletteWidth + codeWidth, startY + sceneHeight, winW, startY + sceneHeight);
    SDL_RenderDrawLine(app->renderer, paletteWidth + codeWidth, startY + sceneHeight + backdropPanelHeight, winW, startY + sceneHeight + backdropPanelHeight);
    SDL_RenderDrawLine(app->renderer, winW - penPanelWidth, winH - bottomHeight, winW - penPanelWidth, winH);

    SDL_RenderPresent(app->renderer);
}
// ExecutionEngine function
void ExecutionEngine_step(ExecutionEngine* eng, Uint32 currentTime) {
    int stepsThisFrame = 0;
    int winW, winH;
    SDL_GetWindowSize(gWindow, &winW, &winH);
    SDL_Renderer* renderer = SDL_GetRenderer(gWindow);
    int paletteWidth = 200;
    int codeWidth = 400;
    int sceneWidth = winW - paletteWidth - codeWidth;
    int bottomHeight = 128;
    int rightPanelHeight = winH - 40 - bottomHeight;
    int backdropPanelHeight = 150;
    int soundPanelHeight = 150;
    int sceneHeight = rightPanelHeight - backdropPanelHeight - soundPanelHeight;
    if (sceneHeight < 200) sceneHeight = 200;
    SDL_Rect stageRect = {paletteWidth + codeWidth, 40, sceneWidth, sceneHeight};

    for (int i = 0; i < (int)eng->contexts.size(); i++) {
        ExecutionContext* ctx = eng->contexts[i];

        if (ctx->waitingForAnswer) {
            if (gApp && gApp->answerReady) {
                eng->project->answer = gApp->pendingAnswer;
                gApp->answerReady = false;
                ctx->waitingForAnswer = false;
                ctx->pc++;
            }
            continue;
        }

        if (ctx->waitUntil > currentTime) continue;

        if (ctx->waitingForSoundChannel != -1) {
            if (!Mix_Playing(ctx->waitingForSoundChannel)) {
                ctx->pc++;
                ctx->waitingForSoundChannel = -1;
            }
            continue;
        }

        if (ctx->pc < 0) continue;
        Sprite* sprite = eng->project->sprites[ctx->spriteId];
        if (ctx->scriptId >= (int)sprite->scripts.size()) {
            ExecutionEngine_removeContext(eng, i);
            i--;
            continue;
        }
        Script* script = sprite->scripts[ctx->scriptId];
        if (ctx->pc >= (int)script->blocks.size()) {
            ExecutionEngine_removeContext(eng, i);
            i--;
            continue;
        }
        Block* block = script->blocks[ctx->pc];

        switch (block->type) {
            case BLOCK_MOVE: {
                float steps = block->numParam1;
                float rad = sprite->direction * (float)M_PI / 180.0f;
                float newX = sprite->x + steps * cosf(rad);
                float newY = sprite->y + steps * sinf(rad);
                if (newX > 240) newX = 240;
                if (newX < -240) newX = -240;
                if (newY > 180) newY = 180;
                if (newY < -180) newY = -180;

                if (sprite->penDown) {
                    int x1 = stageRect.x + stageRect.w/2 + (int)sprite->x;
                    int y1 = stageRect.y + stageRect.h/2 - (int)sprite->y;
                    int x2 = stageRect.x + stageRect.w/2 + (int)newX;
                    int y2 = stageRect.y + stageRect.h/2 - (int)newY;
                    SDL_Color color = hslToRgb(sprite->penHue, sprite->penSaturation, sprite->penBrightness);
                    drawLineOnCanvas(renderer, sprite->penCanvas, x1, y1, x2, y2, color, sprite->penSize);
                }
                sprite->x = newX;
                sprite->y = newY;
                ctx->pc++;
                break;
            }
            case BLOCK_TURN:
                sprite->direction += block->numParam1;
                ctx->pc++;
                break;
            case BLOCK_GOTO:
                sprite->x = block->numParam1;
                if (sprite->x > 240) sprite->x = 240;
                if (sprite->x < -240) sprite->x = -240;
                sprite->y = block->numParam2;
                if (sprite->y > 180) sprite->y = 180;
                if (sprite->y < -180) sprite->y = -180;
                ctx->pc++;
                break;
            case BLOCK_CHANGE_X: {
                float oldX = sprite->x;
                sprite->x += block->numParam1;
                if (sprite->x > 240) sprite->x = 240;
                if (sprite->x < -240) sprite->x = -240;
                if (sprite->penDown) {
                    int x1 = stageRect.x + stageRect.w/2 + (int)oldX;
                    int y1 = stageRect.y + stageRect.h/2 - (int)sprite->y;
                    int x2 = stageRect.x + stageRect.w/2 + (int)sprite->x;
                    int y2 = stageRect.y + stageRect.h/2 - (int)sprite->y;
                    SDL_Color color = hslToRgb(sprite->penHue, sprite->penSaturation, sprite->penBrightness);
                    drawLineOnCanvas(renderer, sprite->penCanvas, x1, y1, x2, y2, color, sprite->penSize);
                }
                ctx->pc++;
                break;
            }
            case BLOCK_CHANGE_Y: {
                float oldY = sprite->y;
                sprite->y += block->numParam1;
                if (sprite->y > 180) sprite->y = 180;
                if (sprite->y < -180) sprite->y = -180;
                if (sprite->penDown) {
                    int x1 = stageRect.x + stageRect.w/2 + (int)sprite->x;
                    int y1 = stageRect.y + stageRect.h/2 - (int)oldY;
                    int x2 = stageRect.x + stageRect.w/2 + (int)sprite->x;
                    int y2 = stageRect.y + stageRect.h/2 - (int)sprite->y;
                    SDL_Color color = hslToRgb(sprite->penHue, sprite->penSaturation, sprite->penBrightness);
                    drawLineOnCanvas(renderer, sprite->penCanvas, x1, y1, x2, y2, color, sprite->penSize);
                }
                ctx->pc++;
                break;
            }
            case BLOCK_SET_DIRECTION:
                sprite->direction = block->numParam1;
                ctx->pc++;
                break;
            case BLOCK_GO_TO_RANDOM: {
                float newX = (rand() / (float)RAND_MAX) * 480 - 240;
                float newY = (rand() / (float)RAND_MAX) * 360 - 180;
                if (sprite->penDown) {
                    int x1 = stageRect.x + stageRect.w/2 + (int)sprite->x;
                    int y1 = stageRect.y + stageRect.h/2 - (int)sprite->y;
                    int x2 = stageRect.x + stageRect.w/2 + (int)newX;
                    int y2 = stageRect.y + stageRect.h/2 - (int)newY;
                    SDL_Color color = hslToRgb(sprite->penHue, sprite->penSaturation, sprite->penBrightness);
                    drawLineOnCanvas(renderer, sprite->penCanvas, x1, y1, x2, y2, color, sprite->penSize);
                }
                sprite->x = newX;
                sprite->y = newY;
                ctx->pc++;
                break;
            }
            case BLOCK_GO_TO_MOUSE: {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                float newX = mouseX - (stageRect.x + stageRect.w/2);
                float newY = (stageRect.y + stageRect.h/2) - mouseY;
                if (newX < -240) newX = -240;
                if (newX > 240) newX = 240;
                if (newY < -180) newY = -180;
                if (newY > 180) newY = 180;
                if (sprite->penDown) {
                    int x1 = stageRect.x + stageRect.w/2 + (int)sprite->x;
                    int y1 = stageRect.y + stageRect.h/2 - (int)sprite->y;
                    int x2 = stageRect.x + stageRect.w/2 + (int)newX;
                    int y2 = stageRect.y + stageRect.h/2 - (int)newY;
                    SDL_Color color = hslToRgb(sprite->penHue, sprite->penSaturation, sprite->penBrightness);
                    drawLineOnCanvas(renderer, sprite->penCanvas, x1, y1, x2, y2, color, sprite->penSize);
                }
                sprite->x = newX;
                sprite->y = newY;
                ctx->pc++;
                break;
            }
            case BLOCK_IF_ON_EDGE_BOUNCE: {
                bool bounced = false;
                if (sprite->x >= 240) {
                    sprite->x = 240;
                    sprite->direction = 180 - sprite->direction;
                    bounced = true;
                } else if (sprite->x <= -240) {
                    sprite->x = -240;
                    sprite->direction = 180 - sprite->direction;
                    bounced = true;
                }
                if (sprite->y >= 180) {
                    sprite->y = 180;
                    sprite->direction = -sprite->direction;
                    bounced = true;
                } else if (sprite->y <= -180) {
                    sprite->y = -180;
                    sprite->direction = -sprite->direction;
                    bounced = true;
                }
                while (sprite->direction < 0) sprite->direction += 360;
                while (sprite->direction >= 360) sprite->direction -= 360;
                ctx->pc++;
                break;
            }
            case BLOCK_SAY: {
                if (!block->strParam.empty()) {
                    sprite->sayText = block->strParam;
                    sprite->thinkText.clear();
                    if (block->numParam1 > 0) {
                        sprite->sayUntil = currentTime + (Uint32)(block->numParam1 * 1000);
                    } else {
                        sprite->sayUntil = 0;
                    }
                } else {
                    sprite->sayText.clear();
                }
                ctx->pc++;
                break;
            }
            case BLOCK_THINK: {
                if (!block->strParam.empty()) {
                    sprite->thinkText = block->strParam;
                    sprite->sayText.clear();
                    if (block->numParam1 > 0) {
                        sprite->thinkUntil = currentTime + (Uint32)(block->numParam1 * 1000);
                    } else {
                        sprite->thinkUntil = 0;
                    }
                } else {
                    sprite->thinkText.clear();
                }
                ctx->pc++;
                break;
            }
            case BLOCK_SWITCH_COSTUME:
                if (!block->strParam.empty()) {
                    for (size_t j = 0; j < sprite->costumes.size(); j++) {
                        if (sprite->costumes[j]->name == block->strParam) {
                            sprite->currentCostume = j;
                            break;
                        }
                    }
                } else {
                    sprite->currentCostume = (int)block->numParam1;
                }
                ctx->pc++;
                break;
            case BLOCK_NEXT_COSTUME:
                if (!sprite->costumes.empty()) {
                    sprite->currentCostume = (sprite->currentCostume + 1) % sprite->costumes.size();
                }
                ctx->pc++;
                break;
            case BLOCK_SWITCH_BACKDROP:
                if (!block->strParam.empty()) {
                    for (size_t j = 0; j < eng->project->backdrops.size(); j++) {
                        if (eng->project->backdrops[j]->name == block->strParam) {
                            eng->project->currentBackdrop = j;
                            break;
                        }
                    }
                }
                ctx->pc++;
                break;
            case BLOCK_NEXT_BACKDROP:
                if (!eng->project->backdrops.empty()) {
                    eng->project->currentBackdrop = (eng->project->currentBackdrop + 1) % eng->project->backdrops.size();
                }
                ctx->pc++;
                break;
            case BLOCK_CHANGE_SIZE:
                sprite->size += block->numParam1;
                ctx->pc++;
                break;
            case BLOCK_SET_SIZE:
                sprite->size = block->numParam1;
                ctx->pc++;
                break;
            case BLOCK_CHANGE_COLOR:
                sprite->colorEffect += block->numParam1;
                if (sprite->colorEffect < 0) sprite->colorEffect = 0;
                if (sprite->colorEffect > 200) sprite->colorEffect = 200;
                ctx->pc++;
                break;
            case BLOCK_SET_COLOR:
                sprite->colorEffect = block->numParam1;
                if (sprite->colorEffect < 0) sprite->colorEffect = 0;
                if (sprite->colorEffect > 200) sprite->colorEffect = 200;
                ctx->pc++;
                break;
            case BLOCK_CLEAR_EFFECTS:
                sprite->colorEffect = 0;
                sprite->brightnessEffect = 100;
                sprite->saturationEffect = 100;
                ctx->pc++;
                break;
            case BLOCK_SHOW:
                sprite->visible = 1;
                ctx->pc++;
                break;
            case BLOCK_HIDE:
                sprite->visible = 0;
                ctx->pc++;
                break;
            case BLOCK_GO_TO_LAYER:
                if (block->strParam == "front") {
                    sprite->layer = 1000;
                } else if (block->strParam == "end") {
                    sprite->layer = -1000;
                }
                ctx->pc++;
                break;
            case BLOCK_CHANGE_LAYER:
                sprite->layer += (int)block->numParam1;
                ctx->pc++;
                break;
            case BLOCK_CHANGE_BRIGHTNESS:
                sprite->brightnessEffect += block->numParam1;
                if (sprite->brightnessEffect < 0) sprite->brightnessEffect = 0;
                if (sprite->brightnessEffect > 100) sprite->brightnessEffect = 100;
                ctx->pc++;
                break;
            case BLOCK_SET_BRIGHTNESS:
                sprite->brightnessEffect = block->numParam1;
                if (sprite->brightnessEffect < 0) sprite->brightnessEffect = 0;
                if (sprite->brightnessEffect > 100) sprite->brightnessEffect = 100;
                ctx->pc++;
                break;
            case BLOCK_CHANGE_SATURATION:
                sprite->saturationEffect += block->numParam1;
                if (sprite->saturationEffect < 0) sprite->saturationEffect = 0;
                if (sprite->saturationEffect > 100) sprite->saturationEffect = 100;
                ctx->pc++;
                break;
            case BLOCK_SET_SATURATION:
                sprite->saturationEffect = block->numParam1;
                if (sprite->saturationEffect < 0) sprite->saturationEffect = 0;
                if (sprite->saturationEffect > 100) sprite->saturationEffect = 100;
                ctx->pc++;
                break;
            case BLOCK_PLAY_SOUND: {
                const char* soundName = block->strParam.c_str();
                if (soundName) {
                    int idx = findSoundByName(eng->project, soundName);
                    if (idx >= 0) {
                        Sound* snd = eng->project->sounds[idx];
                        if (snd->chunk && !snd->muted) {
                            int volume = (int)(snd->volume * MIX_MAX_VOLUME / 100.0f);
                            Mix_VolumeChunk(snd->chunk, volume);
                            Mix_PlayChannel(-1, snd->chunk, 0);
                        }
                    }
                }
                ctx->pc++;
                break;
            }
            case BLOCK_PLAY_SOUND_UNTIL_DONE: {
                const char* soundName = block->strParam.c_str();
                if (soundName) {
                    int idx = findSoundByName(eng->project, soundName);
                    if (idx >= 0) {
                        Sound* snd = eng->project->sounds[idx];
                        if (snd->chunk && !snd->muted) {
                            int volume = (int)(snd->volume * MIX_MAX_VOLUME / 100.0f);
                            Mix_VolumeChunk(snd->chunk, volume);
                            int channel = Mix_PlayChannel(-1, snd->chunk, 0);
                            if (channel >= 0) {
                                ctx->waitingForSoundChannel = channel;
                                break;
                            }
                        }
                    }
                }
                ctx->pc++;
                break;
            }
            case BLOCK_STOP_ALL_SOUNDS:
                Mix_HaltChannel(-1);
                ctx->pc++;
                break;
            case BLOCK_CHANGE_VOLUME: {
                const char* soundName = block->strParam.c_str();
                if (soundName) {
                    int idx = findSoundByName(eng->project, soundName);
                    if (idx >= 0) {
                        Sound* snd = eng->project->sounds[idx];
                        snd->volume += block->numParam1;
                        if (snd->volume < 0) snd->volume = 0;
                        if (snd->volume > 100) snd->volume = 100;
                    }
                }
                ctx->pc++;
                break;
            }
            case BLOCK_SET_VOLUME: {
                const char* soundName = block->strParam.c_str();
                if (soundName) {
                    int idx = findSoundByName(eng->project, soundName);
                    if (idx >= 0) {
                        Sound* snd = eng->project->sounds[idx];
                        snd->volume = block->numParam1;
                        if (snd->volume < 0) snd->volume = 0;
                        if (snd->volume > 100) snd->volume = 100;
                    }
                }
                ctx->pc++;
                break;
            }
            case BLOCK_WAIT:
                ctx->waitUntil = currentTime + (Uint32)(block->numParam1 * 1000);
                break;
            case BLOCK_REPEAT: {
                LoopInfo loop;
                loop.start = ctx->pc + 1;
                loop.end = block->bodyEnd;
                loop.count = (int)block->numParam1;
                ctx->loopStack.push_back(loop);
                ctx->pc = loop.start;
                break;
            }
            case BLOCK_FOREVER: {
                LoopInfo loop;
                loop.start = ctx->pc + 1;
                loop.end = block->bodyEnd;
                loop.count = -1;
                ctx->loopStack.push_back(loop);
                ctx->pc = loop.start;
                break;
            }
            case BLOCK_IF: {
                float cond = 0;
                if (!block->children.empty()) {
                    Value condVal = evaluateBlock(block->children[0], ctx, eng->project);
                    cond = value_to_number(condVal);
                } else {
                    cond = block->numParam1; // برای سازگاری با عقب
                }
                if (cond != 0) {
                    ctx->pc++;
                } else {
                    ctx->pc = block->bodyEnd;
                }
                break;
            }
            case BLOCK_IF_ELSE: {
                float cond = 0;
                if (!block->children.empty()) {
                    Value condVal = evaluateBlock(block->children[0], ctx, eng->project);
                    cond = value_to_number(condVal);
                } else {
                    cond = block->numParam1;
                }
                IfInfo info;
                info.elseStart = block->elseStart;
                info.endifPos = block->bodyEnd;
                info.trueBranch = (cond != 0);
                ctx->ifStack.push_back(info);
                if (cond != 0) {
                    ctx->pc++;
                } else {
                    ctx->pc = block->elseStart;
                }
                break;
            }
            case BLOCK_WAIT_UNTIL: {
                float cond = 0;
                if (!block->children.empty()) {
                    Value condVal = evaluateBlock(block->children[0], ctx, eng->project);
                    cond = value_to_number(condVal);
                }
                if (cond != 0) {
                    ctx->pc++;
                }
                break;
            }
            case BLOCK_REPEAT_UNTIL: {
                float cond = 0;
                if (!block->children.empty()) {
                    Value condVal = evaluateBlock(block->children[0], ctx, eng->project);
                    cond = value_to_number(condVal);
                }
                if (cond != 0) {
                    if (!ctx->loopStack.empty() && ctx->loopStack.back().start == ctx->pc) {
                        ctx->loopStack.pop_back();
                    }
                    ctx->pc = block->bodyEnd;
                } else {
                    bool alreadyInLoop = false;
                    for (const auto& l : ctx->loopStack) {
                        if (l.start == ctx->pc) {
                            alreadyInLoop = true;
                            break;
                        }
                    }
                    if (!alreadyInLoop) {
                        LoopInfo loop;
                        loop.start = ctx->pc;
                        loop.end = block->bodyEnd;
                        loop.count = -1;
                        ctx->loopStack.push_back(loop);
                    }
                    ctx->pc = ctx->pc + 1;
                }
                break;
            }
            case BLOCK_STOP_ALL:
                eng->contexts.clear();
                return;
            case BLOCK_BROADCAST:
            {
                const char* msg = block->strParam.c_str();
                if (msg) {
                    for (size_t s = 0; s < eng->project->sprites.size(); s++) {
                        Sprite* targetSprite = eng->project->sprites[s];
                        for (size_t t = 0; t < targetSprite->scripts.size(); t++) {
                            Script* targetScript = targetSprite->scripts[t];
                            if (!targetScript->blocks.empty() && targetScript->blocks[0]->type == BLOCK_WHEN_I_RECEIVE) {
                                if (targetScript->blocks[0]->strParam == msg) {
                                    ExecutionEngine_addContext(eng, s, t);
                                }
                            }
                        }
                    }
                }
                ctx->pc++;
                break;
            }
            case BLOCK_BROADCAST_AND_WAIT:
            {
                const char* msg = block->strParam.c_str();
                if (msg) {
                    ctx->childrenLeft = 0;
                    ctx->waitingForChildren = true;
                    for (size_t s = 0; s < eng->project->sprites.size(); s++) {
                        Sprite* targetSprite = eng->project->sprites[s];
                        for (size_t t = 0; t < targetSprite->scripts.size(); t++) {
                            Script* targetScript = targetSprite->scripts[t];
                            if (!targetScript->blocks.empty() && targetScript->blocks[0]->type == BLOCK_WHEN_I_RECEIVE) {
                                if (targetScript->blocks[0]->strParam == msg) {
                                    ExecutionEngine_addChildContext(eng, s, t, ctx);
                                }
                            }
                        }
                    }
                }
                if (ctx->childrenLeft == 0) {
                    ctx->pc++;
                    ctx->waitingForChildren = false;
                }
                break;
            }
            case BLOCK_SET_VARIABLE: {
                if (!block->strParam.empty() && !block->children.empty()) {
                    Value val = evaluateBlock(block->children[0], ctx, eng->project);
                    setVariable(eng->project, block->strParam, val);
                }
                ctx->pc++;
                break;
            }
            case BLOCK_CHANGE_VARIABLE: {
                if (!block->strParam.empty() && !block->children.empty()) {
                    Value deltaVal = evaluateBlock(block->children[0], ctx, eng->project);
                    float delta = value_to_number(deltaVal);
                    Value cur = getVariable(eng->project, block->strParam);
                    float curNum = value_to_number(cur);
                    curNum += delta;
                    Value newVal = make_number(curNum);
                    setVariable(eng->project, block->strParam, newVal);
                }
                ctx->pc++;
                break;
            }
            case BLOCK_VARIABLE_GET:
                ctx->pc++;
                break;
            case BLOCK_NUMBER:
            case BLOCK_STRING:
            case BLOCK_ADD:
            case BLOCK_SUBTRACT:
            case BLOCK_MULTIPLY:
            case BLOCK_DIVIDE:
            case BLOCK_RANDOM:
            case BLOCK_LT:
            case BLOCK_GT:
            case BLOCK_EQUALS:
            case BLOCK_AND:
            case BLOCK_OR:
            case BLOCK_NOT:
            case BLOCK_JOIN:
            case BLOCK_LETTER_OF:
            case BLOCK_LENGTH:
            case BLOCK_MOD:
            case BLOCK_ROUND:
            case BLOCK_ABS:
            case BLOCK_SQRT:
            case BLOCK_SIN:
            case BLOCK_COS:
            case BLOCK_TAN:
            case BLOCK_ASIN:
            case BLOCK_ACOS:
            case BLOCK_ATAN:
            case BLOCK_LN:
            case BLOCK_LOG:
            case BLOCK_POW:
            case BLOCK_TOUCHING_EDGE:
            case BLOCK_MOUSE_X:
            case BLOCK_MOUSE_Y:
            case BLOCK_KEY_PRESSED:
            case BLOCK_COSTUME_NUMBER:
            case BLOCK_COSTUME_NAME:
            case BLOCK_BACKDROP_NUMBER:
            case BLOCK_BACKDROP_NAME:
            case BLOCK_SIZE:
            case BLOCK_TOUCHING_MOUSEPOINTER:
            case BLOCK_TOUCHING_SPRITE:
            case BLOCK_TOUCHING_COLOR:
            case BLOCK_COLOR_TOUCHING_COLOR:
            case BLOCK_DISTANCE_TO:
            case BLOCK_ANSWER:
            case BLOCK_MOUSE_DOWN:
            case BLOCK_TIMER:
                ctx->pc++;
                break;
            case BLOCK_ASK_AND_WAIT:
                SDL_StartTextInput();
                ctx->waitingForAnswer = true;
                break;
            case BLOCK_SET_DRAG_MODE:
                if (block->strParam == "draggable") {
                    sprite->draggable = true;
                } else if (block->strParam == "not draggable") {
                    sprite->draggable = false;
                }
                ctx->pc++;
                break;
            case BLOCK_RESET_TIMER:
                eng->project->timerStart = SDL_GetTicks();
                ctx->pc++;
                break;
            case BLOCK_PEN_DOWN:
                sprite->penDown = true;
                ctx->pc++;
                break;
            case BLOCK_PEN_UP:
                sprite->penDown = false;
                ctx->pc++;
                break;
            case BLOCK_SET_PEN_COLOR:
                sprite->penHue = block->numParam1;
                if (sprite->penHue < 0) sprite->penHue = 0;
                if (sprite->penHue > 200) sprite->penHue = 200;
                ctx->pc++;
                break;
            case BLOCK_CHANGE_PEN_COLOR:
                sprite->penHue += block->numParam1;
                while (sprite->penHue < 0) sprite->penHue += 200;
                while (sprite->penHue > 200) sprite->penHue -= 200;
                ctx->pc++;
                break;
            case BLOCK_SET_PEN_BRIGHTNESS:
                sprite->penBrightness = block->numParam1;
                if (sprite->penBrightness < 0) sprite->penBrightness = 0;
                if (sprite->penBrightness > 100) sprite->penBrightness = 100;
                ctx->pc++;
                break;
            case BLOCK_CHANGE_PEN_BRIGHTNESS:
                sprite->penBrightness += block->numParam1;
                if (sprite->penBrightness < 0) sprite->penBrightness = 0;
                if (sprite->penBrightness > 100) sprite->penBrightness = 100;
                ctx->pc++;
                break;
            case BLOCK_SET_PEN_SATURATION:
                sprite->penSaturation = block->numParam1;
                if (sprite->penSaturation < 0) sprite->penSaturation = 0;
                if (sprite->penSaturation > 100) sprite->penSaturation = 100;
                ctx->pc++;
                break;
            case BLOCK_CHANGE_PEN_SATURATION:
                sprite->penSaturation += block->numParam1;
                if (sprite->penSaturation < 0) sprite->penSaturation = 0;
                if (sprite->penSaturation > 100) sprite->penSaturation = 100;
                ctx->pc++;
                break;
            case BLOCK_SET_PEN_SIZE:
                sprite->penSize = (int)block->numParam1;
                if (sprite->penSize < 1) sprite->penSize = 1;
                ctx->pc++;
                break;
            case BLOCK_CHANGE_PEN_SIZE:
                sprite->penSize += (int)block->numParam1;
                if (sprite->penSize < 1) sprite->penSize = 1;
                ctx->pc++;
                break;
            case BLOCK_ERASE_ALL: {
                SDL_SetRenderTarget(renderer, sprite->penCanvas);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                SDL_RenderClear(renderer);
                SDL_SetRenderTarget(renderer, NULL);
                ctx->pc++;
                break;
            }
            case BLOCK_STAMP: {
                if (!sprite->costumes.empty() && sprite->currentCostume < (int)sprite->costumes.size()) {
                    Costume* costume = sprite->costumes[sprite->currentCostume];
                    if (costume->texture) {
                        int screenX = stageRect.x + stageRect.w/2 + (int)sprite->x;
                        int screenY = stageRect.y + stageRect.h/2 - (int)sprite->y;
                        int stampW = (int)(50 * sprite->size / 100.0f);
                        int stampH = (int)(50 * sprite->size / 100.0f);
                        SDL_Rect destRect = {screenX - stampW/2, screenY - stampH/2, stampW, stampH};
                        SDL_SetRenderTarget(renderer, sprite->penCanvas);
                        SDL_RenderCopy(renderer, costume->texture, NULL, &destRect);
                        SDL_SetRenderTarget(renderer, NULL);
                    }
                }
                ctx->pc++;
                break;
            }
            case BLOCK_ELSE:
                if (!ctx->ifStack.empty()) {
                    IfInfo* top = &ctx->ifStack.back();
                    if (top->trueBranch) {
                        ctx->pc = top->endifPos;
                        ctx->ifStack.pop_back();
                    } else {
                        ctx->pc++;
                    }
                } else {
                    ctx->pc++;
                }
                break;
            case BLOCK_ENDIF:
                if (!ctx->ifStack.empty()) {
                    ctx->ifStack.pop_back();
                }
                ctx->pc++;
                break;
            case BLOCK_ENDLOOP:
                ctx->pc++;
                break;
            default:
                ctx->pc++;
                break;
        }

        stepsThisFrame++;
        if (stepsThisFrame > MAX_STEPS_PER_FRAME) {
            setError(gApp, "⚠️ حلقه بی‌نهایت تشخیص داده شد! اجرا متوقف شد.");
            eng->contexts.clear();
            return;
        }

        if (block->type != BLOCK_WAIT && block->type != BLOCK_WAIT_UNTIL && block->type != BLOCK_ASK_AND_WAIT) {
            while (!ctx->loopStack.empty()) {
                LoopInfo* top = &ctx->loopStack.back();
                if (ctx->pc == top->end) {
                    if (top->count == -1) {
                        ctx->pc = top->start;
                        break;
                    } else if (top->count > 0) {
                        top->count--;
                        if (top->count > 0) {
                            ctx->pc = top->start;
                            break;
                        } else {
                            ctx->loopStack.pop_back();
                        }
                    } else {
                        ctx->loopStack.pop_back();
                    }
                } else {
                    break;
                }
            }
        }
    }

    if (eng->stepMode) {
        eng->stepMode = false;
        return;
    }
}

void ExecutionEngine_run(ExecutionEngine* eng) {
    eng->contexts.clear();
    for (size_t i = 0; i < eng->project->sprites.size(); i++) {
        Sprite* sprite = eng->project->sprites[i];
        for (size_t j = 0; j < sprite->scripts.size(); j++) {
            Script* script = sprite->scripts[j];
            if (!script->blocks.empty() && script->blocks[0]->type == BLOCK_WHEN_FLAG_CLICKED) {
                ExecutionEngine_addContext(eng, i, j);
            }
        }
    }
}

void ExecutionEngine_stop(ExecutionEngine* eng) {
    for (ExecutionContext* ctx : eng->contexts) {
        delete ctx;
    }
    eng->contexts.clear();
}

void ExecutionEngine_startKeyScripts(ExecutionEngine* eng, SDL_Keycode key) {
    const char* keyName = NULL;
    switch (key) {
        case SDLK_SPACE: keyName = "space"; break;
        case SDLK_UP: keyName = "up arrow"; break;
        case SDLK_DOWN: keyName = "down arrow"; break;
        case SDLK_LEFT: keyName = "left arrow"; break;
        case SDLK_RIGHT: keyName = "right arrow"; break;
        case SDLK_a: keyName = "a"; break;
        case SDLK_b: keyName = "b"; break;
        case SDLK_c: keyName = "c"; break;
        case SDLK_d: keyName = "d"; break;
        case SDLK_e: keyName = "e"; break;
        case SDLK_f: keyName = "f"; break;
        case SDLK_g: keyName = "g"; break;
        case SDLK_h: keyName = "h"; break;
        case SDLK_i: keyName = "i"; break;
        case SDLK_j: keyName = "j"; break;
        case SDLK_k: keyName = "k"; break;
        case SDLK_l: keyName = "l"; break;
        case SDLK_m: keyName = "m"; break;
        case SDLK_n: keyName = "n"; break;
        case SDLK_o: keyName = "o"; break;
        case SDLK_p: keyName = "p"; break;
        case SDLK_q: keyName = "q"; break;
        case SDLK_r: keyName = "r"; break;
        case SDLK_s: keyName = "s"; break;
        case SDLK_t: keyName = "t"; break;
        case SDLK_u: keyName = "u"; break;
        case SDLK_v: keyName = "v"; break;
        case SDLK_w: keyName = "w"; break;
        case SDLK_x: keyName = "x"; break;
        case SDLK_y: keyName = "y"; break;
        case SDLK_z: keyName = "z"; break;
        case SDLK_0: keyName = "0"; break;
        case SDLK_1: keyName = "1"; break;
        case SDLK_2: keyName = "2"; break;
        case SDLK_3: keyName = "3"; break;
        case SDLK_4: keyName = "4"; break;
        case SDLK_5: keyName = "5"; break;
        case SDLK_6: keyName = "6"; break;
        case SDLK_7: keyName = "7"; break;
        case SDLK_8: keyName = "8"; break;
        case SDLK_9: keyName = "9"; break;
        default: return;
    }
    for (size_t i = 0; i < eng->project->sprites.size(); i++) {
        Sprite* sprite = eng->project->sprites[i];
        for (size_t j = 0; j < sprite->scripts.size(); j++) {
            Script* script = sprite->scripts[j];
            if (!script->blocks.empty() && script->blocks[0]->type == BLOCK_WHEN_KEY_PRESSED) {
                if (script->blocks[0]->strParam == keyName) {
                    ExecutionEngine_addContext(eng, i, j);
                }
            }
        }
    }
}

int ExecutionEngine_startSpriteClickScripts(ExecutionEngine* eng, int mouseX, int mouseY, SDL_Rect stageRect) {
    int topSprite = -1;
    int topLayer = -1000000;
    for (size_t i = 0; i < eng->project->sprites.size(); i++) {
        Sprite* s = eng->project->sprites[i];
        if (!s->visible) continue;
        int screenX = stageRect.x + stageRect.w/2 + (int)s->x;
        int screenY = stageRect.y + stageRect.h/2 - (int)s->y;
        int spriteW = (int)(50 * s->size / 100.0f);
        int spriteH = (int)(50 * s->size / 100.0f);
        SDL_Rect rect = {screenX - spriteW/2, screenY - spriteH/2, spriteW, spriteH};
        if (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h) {
            if (s->layer > topLayer) {
                topLayer = s->layer;
                topSprite = i;
            }
        }
    }
    if (topSprite >= 0) {
        Sprite* sprite = eng->project->sprites[topSprite];
        for (size_t j = 0; j < sprite->scripts.size(); j++) {
            Script* script = sprite->scripts[j];
            if (!script->blocks.empty() && script->blocks[0]->type == BLOCK_WHEN_SPRITE_CLICKED) {
                ExecutionEngine_addContext(eng, topSprite, j);
            }
        }
    }
    return topSprite;
}

bool findBlockAt(CodeAreaUI* ui, int mouseX, int mouseY, int* outScriptIndex, int* outBlockIndex) {
    if (ui->selectedSpriteIndex < 0 || ui->selectedSpriteIndex >= (int)ui->project->sprites.size())
        return false;
    Sprite* sprite = ui->project->sprites[ui->selectedSpriteIndex];
    int scriptSpacing = 10;
    int scriptWidth = 180;
    int blockHeight = 40;
    int blockGap = 2;

    for (size_t i = 0; i < sprite->scripts.size(); i++) {
        int scriptX = ui->rect.x + 10 + i * (scriptWidth + scriptSpacing) - ui->scrollX;
        if (mouseX < scriptX || mouseX > scriptX + scriptWidth - 10) continue;
        for (size_t j = 0; j < sprite->scripts[i]->blocks.size(); j++) {
            int blockY = ui->rect.y + 10 - ui->scrollY + j * (blockHeight + blockGap);
            if (mouseY >= blockY && mouseY <= blockY + blockHeight) {
                *outScriptIndex = i;
                *outBlockIndex = j;
                return true;
            }
        }
    }
    return false;
}
// SpriteManagerUI functions
// ============================================================================
SpriteManagerUI* SpriteManagerUI_create(SDL_Renderer* ren, Project* proj) {
    SpriteManagerUI* ui = new SpriteManagerUI;
    ui->renderer = ren;
    ui->project = proj;
    ui->selectedSpriteIndex = -1;
    ui->scrollOffset = 0;
    ui->rect = {0, 0, 0, 0};
    char* fontPath = findFontFile("arial.ttf");
    ui->font = TTF_OpenFont(fontPath, 12);
    free(fontPath);
    if (!ui->font) {
        printf("TTF_OpenFont error: could not find arial.ttf\n");
    }
    ui->editingName = -1;
    ui->editingField = 0;
    ui->editingSpriteIndex = -1;
    ui->lastClickTime = 0;
    ui->lastClickedIndex = -1;
    return ui;
}

void SpriteManagerUI_destroy(SpriteManagerUI* ui) {
    if (ui->font) TTF_CloseFont(ui->font);
    delete ui;
}

void SpriteManagerUI_render(SpriteManagerUI* ui) {
    SDL_SetRenderDrawColor(ui->renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(ui->renderer, &ui->rect);
    SDL_SetRenderDrawColor(ui->renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(ui->renderer, &ui->rect);

    int iconSize = 50;
    int spacing = 70;
    int startX = ui->rect.x + 10;
    int startY = ui->rect.y + 10 - ui->scrollOffset;

    for (size_t i = 0; i < ui->project->sprites.size(); i++) {
        Sprite* s = ui->project->sprites[i];
        int x = startX + i * spacing;
        int y = startY;
        if (y + iconSize + 30 > ui->rect.y + ui->rect.h || y < ui->rect.y) {
            continue;
        }
        SDL_Rect iconRect = {x, y, iconSize, iconSize};
        if (!s->costumes.empty() && s->currentCostume < (int)s->costumes.size() && s->costumes[s->currentCostume]->texture) {
            SDL_RenderCopy(ui->renderer, s->costumes[s->currentCostume]->texture, NULL, &iconRect);
        } else {
            Uint8 r = (s->currentCostume * 50) % 256;
            Uint8 g = (s->currentCostume * 80) % 256;
            Uint8 b = (s->currentCostume * 110) % 256;
            SDL_SetRenderDrawColor(ui->renderer, r, g, b, 255);
            SDL_RenderFillRect(ui->renderer, &iconRect);
        }
        SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(ui->renderer, &iconRect);
        if ((int)i == ui->selectedSpriteIndex) {
            SDL_SetRenderDrawColor(ui->renderer, 255, 0, 0, 255);
            for (int thickness = 1; thickness <= 3; thickness++) {
                SDL_Rect borderRect = {x - thickness, y - thickness, iconSize + 2*thickness, iconSize + 2*thickness};
                SDL_RenderDrawRect(ui->renderer, &borderRect);
            }
        }

        if (ui->font) {
            string nameBuffer;
            SDL_Color textColor = {0, 0, 0, 255};
            SDL_Rect textRect = {x, y + iconSize + 2, 0, 0};

            if (ui->editingName == (int)i) {
                nameBuffer = ui->nameEditBuffer;
                SDL_Rect editBg = {x - 2, y + iconSize, 100, 18};
                SDL_SetRenderDrawColor(ui->renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(ui->renderer, &editBg);
                SDL_SetRenderDrawColor(ui->renderer, 0, 0, 255, 255);
                SDL_RenderDrawRect(ui->renderer, &editBg);
            } else {
                nameBuffer = s->name;
            }

            SDL_Surface* surf = TTF_RenderText_Blended(ui->font, nameBuffer.c_str(), textColor);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(ui->renderer, surf);
                textRect.w = surf->w;
                textRect.h = surf->h;
                SDL_RenderCopy(ui->renderer, tex, NULL, &textRect);
                SDL_DestroyTexture(tex);
                SDL_FreeSurface(surf);
            }
        }

        if ((int)i == ui->selectedSpriteIndex) {
            int editX = x + iconSize + 10;
            int editY = y;
            int editW = 150;
            int lineHeight = 20;
            int btnW = 20;
            int btnH = 18;

            SDL_Rect panelRect = {editX, editY, editW, 100};
            SDL_SetRenderDrawColor(ui->renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(ui->renderer, &panelRect);
            SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(ui->renderer, &panelRect);

            char labelX[32];
            snprintf(labelX, sizeof(labelX), "x: %.1f", s->x);
            SDL_Surface* surfX = TTF_RenderText_Blended(ui->font, labelX, {0,0,0,255});
            if (surfX) {
                SDL_Texture* texX = SDL_CreateTextureFromSurface(ui->renderer, surfX);
                SDL_Rect textRectX = {editX + 5, editY + 5, surfX->w, surfX->h};
                SDL_RenderCopy(ui->renderer, texX, NULL, &textRectX);
                SDL_DestroyTexture(texX);
                SDL_FreeSurface(surfX);
            }
            SDL_Rect btnXPlus = {editX + editW - btnW - 5, editY + 5, btnW, btnH};
            SDL_Rect btnXMinus = {editX + editW - 2*btnW - 10, editY + 5, btnW, btnH};
            SDL_SetRenderDrawColor(ui->renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(ui->renderer, &btnXPlus);
            SDL_RenderFillRect(ui->renderer, &btnXMinus);
            SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(ui->renderer, &btnXPlus);
            SDL_RenderDrawRect(ui->renderer, &btnXMinus);

            SDL_Surface* plusSurf = TTF_RenderText_Blended(ui->font, "+", {0,0,0,255});
            SDL_Surface* minusSurf = TTF_RenderText_Blended(ui->font, "-", {0,0,0,255});
            SDL_Texture* plusTex = nullptr;
            SDL_Texture* minusTex = nullptr;
            if (plusSurf) {
                plusTex = SDL_CreateTextureFromSurface(ui->renderer, plusSurf);
                SDL_FreeSurface(plusSurf);
            }
            if (minusSurf) {
                minusTex = SDL_CreateTextureFromSurface(ui->renderer, minusSurf);
                SDL_FreeSurface(minusSurf);
            }

            if (plusTex && minusTex) {
                SDL_Rect textRectPlus = {btnXPlus.x + (btnW - 8)/2, btnXPlus.y + (btnH - 8)/2, 8, 8};
                SDL_RenderCopy(ui->renderer, plusTex, NULL, &textRectPlus);
                SDL_Rect textRectMinus = {btnXMinus.x + (btnW - 8)/2, btnXMinus.y + (btnH - 8)/2, 8, 8};
                SDL_RenderCopy(ui->renderer, minusTex, NULL, &textRectMinus);

                char labelY[32];
                snprintf(labelY, sizeof(labelY), "y: %.1f", s->y);
                SDL_Surface* surfY = TTF_RenderText_Blended(ui->font, labelY, {0,0,0,255});
                if (surfY) {
                    SDL_Texture* texY = SDL_CreateTextureFromSurface(ui->renderer, surfY);
                    SDL_Rect textRectY = {editX + 5, editY + 5 + lineHeight, surfY->w, surfY->h};
                    SDL_RenderCopy(ui->renderer, texY, NULL, &textRectY);
                    SDL_DestroyTexture(texY);
                    SDL_FreeSurface(surfY);
                }
                SDL_Rect btnYPlus = {editX + editW - btnW - 5, editY + 5 + lineHeight, btnW, btnH};
                SDL_Rect btnYMinus = {editX + editW - 2*btnW - 10, editY + 5 + lineHeight, btnW, btnH};
                SDL_SetRenderDrawColor(ui->renderer, 200, 200, 200, 255);
                SDL_RenderFillRect(ui->renderer, &btnYPlus);
                SDL_RenderFillRect(ui->renderer, &btnYMinus);
                SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(ui->renderer, &btnYPlus);
                SDL_RenderDrawRect(ui->renderer, &btnYMinus);
                textRectPlus.x = btnYPlus.x + (btnW - 8)/2;
                textRectPlus.y = btnYPlus.y + (btnH - 8)/2;
                SDL_RenderCopy(ui->renderer, plusTex, NULL, &textRectPlus);
                textRectMinus.x = btnYMinus.x + (btnW - 8)/2;
                textRectMinus.y = btnYMinus.y + (btnH - 8)/2;
                SDL_RenderCopy(ui->renderer, minusTex, NULL, &textRectMinus);

                char labelDir[32];
                snprintf(labelDir, sizeof(labelDir), "dir: %.0f", s->direction);
                SDL_Surface* surfDir = TTF_RenderText_Blended(ui->font, labelDir, {0,0,0,255});
                if (surfDir) {
                    SDL_Texture* texDir = SDL_CreateTextureFromSurface(ui->renderer, surfDir);
                    SDL_Rect textRectDir = {editX + 5, editY + 5 + 2*lineHeight, surfDir->w, surfDir->h};
                    SDL_RenderCopy(ui->renderer, texDir, NULL, &textRectDir);
                    SDL_DestroyTexture(texDir);
                    SDL_FreeSurface(surfDir);
                }
                SDL_Rect btnDirPlus = {editX + editW - btnW - 5, editY + 5 + 2*lineHeight, btnW, btnH};
                SDL_Rect btnDirMinus = {editX + editW - 2*btnW - 10, editY + 5 + 2*lineHeight, btnW, btnH};
                SDL_SetRenderDrawColor(ui->renderer, 200, 200, 200, 255);
                SDL_RenderFillRect(ui->renderer, &btnDirPlus);
                SDL_RenderFillRect(ui->renderer, &btnDirMinus);
                SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(ui->renderer, &btnDirPlus);
                SDL_RenderDrawRect(ui->renderer, &btnDirMinus);
                textRectPlus.x = btnDirPlus.x + (btnW - 8)/2;
                textRectPlus.y = btnDirPlus.y + (btnH - 8)/2;
                SDL_RenderCopy(ui->renderer, plusTex, NULL, &textRectPlus);
                textRectMinus.x = btnDirMinus.x + (btnW - 8)/2;
                textRectMinus.y = btnDirMinus.y + (btnH - 8)/2;
                SDL_RenderCopy(ui->renderer, minusTex, NULL, &textRectMinus);

                char labelSize[32];
                snprintf(labelSize, sizeof(labelSize), "size: %.0f%%", s->size);
                SDL_Surface* surfSize = TTF_RenderText_Blended(ui->font, labelSize, {0,0,0,255});
                if (surfSize) {
                    SDL_Texture* texSize = SDL_CreateTextureFromSurface(ui->renderer, surfSize);
                    SDL_Rect textRectSize = {editX + 5, editY + 5 + 3*lineHeight, surfSize->w, surfSize->h};
                    SDL_RenderCopy(ui->renderer, texSize, NULL, &textRectSize);
                    SDL_DestroyTexture(texSize);
                    SDL_FreeSurface(surfSize);
                }
                SDL_Rect btnSizePlus = {editX + editW - btnW - 5, editY + 5 + 3*lineHeight, btnW, btnH};
                SDL_Rect btnSizeMinus = {editX + editW - 2*btnW - 10, editY + 5 + 3*lineHeight, btnW, btnH};
                SDL_SetRenderDrawColor(ui->renderer, 200, 200, 200, 255);
                SDL_RenderFillRect(ui->renderer, &btnSizePlus);
                SDL_RenderFillRect(ui->renderer, &btnSizeMinus);
                SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(ui->renderer, &btnSizePlus);
                SDL_RenderDrawRect(ui->renderer, &btnSizeMinus);
                textRectPlus.x = btnSizePlus.x + (btnW - 8)/2;
                textRectPlus.y = btnSizePlus.y + (btnH - 8)/2;
                SDL_RenderCopy(ui->renderer, plusTex, NULL, &textRectPlus);
                textRectMinus.x = btnSizeMinus.x + (btnW - 8)/2;
                textRectMinus.y = btnSizeMinus.y + (btnH - 8)/2;
                SDL_RenderCopy(ui->renderer, minusTex, NULL, &textRectMinus);
            }

            SDL_Rect btnShow = {editX + 5, editY + 5 + 4*lineHeight, 60, btnH};
            SDL_SetRenderDrawColor(ui->renderer, s->visible ? 100 : 200, s->visible ? 200 : 100, 100, 255);
            SDL_RenderFillRect(ui->renderer, &btnShow);
            SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(ui->renderer, &btnShow);
            SDL_Surface* showSurf = TTF_RenderText_Blended(ui->font, s->visible ? "Hide" : "Show", {0,0,0,255});
            if (showSurf) {
                SDL_Texture* showTex = SDL_CreateTextureFromSurface(ui->renderer, showSurf);
                SDL_Rect textRect = {btnShow.x + (60 - showSurf->w)/2, btnShow.y + (btnH - showSurf->h)/2, showSurf->w, showSurf->h};
                SDL_RenderCopy(ui->renderer, showTex, NULL, &textRect);
                SDL_DestroyTexture(showTex);
                SDL_FreeSurface(showSurf);
            }

            SDL_Rect btnDelete = {editX + 70, editY + 5 + 4*lineHeight, 50, btnH};
            SDL_SetRenderDrawColor(ui->renderer, 200, 100, 100, 255);
            SDL_RenderFillRect(ui->renderer, &btnDelete);
            SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(ui->renderer, &btnDelete);
            SDL_Surface* delSurf = TTF_RenderText_Blended(ui->font, "Del", {0,0,0,255});
            if (delSurf) {
                SDL_Texture* delTex = SDL_CreateTextureFromSurface(ui->renderer, delSurf);
                SDL_Rect textRect = {btnDelete.x + (50 - delSurf->w)/2, btnDelete.y + (btnH - delSurf->h)/2, delSurf->w, delSurf->h};
                SDL_RenderCopy(ui->renderer, delTex, NULL, &textRect);
                SDL_DestroyTexture(delTex);
                SDL_FreeSurface(delSurf);
            }

            if (plusTex) SDL_DestroyTexture(plusTex);
            if (minusTex) SDL_DestroyTexture(minusTex);
        }
    }
}

void SpriteManagerUI_handleEvent(SpriteManagerUI* ui, SDL_Event* e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int x = e->button.x, y = e->button.y;
        if (x >= ui->rect.x && x <= ui->rect.x + ui->rect.w &&
            y >= ui->rect.y && y <= ui->rect.y + ui->rect.h) {
            int iconSize = 50;
            int spacing = 70;
            int startX = ui->rect.x + 10;
            int startY = ui->rect.y + 10 - ui->scrollOffset;
            int index = (x - startX) / spacing;
            int iconY = startY + index * spacing;

            if (index >= 0 && index < (int)ui->project->sprites.size() &&
                y >= iconY && y <= iconY + iconSize) {

                Uint32 now = SDL_GetTicks();
                if (ui->lastClickedIndex == index && now - ui->lastClickTime < 500) {
                    ui->editingName = index;
                    ui->nameEditBuffer = ui->project->sprites[index]->name;
                    SDL_StartTextInput();
                    ui->lastClickTime = 0;
                    ui->lastClickedIndex = -1;
                } else {
                    ui->selectedSpriteIndex = index;
                    ui->lastClickTime = now;
                    ui->lastClickedIndex = index;
                    ui->editingName = -1;
                    SDL_StopTextInput();
                }
                return;
            } else {
                if (ui->selectedSpriteIndex >= 0 && ui->selectedSpriteIndex < (int)ui->project->sprites.size()) {
                    Sprite* s = ui->project->sprites[ui->selectedSpriteIndex];
                    int selectedIconY = startY + ui->selectedSpriteIndex * spacing;
                    int editX = startX + ui->selectedSpriteIndex * spacing + iconSize + 10;
                    int editY = selectedIconY;
                    int editW = 150;
                    int lineHeight = 20;
                    int btnW = 20;
                    int btnH = 18;

                    SDL_Rect btnXPlus = {editX + editW - btnW - 5, editY + 5, btnW, btnH};
                    SDL_Rect btnXMinus = {editX + editW - 2*btnW - 10, editY + 5, btnW, btnH};
                    if (x >= btnXPlus.x && x <= btnXPlus.x + btnXPlus.w && y >= btnXPlus.y && y <= btnXPlus.y + btnXPlus.h) {
                        s->x += 10;
                        if (s->x > 240) s->x = 240;
                        return;
                    }
                    if (x >= btnXMinus.x && x <= btnXMinus.x + btnXMinus.w && y >= btnXMinus.y && y <= btnXMinus.y + btnXMinus.h) {
                        s->x -= 10;
                        if (s->x < -240) s->x = -240;
                        return;
                    }

                    SDL_Rect btnYPlus = {editX + editW - btnW - 5, editY + 5 + lineHeight, btnW, btnH};
                    SDL_Rect btnYMinus = {editX + editW - 2*btnW - 10, editY + 5 + lineHeight, btnW, btnH};
                    if (x >= btnYPlus.x && x <= btnYPlus.x + btnYPlus.w && y >= btnYPlus.y && y <= btnYPlus.y + btnYPlus.h) {
                        s->y += 10;
                        if (s->y > 180) s->y = 180;
                        return;
                    }
                    if (x >= btnYMinus.x && x <= btnYMinus.x + btnYMinus.w && y >= btnYMinus.y && y <= btnYMinus.y + btnYMinus.h) {
                        s->y -= 10;
                        if (s->y < -180) s->y = -180;
                        return;
                    }

                    SDL_Rect btnDirPlus = {editX + editW - btnW - 5, editY + 5 + 2*lineHeight, btnW, btnH};
                    SDL_Rect btnDirMinus = {editX + editW - 2*btnW - 10, editY + 5 + 2*lineHeight, btnW, btnH};
                    if (x >= btnDirPlus.x && x <= btnDirPlus.x + btnDirPlus.w && y >= btnDirPlus.y && y <= btnDirPlus.y + btnDirPlus.h) {
                        s->direction += 15;
                        if (s->direction >= 360) s->direction -= 360;
                        return;
                    }
                    if (x >= btnDirMinus.x && x <= btnDirMinus.x + btnDirMinus.w && y >= btnDirMinus.y && y <= btnDirMinus.y + btnDirMinus.h) {
                        s->direction -= 15;
                        if (s->direction < 0) s->direction += 360;
                        return;
                    }

                    SDL_Rect btnSizePlus = {editX + editW - btnW - 5, editY + 5 + 3*lineHeight, btnW, btnH};
                    SDL_Rect btnSizeMinus = {editX + editW - 2*btnW - 10, editY + 5 + 3*lineHeight, btnW, btnH};
                    if (x >= btnSizePlus.x && x <= btnSizePlus.x + btnSizePlus.w && y >= btnSizePlus.y && y <= btnSizePlus.y + btnSizePlus.h) {
                        s->size += 10;
                        if (s->size > 200) s->size = 200;
                        return;
                    }
                    if (x >= btnSizeMinus.x && x <= btnSizeMinus.x + btnSizeMinus.w && y >= btnSizeMinus.y && y <= btnSizeMinus.y + btnSizeMinus.h) {
                        s->size -= 10;
                        if (s->size < 10) s->size = 10;
                        return;
                    }

                    SDL_Rect btnShow = {editX + 5, editY + 5 + 4*lineHeight, 60, btnH};
                    if (x >= btnShow.x && x <= btnShow.x + btnShow.w && y >= btnShow.y && y <= btnShow.y + btnShow.h) {
                        s->visible = !s->visible;
                        return;
                    }

                    SDL_Rect btnDelete = {editX + 70, editY + 5 + 4*lineHeight, 50, btnH};
                    if (x >= btnDelete.x && x <= btnDelete.x + btnDelete.w && y >= btnDelete.y && y <= btnDelete.y + btnDelete.h) {
                        if (ui->selectedSpriteIndex >= 0) {
                            Sprite* s = ui->project->sprites[ui->selectedSpriteIndex];
                            for (Costume* c : s->costumes) {
                                if (c->texture) SDL_DestroyTexture(c->texture);
                                delete c;
                            }
                            for (Script* scr : s->scripts) {
                                for (Block* b : scr->blocks) {
                                    free_block(b);
                                }
                                delete scr;
                            }
                            if (s->penCanvas) SDL_DestroyTexture(s->penCanvas);
                            delete s;
                            ui->project->sprites.erase(ui->project->sprites.begin() + ui->selectedSpriteIndex);
                            if (ui->selectedSpriteIndex >= (int)ui->project->sprites.size()) {
                                ui->selectedSpriteIndex = ui->project->sprites.size() - 1;
                            }
                        }
                        return;
                    }
                }
            }
        }
    } else if (e->type == SDL_MOUSEWHEEL) {
        if (e->wheel.y != 0) {
            ui->scrollOffset -= e->wheel.y * 20;
            int maxOffset = (ui->project->sprites.size() * 70 + 10) - ui->rect.h;
            if (maxOffset < 0) maxOffset = 0;
            if (ui->scrollOffset < 0) ui->scrollOffset = 0;
            if (ui->scrollOffset > maxOffset) ui->scrollOffset = maxOffset;
        }
    } else if (e->type == SDL_KEYDOWN && ui->editingName >= 0) {
        if (e->key.keysym.sym == SDLK_RETURN) {
            if (ui->editingName >= 0 && ui->editingName < (int)ui->project->sprites.size()) {
                Sprite* s = ui->project->sprites[ui->editingName];
                s->name = ui->nameEditBuffer;
            }
            ui->editingName = -1;
            SDL_StopTextInput();
        } else if (e->key.keysym.sym == SDLK_ESCAPE) {
            ui->editingName = -1;
            SDL_StopTextInput();
        } else if (e->key.keysym.sym == SDLK_BACKSPACE) {
            if (!ui->nameEditBuffer.empty()) {
                ui->nameEditBuffer.pop_back();
            }
        }
    } else if (e->type == SDL_TEXTINPUT && ui->editingName >= 0) {
        ui->nameEditBuffer += e->text.text;
    }
}

void SpriteManagerUI_uploadCostume(SpriteManagerUI* ui, const char* filepath) {
    if (ui->selectedSpriteIndex < 0) {
        printf("No sprite selected.\n");
        return;
    }
    FILE* f = fopen(filepath, "rb");
    if (!f) {
        printf("File %s not found. Please place it in the program directory.\n", filepath);
        return;
    }
    fclose(f);

    char* fullPath = findFontFile(filepath);
    Sprite* sprite = ui->project->sprites[ui->selectedSpriteIndex];
    size_t oldCount = sprite->costumes.size();
    Sprite_addCostumeFromFile(sprite, ui->renderer, fullPath);
    if (sprite->costumes.size() > oldCount) {
        if (sprite->costumes.back()->texture) {
            sprite->currentCostume = sprite->costumes.size() - 1;
            printf("New costume successfully added from %s and set as current.\n", fullPath);
        } else {
            printf("Error: texture is NULL after loading %s\n", fullPath);
        }
    } else {
        printf("Error loading image from path: %s (costume count did not increase)\n", fullPath);
    }
    free(fullPath);
}
