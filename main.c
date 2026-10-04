#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//gcc main.c -o main.exe -IC:\raylib\raylib\src -LC:\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm
//.\main.exe

// ============================================================
// SETTINGS
// ============================================================

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define MAX_HISTORY 100
#define MAX_ALIAS 50
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
    SCREEN_ALIAS,
    SCREEN_MENU,
    SCREEN_MISSIONS,
    SCREEN_MISSION,
    SCREEN_RESULT,
    SCREEN_HISTORY,
    SCREEN_GAME_COMPLETE

} GameScreen;

GameScreen currentScreen = SCREEN_ALIAS;

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

// Shared neon grid used by every screen.
void DrawCyberBackground(void)
{
    ClearBackground(BACKGROUND);
    for (int x = 0; x < SCREEN_WIDTH; x += 40) DrawLine(x, 54, x, SCREEN_HEIGHT, Fade(CYAN, 0.10f));
    for (int y = 94; y < SCREEN_HEIGHT; y += 40) DrawLine(0, y, SCREEN_WIDTH, y, Fade(CYAN, 0.08f));
    DrawRectangle(0, 0, SCREEN_WIDTH, 54, Fade(PANEL, 0.98f));
    DrawLine(0, 54, SCREEN_WIDTH, 54, CYAN);
    DrawText("CYBERNET // SECURE OPERATIONS", 24, 18, 14, CYAN);
    DrawText("NETWORK: ENCRYPTED", SCREEN_WIDTH - 230, 18, 14, MY_GREEN);
}

void DrawNeonPanel(Rectangle rect, Color accent)
{
    DrawRectangleRounded(rect, 0.08f, 8, Fade(PANEL, 0.96f));
    DrawRectangleRoundedLines(rect, 0.08f, 8, accent);
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
    bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);
    DrawRectangleRounded(rect, 0.08f, 8, Fade(hovered ? accent : PANEL_LIGHT, hovered ? 0.55f : 0.92f));
    DrawRectangleRoundedLines(rect, 0.08f, 8, accent);
    DrawRectangle((int)rect.x, (int)rect.y, 4, (int)rect.height, accent);
    int size = 20, width = MeasureText(text, size);
    DrawText(text, (int)(rect.x + (rect.width - width) / 2), (int)(rect.y + (rect.height - size) / 2), size, MY_WHITE);
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
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

int main()
{
    srand((unsigned int)time(NULL));


    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Cyberpunk Codebreaker"
    );

    SetTargetFPS(60);

    while (!WindowShouldClose() && gameRunning)
    {
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

        switch (currentScreen)
        {
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

        EndDrawing();
    }

    CloseWindow();

    return 0;
}