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
