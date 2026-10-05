#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

//gcc main.c -o main.exe -IC:\raylib\raylib\src -LC:\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm -lm
//.\main.exe

// ============================================================
// SETTINGS
// ============================================================

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define MAX_HISTORY 100
#define MAX_ALIAS 30
#define MISSION_COUNT 15

// ============================================================
// PLAYER
// ============================================================

typedef struct
{
    char alias[MAX_ALIAS];
    int reputation;
    int skillLevel;
} Player;

// ============================================================
// HISTORY
// ============================================================

typedef struct
{
    char description[200];
    int reputation;
    int skillLevel;
} HistoryEntry;

HistoryEntry history[MAX_HISTORY];
int historyCount = 0;

// ============================================================
// MISSION
// ============================================================

typedef struct
{
    char title[100];
    char description1[200];
    char description2[200];

    char choices[3][100];

    int successChance[3];
    int reputationChange[3];
    int skillChange[3];

    char successMessage[3][200];
    char failureMessage[3][200];

    char awarenessMessage[200];

} Mission;

// ============================================================
// GAME STATES
// ============================================================

typedef enum
{
    SCREEN_LOADING,
    SCREEN_ALIAS,
    SCREEN_MENU,
    SCREEN_MISSIONS,
    SCREEN_MISSION,
    SCREEN_RESULT,
    SCREEN_HISTORY,
    SCREEN_GAME_COMPLETE

} GameScreen;

GameScreen currentScreen = SCREEN_LOADING;

// ============================================================
// GLOBAL VARIABLES
// ============================================================

Player player;

Mission missions[MISSION_COUNT] = {
    {
        .title = "MISSION 1: DATA BREACH",
        .description1 = "A company has suffered a data breach.",
        .description2 = "Private customer information may be exposed.",
        .awarenessMessage = "Reporting suspicious activity helps protect users.",
        .choices = { "Report immediately", "Investigate", "Secure accounts" },
        .successChance = { 90, 60, 75 },
        .reputationChange = { 5, 10, 8 },
        .skillChange = { 2, 3, 3 },
        .successMessage = { "You reported the breach.", "You found evidence.", "You secured the accounts." },
        .failureMessage = { "The report failed.", "The investigation failed.", "You failed to secure the accounts." },
    },
    {
        .title = "MISSION 2: FIREWALL PUZZLE",
        .description1 = "You reached a protected system.",
        .description2 = "A firewall is blocking your access.",
        .awarenessMessage = "Use authorized methods when dealing with protected systems.",
        .choices = { "Safe method", "Risky shortcut", "Analyze rules" },
        .successChance = { 80, 50, 70 },
        .reputationChange = { 5, 15, 7 },
        .skillChange = { 3, 4, 4 },
        .successMessage = { "The safe method worked.", "The risky shortcut worked.", "You analyzed the rules successfully." },
        .failureMessage = { "The safe method failed.", "The shortcut failed.", "You could not analyze the rules." },
    },
    {
        .title = "MISSION 3: AI BIAS",
        .description1 = "An AI system is being used to make important decisions.",
        .description2 = "You suspect the system may be treating users unfairly.",
        .awarenessMessage = "AI systems should be checked for fairness and bias.",
        .choices = { "Report bias", "Ignore for reward", "Review evidence" },
        .successChance = { 85, 55, 75 },
        .reputationChange = { 8, 12, 10 },
        .skillChange = { 3, 2, 4 },
        .successMessage = { "The bias was reported.", "You gained a quick reward.", "You found evidence." },
        .failureMessage = { "The report failed.", "You failed to gain the reward.", "You found no useful evidence." },
    },
    {
        .title = "MISSION 4: PHISHING INVESTIGATION",
        .description1 = "An employee received a suspicious email.",
        .description2 = "The message requests sensitive account information.",
        .awarenessMessage = "Always verify suspicious messages before responding.",
        .choices = { "Report email", "Investigate", "Verify sender" },
        .successChance = { 90, 65, 85 },
        .reputationChange = { 7, 12, 9 },
        .skillChange = { 3, 4, 3 },
        .successMessage = { "You reported the phishing email.", "You investigated successfully.", "You verified the sender." },
        .failureMessage = { "The report failed.", "The investigation was too slow.", "You could not verify the sender." },
    },
    {
        .title = "MISSION 5: RANSOMWARE CRISIS",
        .description1 = "Multiple computers are suddenly unavailable.",
        .description2 = "Important files appear to have been locked.",
        .awarenessMessage = "Isolating affected systems can limit the spread of ransomware.",
        .choices = { "Emergency plan", "Risky recovery", "Isolate computers" },
        .successChance = { 75, 40, 80 },
        .reputationChange = { 15, 25, 12 },
        .skillChange = { 5, 6, 5 },
        .successMessage = { "The response plan worked.", "The risky recovery worked.", "You isolated the computers." },
        .failureMessage = { "The response plan failed.", "The recovery failed.", "You failed to isolate the computers." },
    },
    {
        .title = "MISSION 6: INSIDER THREAT",
        .description1 = "Security logs show unusual activity from an employee.",
        .description2 = "You must investigate without making a false accusation.",
        .awarenessMessage = "Security investigations should be based on evidence.",
        .choices = { "Report activity", "Confront employee", "Review logs" },
        .successChance = { 85, 45, 80 },
        .reputationChange = { 18, 20, 10 },
        .skillChange = { 5, 4, 4 },
        .successMessage = { "You reported the activity properly.", "The confrontation became complicated.", "You found evidence." },
        .failureMessage = { "The report failed.", "The confrontation failed.", "You found no evidence." },
    },
    {
        .title = "MISSION 7: PASSWORD SECURITY",
        .description1 = "An employee is using a weak and predictable password.",
        .description2 = "The account has access to important resources.",
        .awarenessMessage = "Strong and unique passwords help protect accounts.",
        .choices = { "Report weak password", "Change password", "Review policies" },
        .successChance = { 85, 80, 75 },
        .reputationChange = { 6, 8, 7 },
        .skillChange = { 3, 4, 4 },
        .successMessage = { "You reported the weak password.", "You changed the password successfully.", "You reviewed the password policies." },
        .failureMessage = { "The report failed.", "You failed to change the password.", "You could not review the policies." },
    },
    {
        .title = "MISSION 8: SOCIAL ENGINEERING",
        .description1 = "Someone calls pretending to be a senior manager.",
        .description2 = "They urgently request confidential information.",
        .awarenessMessage = "Always verify unusual requests before sharing information.",
        .choices = { "Provide information", "Verify caller", "Report request" },
        .successChance = { 35, 90, 85 },
        .reputationChange = { 15, 10, 9 },
        .skillChange = { 2, 4, 4 },
        .successMessage = { "The information was provided.", "You verified the caller successfully.", "You reported the suspicious request." },
        .failureMessage = { "The request was rejected.", "You could not verify the caller.", "The report failed." },
    },
    {
        .title = "MISSION 9: SECURE BACKUP",
        .description1 = "Important backups have not been tested.",
        .description2 = "A recent system failure makes the situation urgent.",
        .awarenessMessage = "Backups should be maintained and tested regularly.",
        .choices = { "Test backup", "Ignore issue", "Create new backup" },
        .successChance = { 85, 40, 80 },
        .reputationChange = { 9, 12, 10 },
        .skillChange = { 4, 2, 5 },
        .successMessage = { "The backup test was successful.", "You ignored the issue.", "You created a new backup successfully." },
        .failureMessage = { "The backup test failed.", "Ignoring the issue caused problems.", "The backup could not be created." },
    },
    {
        .title = "MISSION 10: PUBLIC WI-FI",
        .description1 = "You are working from a public location.",
        .description2 = "You need to access an important company account.",
        .awarenessMessage = "Avoid sensitive activities on unsecured networks.",
        .choices = { "Use public network", "Use secure connection", "Wait for trusted network" },
        .successChance = { 40, 90, 85 },
        .reputationChange = { 14, 10, 8 },
        .skillChange = { 2, 4, 3 },
        .successMessage = { "You accessed the account.", "You used a secure connection.", "You waited for a trusted network." },
        .failureMessage = { "The connection caused security problems.", "The secure connection failed.", "You could not find a trusted network." },
    },
    {
        .title = "MISSION 11: SOFTWARE UPDATE",
        .description1 = "A critical security update is available.",
        .description2 = "Employees worry that the update may interrupt work.",
        .awarenessMessage = "Keeping software updated reduces security risks.",
        .choices = { "Install immediately", "Delay update", "Test update first" },
        .successChance = { 85, 45, 90 },
        .reputationChange = { 9, 12, 10 },
        .skillChange = { 4, 2, 5 },
        .successMessage = { "The security update was installed successfully.", "You delayed the update.", "You tested the update successfully." },
        .failureMessage = { "The update installation failed.", "The delay caused problems.", "The update test failed." },
    },
    {
        .title = "MISSION 12: USB THREAT",
        .description1 = "An unknown USB device has been found in the office.",
        .description2 = "You do not know whether it contains malicious software.",
        .awarenessMessage = "Unknown devices should never be connected to company computers.",
        .choices = { "Plug it into a computer", "Report the device", "Give it to security" },
        .successChance = { 30, 90, 85 },
        .reputationChange = { 18, 8, 9 },
        .skillChange = { 2, 3, 4 },
        .successMessage = { "The USB device appeared harmless.", "You reported the unknown device.", "You gave the device to security staff." },
        .failureMessage = { "The USB device caused a security incident.", "The report failed.", "Security staff could not safely process the device." },
    },
    {
        .title = "MISSION 13: DATA CLASSIFICATION",
        .description1 = "Several files contain sensitive company information.",
        .description2 = "The files have incorrect access permissions.",
        .awarenessMessage = "Sensitive information should only be accessible to authorized users.",
        .choices = { "Restrict access", "Ignore permissions", "Review permissions" },
        .successChance = { 85, 35, 80 },
        .reputationChange = { 10, 15, 9 },
        .skillChange = { 4, 2, 5 },
        .successMessage = { "You restricted access to the sensitive files.", "You ignored the permissions.", "You reviewed and corrected the permissions." },
        .failureMessage = { "You failed to restrict access.", "Ignoring the permissions caused problems.", "You could not correct the permissions." },
    },
    {
        .title = "MISSION 14: SECURITY AUDIT",
        .description1 = "A routine security audit reveals unusual settings.",
        .description2 = "You need to determine whether they create risks.",
        .awarenessMessage = "Regular security audits help identify weaknesses.",
        .choices = { "Ignore settings", "Review settings", "Report findings" },
        .successChance = { 35, 85, 90 },
        .reputationChange = { 14, 10, 12 },
        .skillChange = { 2, 5, 4 },
        .successMessage = { "You ignored the settings.", "You reviewed the settings successfully.", "You reported the audit findings." },
        .failureMessage = { "Ignoring the settings caused problems.", "You could not review the settings.", "The report failed." },
    },
    {
        .title = "MISSION 15: INCIDENT RESPONSE",
        .description1 = "Multiple security alerts appear across the network.",
        .description2 = "You must respond before the situation gets worse.",
        .awarenessMessage = "A prepared incident response plan helps organizations handle incidents.",
        .choices = { "Investigate alerts", "Shut down everything", "Follow response plan" },
        .successChance = { 80, 50, 90 },
        .reputationChange = { 12, 15, 15 },
        .skillChange = { 5, 3, 6 },
        .successMessage = { "You investigated the alerts successfully.", "The shutdown prevented further activity.", "You successfully followed the response plan." },
        .failureMessage = { "The investigation failed.", "The shutdown caused additional problems.", "The response plan failed." },
    },
};

int currentMission = -1;
int selectedChoice = -1;

int missionRoll = 0;
int missionSuccess = 0;

char resultMessage[200];
char awarenessMessage[200];

int gameRunning = true;

char aliasInput[MAX_ALIAS] = "";
int aliasLength = 0;

// Animation and optional audio effects. Sound files are safe to omit.
#define FX_PARTICLE_COUNT 72
typedef struct { float x, y, speed, size, phase; } FxParticle;
static FxParticle fxParticles[FX_PARTICLE_COUNT];
static float fxTime = 0.0f;
static float loadingTime = 0.0f;
static float transitionFlash = 0.0f;
static float resultEffectTime = 0.0f;
static bool audioReady = false;
typedef struct { Sound hover, click, success, failure, transition; } GameSounds;
static GameSounds gameSounds = { 0 };

// ============================================================
// COLORS
// ============================================================

Color BACKGROUND = {10, 10, 18, 255};
Color PANEL = {20, 22, 35, 255};
Color PANEL_LIGHT = {30, 32, 48, 255};
Color MY_GREEN = {50, 220, 120, 255};
Color MY_RED = {230, 70, 80, 255};
Color CYAN = {60, 200, 255, 255};
Color MY_YELLOW = {245, 210, 70, 255};
Color MY_WHITE = {240, 240, 240, 255};
Color MY_GRAY = {160, 160, 175, 255};

static void PlayIfReady(Sound sound)
{
    if (audioReady && sound.frameCount > 0) PlaySound(sound);
}

static Sound LoadOptionalSound(const char *path)
{
    if (audioReady && FileExists(path)) return LoadSound(path);
    return (Sound){ 0 };
}

static void LoadGameSounds(void)
{
    InitAudioDevice();
    audioReady = IsAudioDeviceReady();
    if (!audioReady) return;
    gameSounds.hover = LoadOptionalSound("sounds/hover.wav");
    gameSounds.click = LoadOptionalSound("sounds/click.wav");
    gameSounds.success = LoadOptionalSound("sounds/success.wav");
    gameSounds.failure = LoadOptionalSound("sounds/failure.wav");
    gameSounds.transition = LoadOptionalSound("sounds/transition.wav");
}

static void UnloadGameSounds(void)
{
    if (!audioReady) return;
    if (gameSounds.hover.frameCount > 0) UnloadSound(gameSounds.hover);
    if (gameSounds.click.frameCount > 0) UnloadSound(gameSounds.click);
    if (gameSounds.success.frameCount > 0) UnloadSound(gameSounds.success);
    if (gameSounds.failure.frameCount > 0) UnloadSound(gameSounds.failure);
    if (gameSounds.transition.frameCount > 0) UnloadSound(gameSounds.transition);
    CloseAudioDevice();
    audioReady = false;
}

static void InitEffects(void)
{
    for (int i=0; i<FX_PARTICLE_COUNT; i++) {
        fxParticles[i].x = (float)((i*173 + 31) % SCREEN_WIDTH);
        fxParticles[i].y = (float)((i*97 + 11) % SCREEN_HEIGHT);
        fxParticles[i].speed = 12.0f + (float)(i%8)*5.0f;
        fxParticles[i].size = 1.0f + (float)(i%3);
        fxParticles[i].phase = (float)i*0.37f;
    }
}

static void UpdateEffects(float dt)
{
    fxTime += dt;
    if (transitionFlash > 0.0f) transitionFlash -= dt;
    if (resultEffectTime > 0.0f) resultEffectTime -= dt;
    for (int i=0; i<FX_PARTICLE_COUNT; i++) {
        fxParticles[i].y += fxParticles[i].speed*dt;
        if (fxParticles[i].y > SCREEN_HEIGHT) {
            fxParticles[i].y = -5.0f;
            fxParticles[i].x = (float)((i*173 + (int)(fxTime*19.0f)) % SCREEN_WIDTH);
        }
    }
}

// Shared animated neon grid used by every game screen.
void DrawCyberBackground(void)
{
    ClearBackground(BACKGROUND);
    float pulse = 0.5f + 0.5f*sinf(fxTime*2.8f);
    for (int x=0; x<SCREEN_WIDTH; x+=40) DrawLine(x,54,x,SCREEN_HEIGHT,Fade(CYAN,0.07f));
    for (int y=94; y<SCREEN_HEIGHT; y+=40) DrawLine(0,y,SCREEN_WIDTH,y,Fade(CYAN,0.06f));

    // Soft falling binary columns and drifting data particles.
    for (int col=0; col<24; col++) {
        int x=(col*53+18)%SCREEN_WIDTH;
        int top=(int)fmodf(fxTime*(38.0f+(col%4)*9.0f)+col*61.0f,SCREEN_HEIGHT+180)-90;
        for (int row=0; row<4; row++) {
            int bit=(col+row+(int)(fxTime*1.5f))%2;
            DrawText(bit ? "1":"0",x,top-row*22,14,Fade(CYAN,0.10f+0.035f*row));
        }
    }
    for (int i=0; i<FX_PARTICLE_COUNT; i++) {
        float twinkle=0.35f+0.45f*(0.5f+0.5f*sinf(fxTime*2.0f+fxParticles[i].phase));
        DrawCircleV((Vector2){fxParticles[i].x,fxParticles[i].y},fxParticles[i].size,Fade(i%4 ? CYAN : MY_GREEN,twinkle));
    }

    // Moving scan beam, corner brackets, and subtle screen scanlines.
    float beamY=58.0f+fmodf(fxTime*72.0f,(float)(SCREEN_HEIGHT-58));
    DrawRectangle(0,(int)beamY,SCREEN_WIDTH,2,Fade(CYAN,0.15f));
    DrawRectangle(0,(int)beamY-5,SCREEN_WIDTH,8,Fade(CYAN,0.035f));
    for (int y=58; y<SCREEN_HEIGHT; y+=4) DrawLine(0,y,SCREEN_WIDTH,y,Fade(BLACK,0.10f));
    DrawRectangle(0,0,SCREEN_WIDTH,54,Fade(PANEL,0.98f));
    DrawLine(0,54,SCREEN_WIDTH,54,Fade(CYAN,0.65f+0.30f*pulse));
    DrawText("CYBERNET // SECURE OPERATIONS",24,18,14,CYAN);
    DrawText("NETWORK: ENCRYPTED",SCREEN_WIDTH-230,18,14,MY_GREEN);

    // Animated HUD corner brackets.
    Color bracket=Fade(CYAN,0.45f+0.45f*pulse);
    DrawLineEx((Vector2){16,70},(Vector2){16,108},2,bracket); DrawLineEx((Vector2){16,70},(Vector2){54,70},2,bracket);
    DrawLineEx((Vector2){SCREEN_WIDTH-16,70},(Vector2){SCREEN_WIDTH-16,108},2,bracket); DrawLineEx((Vector2){SCREEN_WIDTH-16,70},(Vector2){SCREEN_WIDTH-54,70},2,bracket);
}

void DrawNeonPanel(Rectangle rect, Color accent)
{
    float pulse = 0.78f + 0.22f*(0.5f + 0.5f*sinf(fxTime*3.0f + rect.x*0.01f));
    DrawRectangleRounded(rect, 0.08f, 8, Fade(PANEL, 0.97f));
    DrawRectangleRoundedLines(rect, 0.08f, 8, Fade(accent,pulse));
    DrawRectangle((int)rect.x,(int)rect.y,(int)(rect.width*0.18f*pulse),2,Fade(accent,0.75f));
}
// ============================================================
// DRAW CENTERED TEXT
// ============================================================

void DrawCenteredText(const char *text, int y, int fontSize, Color color)
{
    int width = MeasureText(text, fontSize);

    DrawText(
        text,
        SCREEN_WIDTH / 2 - width / 2,
        y,
        fontSize,
        color
    );
}

// ============================================================
// BUTTON
// ============================================================

bool DrawButton(Rectangle rect, const char *text, Color accent)
{
    static bool hoverWasActive = false;
    static Rectangle lastHoverRect = { 0 };
    bool hovered = CheckCollisionPointRec(GetMousePosition(),rect);
    bool sameButton = lastHoverRect.x==rect.x && lastHoverRect.y==rect.y;
    if (hovered && (!hoverWasActive || !sameButton)) PlayIfReady(gameSounds.hover);
    if (hovered) { hoverWasActive=true; lastHoverRect=rect; }
    else if (sameButton) hoverWasActive=false;

    float pulse=0.5f+0.5f*sinf(fxTime*7.0f);
    Rectangle drawRect=rect;
    if (hovered) { drawRect.x-=2; drawRect.y-=2; drawRect.width+=4; drawRect.height+=4; }
    DrawRectangleRounded(drawRect,0.08f,8,Fade(hovered ? accent : PANEL_LIGHT,hovered ? 0.72f : 0.92f));
    DrawRectangleRoundedLines(drawRect,0.08f,8,Fade(accent,hovered ? 0.78f+0.22f*pulse : 0.72f));
    DrawRectangle((int)drawRect.x,(int)drawRect.y,4,(int)drawRect.height,accent);
    int size=20, width=MeasureText(text,size);
    DrawText(text,(int)(drawRect.x+(drawRect.width-width)/2),(int)(drawRect.y+(drawRect.height-size)/2),size,MY_WHITE);
    bool clicked=hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (clicked) PlayIfReady(gameSounds.click);
    return clicked;
}
// ============================================================
// ADD HISTORY
// ============================================================

void AddHistory(const char *message)
{
    if (historyCount >= MAX_HISTORY)
        return;

    strcpy(history[historyCount].description, message);

    history[historyCount].reputation =
        player.reputation;

    history[historyCount].skillLevel =
        player.skillLevel;

    historyCount++;
}

// ============================================================
// START RANDOM MISSION
// ============================================================

void StartRandomMission()
{
    currentMission = rand() % MISSION_COUNT;

    selectedChoice = -1;

    currentScreen = SCREEN_MISSION;
}

// ============================================================
// RESOLVE CHOICE
// ============================================================

void ResolveChoice(int choice)
{
    if (choice < 0 || choice > 2)
        return;

    Mission *m = &missions[currentMission];

    missionRoll = rand() % 100 + 1;

    if (missionRoll <= m->successChance[choice])
    {
        missionSuccess = true;

        player.reputation +=
            m->reputationChange[choice];

        player.skillLevel +=
            m->skillChange[choice];

        strcpy(
            resultMessage,
            m->successMessage[choice]
        );
    }
    else
    {
        missionSuccess = false;

        player.reputation -=
            m->reputationChange[choice];

        player.skillLevel -= 1;

        strcpy(
            resultMessage,
            m->failureMessage[choice]
        );
    }

    strcpy(
        awarenessMessage,
        m->awarenessMessage
    );

    AddHistory(resultMessage);

    selectedChoice = choice;
    PlayIfReady(missionSuccess ? gameSounds.success : gameSounds.failure);
    transitionFlash = 0.22f;
    resultEffectTime = 0.85f;

    currentScreen = SCREEN_RESULT;
}

// ============================================================
// DRAW PLAYER STATS
// ============================================================

void DrawStats()
{
    int y = currentScreen == SCREEN_MISSION ? 72 : currentScreen == SCREEN_RESULT ? 485 : 220;
    Rectangle box = { 820, y, 350, 112 };
    DrawNeonPanel(box, CYAN);
    DrawText("PLAYER STATUS", 840, y + 12, 18, CYAN);
    DrawText(TextFormat("Alias: %s", player.alias), 840, y + 40, 16, MY_WHITE);
    DrawText(TextFormat("Reputation: %d", player.reputation), 840, y + 65, 16, MY_YELLOW);
    DrawText(TextFormat("Skill Level: %d", player.skillLevel), 1010, y + 65, 16, MY_GREEN);
}// ============================================================
// ALIAS SCREEN
// ============================================================
void DrawAliasScreen()
{
    DrawCyberBackground();
    DrawCenteredText(
        "CYBERPUNK CODEBREAKER", 100, 48, CYAN
    );

    DrawCenteredText(
        "DIGITAL REBEL", 160, 25, MY_GREEN
    );

    DrawCenteredText(
        "Enter your player alias", 240, 25, MY_WHITE
    );

    Rectangle inputBox =
    {
        300, 300, 500, 60
    };

    DrawNeonPanel(inputBox, CYAN);

    DrawText(
        aliasInput, 320, 318, 25, MY_WHITE
    );

    DrawText(
        TextFormat("%d/%d", aliasLength, MAX_ALIAS - 1), 750, 320, 18, MY_GRAY
    );

    Rectangle startButton =
    {
        400, 400, 300, 60
    };

    if (DrawButton(startButton, "START GAME", MY_GREEN))
    {
        if (aliasLength == 0)
        {
            strcpy(aliasInput, "Digital Rebel");
        }

        strcpy(player.alias, aliasInput);

        player.reputation = 0;
        player.skillLevel = 1;

        currentScreen = SCREEN_MENU;
    }

    DrawCenteredText(
        "Type your alias using the keyboard",
        500,
        18,
        MY_GRAY
    );
}
// ============================================================
// HANDLE ALIAS INPUT
// ============================================================
void HandleAliasInput()
{
    int key = GetCharPressed();

    while (key > 0)
    {
        if (key >= 32 &&
            key <= 125 &&
            aliasLength < MAX_ALIAS - 1)
        {
            aliasInput[aliasLength] =
                (char) key;

            aliasLength++;

            aliasInput[aliasLength] =
                '\0';
        }

        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) &&
        aliasLength > 0)
    {
        aliasLength--;

        aliasInput[aliasLength] =
            '\0';
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (aliasLength == 0)
        {
            strcpy(aliasInput, "Digital Rebel");
        }

        strcpy(player.alias, aliasInput);

        currentScreen = SCREEN_MENU;
    }
}
// ============================================================
// MAIN MENU
// ============================================================
void DrawMenu()
{
    DrawCyberBackground();

    DrawCenteredText(
        "CYBERPUNK CODEBREAKER", 70, 45, CYAN
    );

    DrawCenteredText(
        "MISSION CONTROL", 125, 22, MY_GREEN
    );

    DrawStats();

    Rectangle previewButton =
    { 350, 210, 400, 60 };

    Rectangle missionButton =
    { 350, 290, 400, 60 };

    Rectangle historyButton =
    { 350, 370, 400, 60 };

    Rectangle exitButton =
    { 350, 450, 400, 60 };

    if (DrawButton(
            previewButton,
            "PREVIEW MISSIONS",
            CYAN))
    {
        currentScreen =
            SCREEN_MISSIONS;
    }

    if (DrawButton(
            missionButton,
            "PLAY RANDOM MISSION",
            MY_GREEN))
    {
        StartRandomMission();
    }

    if (DrawButton(
            historyButton,
            "MISSION HISTORY",
            MY_YELLOW))
    {
        currentScreen =
            SCREEN_HISTORY;
    }

    if (DrawButton(
            exitButton,
            "EXIT GAME",
            MY_RED))
    {
        currentScreen =
            SCREEN_GAME_COMPLETE;
    }
}
// ============================================================
// MISSION LIST
// ============================================================
void DrawMissionList()
{
    DrawCyberBackground();

    DrawCenteredText(
        "MISSION DATABASE", 75, 38, CYAN
    );

    for (int i = 0; i < MISSION_COUNT; i++)
    {
        int column = i / 8;
        int row = i % 8;

        int x = 80 + column * 480;
        int y = 130 + row * 60;

        DrawNeonPanel((Rectangle){ x, y, 430, 45 }, (i % 2) ? CYAN : MY_GREEN);

        DrawText(
            TextFormat(
                "%02d. %s",
                i + 1,
                missions[i].title
            ),
            x + 15,
            y + 12,
            17,
            MY_WHITE
        );
    }

    Rectangle backButton =
    {
        400, 625, 300, 50
    };

    if (DrawButton(
            backButton,
            "BACK TO MENU",
            CYAN))
    {
        currentScreen =
            SCREEN_MENU;
    }
}
// ============================================================
// MISSION SCREEN
// ============================================================
void DrawMission()
{
    DrawCyberBackground();

    Mission *m =
        &missions[currentMission];

    DrawStats();

    DrawText(
        m->title, 50, 50, 32, CYAN
    );

    DrawText(
        m->description1, 50, 120, 21, MY_WHITE
    );

    DrawText(
        m->description2, 50, 155, 21, MY_WHITE
    );

    DrawText(
        "Choose your action:", 50, 215, 23, MY_YELLOW
    );

    for (int i = 0; i < 3; i++)
    {
        Rectangle button =
        {
            80,
            270 + i * 75,
            700,
            55
        };

        if (DrawButton(
                button,
                TextFormat(
                    "%d. %s",
                    i + 1,
                    m->choices[i]
                ),
                PANEL_LIGHT))
        {
            ResolveChoice(i);
        }

        DrawText(
            TextFormat(
                "Success Chance: %d%%",
                m->successChance[i]
            ),
            805,
            285 + i * 75,
            17,
            MY_GRAY
        );
    }

    Rectangle abortButton =
    {
        80, 510, 300, 50
    };

    if (DrawButton(
            abortButton,
            "ABORT MISSION",
            MY_RED))
    {
        currentScreen =
            SCREEN_MENU;
    }

    DrawText(
        "Choose carefully. Your reputation and skill depend on your decisions.",
        80,
        590,
        17,
        MY_GRAY
    );
}

// ============================================================
// RESULT SCREEN
// ============================================================

void DrawResult()
{
    DrawCyberBackground();

    // Outcome impact ring and radial sparks.
    if (resultEffectTime > 0.0f) {
        float age=0.85f-resultEffectTime;
        float radius=35.0f+age*250.0f;
        Color impact=missionSuccess ? MY_GREEN : MY_RED;
        DrawCircleLines(SCREEN_WIDTH/2,330,(int)radius,Fade(impact,resultEffectTime/0.85f));
        DrawCircleLines(SCREEN_WIDTH/2,330,(int)(radius*0.78f),Fade(CYAN,resultEffectTime/1.2f));
        for (int i=0; i<28; i++) {
            float angle=(float)i*6.2831853f/28.0f+age*1.6f;
            float distance=25.0f+age*(100.0f+(float)(i%5)*24.0f);
            Vector2 spark={SCREEN_WIDTH/2.0f+cosf(angle)*distance,330.0f+sinf(angle)*distance};
            DrawCircleV(spark,2.0f+(float)(i%3),Fade(i%3 ? impact : MY_YELLOW,resultEffectTime/0.85f));
        }
    }

    if (missionSuccess)
    {
        DrawCenteredText(
            "MISSION SUCCESS!",
            70,
            45,
            MY_GREEN
        );
    }
    else
    {
        DrawCenteredText(
            "MISSION FAILED!",
            70,
            45,
            MY_RED
        );
    }

    DrawCenteredText(
        TextFormat(
            "Random Roll: %d",
            missionRoll
        ),
        140,
        23,
        MY_WHITE
    );

    DrawNeonPanel((Rectangle){ 150, 200, 800, 120 }, missionSuccess ? MY_GREEN : MY_RED);

    DrawText(
        resultMessage, 180, 235, 23, MY_WHITE
    );

    DrawText(
        "CYBERSECURITY AWARENESS",
        150, 370, 24, CYAN
    );

    DrawText(
        awarenessMessage,
        150, 410, 20, MY_WHITE
    );

    DrawStats();

    Rectangle continueButton =
    {
        350, 540, 400, 60
    };

    if (DrawButton(
            continueButton,
            "RETURN TO MENU",
            MY_GREEN))
    {
        currentScreen =
            SCREEN_MENU;
    }
}

// ============================================================
// HISTORY SCREEN
// ============================================================

void DrawHistory()
{
    DrawCyberBackground();

    DrawCenteredText(
        "PLAYER LOG HISTORY",
        75,
        38,
        CYAN
    );

    if (historyCount == 0)
    {
        DrawCenteredText(
            "No missions taken yet.",
            250,
            25,
            MY_GRAY
        );
    }
    else
    {
        int visibleEntries = historyCount;

        if (visibleEntries > 8)
            visibleEntries = 8;

        for (int i = 0;
             i < visibleEntries;
             i++)
        {
            int y = 130 + i * 55;

            DrawNeonPanel((Rectangle){ 70, y, 960, 45 }, CYAN);

            DrawText(
                TextFormat(
                    "%d.",
                    i + 1
                ),
                85,
                y + 12,
                17,
                CYAN
            );

            DrawText(
                history[i].description,
                120,
                y + 12,
                17,
                MY_WHITE
            );

            DrawText(
                TextFormat(
                    "REP: %d",
                    history[i].reputation
                ),
                720,
                y + 12,
                16,
                MY_YELLOW
            );

            DrawText(
                TextFormat(
                    "SKILL: %d",
                    history[i].skillLevel
                ),
                850,
                y + 12,
                16,
                MY_GREEN
            );
        }
    }

    Rectangle backButton =
    {
        400, 625, 300, 50
    };

    if (DrawButton(
            backButton,
            "BACK TO MENU",
            CYAN))
    {
        currentScreen =
            SCREEN_MENU;
    }
}
// ============================================================
// GAME COMPLETE
// ============================================================
void DrawGameComplete()
{
    DrawCyberBackground();

    DrawCenteredText(
        "GAME COMPLETE",
        100, 48, CYAN
    );

    DrawCenteredText(
        TextFormat(
            "Final Alias: %s",
            player.alias
        ),
        210, 25, MY_WHITE
    );

    DrawCenteredText(
        TextFormat(
            "Final Reputation: %d",
            player.reputation
        ),
        270, 25, MY_YELLOW
    );

    DrawCenteredText(
        TextFormat(
            "Final Skill Level: %d",
            player.skillLevel
        ),
        330, 25, MY_GREEN
    );

    DrawCenteredText(
        "Thank you for playing", 430, 22, MY_GRAY
    );

    DrawCenteredText(
        "CYBERPUNK CODEBREAKER", 470, 30, CYAN
    );

    Rectangle exitButton =
    { 400, 550, 300, 60
    };

    if (DrawButton(
            exitButton,
            "CLOSE GAME",
            MY_RED))
    {
        gameRunning = false;
    }
}

// ============================================================
// MAIN
// ============================================================

static void DrawLoadingScreen(void)
{
    float progress=loadingTime/3.4f;
    if (progress>1.0f) progress=1.0f;
    float pulse=0.5f+0.5f*sinf(fxTime*3.5f);
    ClearBackground((Color){4,8,18,255});
    for (int col=0; col<25; col++) {
        int x=22+col*48;
        int y=(int)fmodf(fxTime*100.0f+col*37.0f,SCREEN_HEIGHT+120)-60;
        for (int row=0; row<6; row++) DrawText(((col+row)%2) ? "1":"0",x,y-row*23,15,Fade(CYAN,0.12f+row*0.025f));
    }
    Rectangle frame={115,92,970,610};
    DrawRectangleRounded(frame,0.025f,8,Fade(PANEL,0.95f));
    DrawRectangleRoundedLines(frame,0.025f,8,Fade(CYAN,0.55f+0.4f*pulse));
    DrawCenteredText("CYBERPUNK CODEBREAKER",132,40,CYAN);
    DrawCenteredText("SECURE BOOT // THREAT INTELLIGENCE SYSTEM",190,17,MY_GREEN);

    Vector2 c={SCREEN_WIDTH/2.0f,360.0f};
    DrawCircleLines((int)c.x,(int)c.y,104+5*pulse,Fade(CYAN,0.65f+0.3f*pulse));
    DrawCircleLines((int)c.x,(int)c.y,82,Fade(MY_GREEN,0.5f));
    DrawPoly(c,6,62,fxTime*20.0f,Fade(CYAN,0.18f));
    DrawPolyLines(c,6,62,-fxTime*20.0f,Fade(CYAN,0.85f));
    DrawRectangleRounded((Rectangle){c.x-32,c.y-8,64,51},0.16f,8,CYAN);
    DrawCircleLines((int)c.x,(int)c.y-29,20,CYAN);
    DrawRectangle((int)c.x-3,(int)c.y+8,6,18,PANEL);
    float scanX=c.x-91+fmodf(fxTime*145.0f,182.0f);
    DrawRectangle((int)scanX,250,3,220,Fade(MY_GREEN,0.75f));
    DrawText("AUTHENTICATING ENCRYPTION KEYS",270,520,18,MY_WHITE);
    DrawRectangle(270,555,660,24,(Color){18,32,48,255});
    DrawRectangle(270,555,(int)(660*progress),24,CYAN);
    DrawRectangleLines(270,555,660,24,Fade(CYAN,0.9f));
    DrawText(TextFormat("%02d%%",(int)(progress*100)),945,557,19,MY_WHITE);
    const char *status=progress<0.30f ? "CHECKING FIREWALL..." : progress<0.68f ? "SCANNING NETWORK..." : "SECURE CHANNEL READY...";
    DrawCenteredText(status,618,19,MY_GRAY);
}

static void DrawTransitionOverlay(void)
{
    if (transitionFlash<=0.0f) return;
    float alpha=transitionFlash/0.22f;
    DrawRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,Fade(CYAN,0.16f*alpha));
    int y=(int)((1.0f-alpha)*SCREEN_HEIGHT);
    DrawRectangle(0,y,SCREEN_WIDTH,2,Fade(MY_GREEN,alpha));
}

int main()
{
    srand((unsigned int)time(NULL));


    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Cyberpunk Codebreaker"
    );

    LoadGameSounds();
    InitEffects();
    SetTargetFPS(60);

    while (!WindowShouldClose() && gameRunning)
    {
        float dt=GetFrameTime();
        UpdateEffects(dt);
        if (currentScreen==SCREEN_LOADING) {
            loadingTime+=dt;
            if (loadingTime>=3.4f) currentScreen=SCREEN_ALIAS;
        }
        // ====================================================
        // UPDATE
        // ====================================================

        if (currentScreen == SCREEN_ALIAS)
        {
            HandleAliasInput();
        }

        // ====================================================
        // DRAW
        // ====================================================

        BeginDrawing();

        GameScreen screenBeforeDraw=currentScreen;
        switch (currentScreen)
        {
            case SCREEN_LOADING:
                DrawLoadingScreen();
                break;
            case SCREEN_ALIAS:
                DrawAliasScreen();
                break;

            case SCREEN_MENU:
                DrawMenu();
                break;

            case SCREEN_MISSIONS:
                DrawMissionList();
                break;

            case SCREEN_MISSION:
                DrawMission();
                break;

            case SCREEN_RESULT:
                DrawResult();
                break;

            case SCREEN_HISTORY:
                DrawHistory();
                break;

            case SCREEN_GAME_COMPLETE:
                DrawGameComplete();
                break;
        }

        if (currentScreen!=screenBeforeDraw && screenBeforeDraw!=SCREEN_LOADING) {
            PlayIfReady(gameSounds.transition);
            if (transitionFlash<0.01f) transitionFlash=0.16f;
        }
        DrawTransitionOverlay();
        EndDrawing();
    }

    UnloadGameSounds();
    CloseWindow();

    return 0;
}