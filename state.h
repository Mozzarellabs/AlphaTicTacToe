//
// State
//
typedef struct
{
    uint8_t iaLevel;
    uint8_t played;
    uint8_t won;
    uint8_t coupPlayed;
    uint8_t buttonsState[9]; // 1 if pressed during the current loop, 2 for released
    uint8_t buttonsPrev[9];  // Value at the previous loop
    uint32_t color[9];       // Desired color
    void (*IA_func)(void*);  // IA function
} UIState;
