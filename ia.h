//
// IA stuff
//
void UIState_play_IA_noplay(void* u)
{
    return;
}

void UIState_play_IA_random(void* u)
{
    UIState* ui = (UIState*)u;

    delay(1000);
    uint8_t played = 0;
    while (!played)
    {
        uint8_t id = random(0, 9);
        if (ui->buttonsState[id] == 0)
        {
            played = 1;
            ui->buttonsState[id] = 2;
            ui->played = 1;
            ui->coupPlayed++;
            return;
        }
    }
}

// If a coup has been played, returns 1
int UIState_check_finish_line(void* u)
{
    UIState* ui = (UIState*)u;

    uint8_t inLine = 0;
    int8_t hline = -1;
    int8_t vline = -1;

    // Check whether we can finish a line
    inLine = 0;
    hline = -1;
    vline = -1;
    for (uint8_t i = 0; i < 3; ++i)
    {
        inLine = 0;
        for (uint8_t j = 0; j < 3; ++j)
            if (ui->buttonsState[i * 3 + j] == 2)
                inLine++;
            else if (ui->buttonsState[i * 3 + j] == 1)
                inLine = 3;

        if (inLine == 2)
        {
            hline = i;
            break;
        }

        inLine = 0;
        for (uint8_t j = 0; j < 3; ++j)
            if (ui->buttonsState[j * 3 + i] == 2 && ui->buttonsState[i * 3 + j] == 0)
                inLine++;
            else if (ui->buttonsState[j * 3 + i] == 1)
                inLine = 3;

        if (inLine == 2)
        {
            vline = i;
            break;
        }
    }

    if (hline >= 0)
    {
        for (uint8_t i = 0; i < 3; ++i)
            if (ui->buttonsState[hline * 3 + i] == 0)
            {
                ui->buttonsState[hline * 3 + i] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return 1;
            }
    }

    if (vline >= 0)
    {
        for (uint8_t i = 0; i < 3; ++i)
            if (ui->buttonsState[i * 3 + vline] == 0)
            {
                ui->buttonsState[i * 3 + vline] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return 1;
            }
    }

    // Check for diagonals
    inLine = 0;
    for (uint8_t i = 0; i < 3; ++i)
        if (ui->buttonsState[i * 3 + i] == 2)
            inLine++;
        else if (ui->buttonsState[i * 3 + i] == 1)
            inLine = 3;
    if (inLine == 2)
    {
        for (uint8_t i = 0; i < 3; ++i)
            if (ui->buttonsState[i * 3 + i] == 0)
            {
                ui->buttonsState[i * 3 + i] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return 1;
            }
    }

    inLine = 0;
    for (uint8_t i = 0; i < 3; ++i)
        if (ui->buttonsState[i * 3 + (2 - i)] == 2)
            inLine++;
        else if (ui->buttonsState[i * 3 + (2 - i)] == 1)
            inLine = 3;
    if (inLine == 2)
    {
        for (uint8_t i = 0; i < 3; ++i)
            if (ui->buttonsState[i * 3 + (2 - i)] == 0)
            {
                ui->buttonsState[i * 3 + (2 - i)] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return 1;
            }
    }

    // Check whether we can prevent a line from being finished
    for (uint8_t i = 0; i < 3; ++i)
    {
        inLine = 0;
        for (uint8_t j = 0; j < 3; ++j)
            if (ui->buttonsState[i * 3 + j] == 1)
                inLine++;
            else if (ui->buttonsState[i * 3 + j] == 2)
                inLine = 3;

        if (inLine == 2)
        {
            hline = i;
            break;
        }

        inLine = 0;
        for (uint8_t j = 0; j < 3; ++j)
            if (ui->buttonsState[j * 3 + i] == 1)
                inLine++;
            else if (ui->buttonsState[j * 3 + i] == 2)
                inLine = 3;

        if (inLine == 2)
        {
            vline = i;
            break;
        }
    }

    if (hline >= 0)
    {
        for (uint8_t i = 0; i < 3; ++i)
            if (ui->buttonsState[hline * 3 + i] == 0)
            {
                ui->buttonsState[hline * 3 + i] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return 1;
            }
    }

    if (vline >= 0)
    {
        for (uint8_t i = 0; i < 3; ++i)
            if (ui->buttonsState[i * 3 + vline] == 0)
            {
                ui->buttonsState[i * 3 + vline] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return 1;
            }
    }

    // Check for diagonals
    inLine = 0;
    for (uint8_t i = 0; i < 3; ++i)
        if (ui->buttonsState[i * 3 + i] == 1)
            inLine++;
        else if (ui->buttonsState[i * 3 + i] == 2)
            inLine = 3;
    if (inLine == 2)
    {
        for (uint8_t i = 0; i < 3; ++i)
            if (ui->buttonsState[i * 3 + i] == 0)
            {
                ui->buttonsState[i * 3 + i] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return 1;
            }
    }

    inLine = 0;
    for (uint8_t i = 0; i < 3; ++i)
        if (ui->buttonsState[i * 3 + (2 - i)] == 1)
            inLine++;
        else if (ui->buttonsState[i * 3 + (2 - i)] == 2)
            inLine = 3;
    if (inLine == 2)
    {
        for (uint8_t i = 0; i < 3; ++i)
            if (ui->buttonsState[i * 3 + (2 - i)] == 0)
            {
                ui->buttonsState[i * 3 + (2 - i)] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return 1;
            }
    }

    return 0;
}

void UIState_play_IA_semi_random(void* u)
{
    delay(1000);

    UIState* ui = (UIState*)u;

    // Check whether we can prevent or finish a line
    if (UIState_check_finish_line(u))
        return;

    // Otherwise, play random
    uint8_t played = 0;
    while (!played)
    {
        uint8_t id = random(0, 9);
        if (ui->buttonsState[id] == 0)
        {
            played = 1;
            ui->buttonsState[id] = 2;
            ui->played = 1;
            ui->coupPlayed++;
            return;
        }
    }
}

void UIState_play_IA_evil(void* u)
{
    delay(1000);

    UIState* ui = (UIState*)u;

    // Check whether we can prevent or finish a line
    if (UIState_check_finish_line(u))
        return;

    // Play evil
    // Take the middle
    if (ui->buttonsState[4] == 0)
    {
        ui->buttonsState[4] = 2;
        ui->played = 1;
        ui->coupPlayed++;
        return;
    }

    // TODO: if opposite corner are taken, should play to make a non-diagonal line
    // Check whether opposite corners are played
    bool oppositesPlayed = false;
    if (ui->buttonsState[0] == 1 && ui->buttonsState[8] || ui->buttonsState[2] == 1 && ui->buttonsState[6])
        oppositesPlayed = true;

    // Take the corners
    if (oppositesPlayed)
    {
        int corners[4] = {1, 3, 5, 7};
        for (int i = 0; i < 4; ++i)
        {
            if (ui->buttonsState[corners[i]] == 0)
            {
                ui->buttonsState[corners[i]] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return;
            }
        }
    }
    else
    {
        int corners[4] = {0, 2, 6, 8};
        for (int i = 0; i < 4; ++i)
        {
            if (ui->buttonsState[corners[i]] == 0)
            {
                ui->buttonsState[corners[i]] = 2;
                ui->played = 1;
                ui->coupPlayed++;
                return;
            }
        }
    }

    // Random!
    uint8_t played = 0;
    while (!played)
    {
        uint8_t id = random(0, 9);
        if (ui->buttonsState[id] == 0)
        {
            played = 1;
            ui->buttonsState[id] = 2;
            ui->played = 1;
            ui->coupPlayed++;
            return;
        }
    }
}
