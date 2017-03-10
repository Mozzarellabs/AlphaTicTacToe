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

void UIState_play_IA_semi_random(void* u)
{
    delay(1000);

    UIState* ui = (UIState*)u;

    uint8_t inLine = 0;
    int8_t hline = -1;
    int8_t vline = -1;
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
            if (ui->buttonsState[j * 3 + i] == 1 && ui->buttonsState[i * 3 + j] == 0)
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
                return;
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
                return;
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
                return;
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
                return;
            }
    }

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
                return;
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
                return;
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
                return;
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
                return;
            }
    }

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
    UIState* ui = (UIState*)u;

    if (ui->coupPlayed == 1)
    {
        if (ui->buttonsState[4] == 1)
        {
            switch (random(0, 3))
            {
            case 0:
                ui->buttonsState[0] = 2;
                break;
            case 1:
                ui->buttonsState[2] = 2;
                break;
            case 2:
                ui->buttonsState[6] = 2;
                break;
            case 3:
                ui->buttonsState[8] = 2;
                break;
            }
            ui->played = 1;
            ui->coupPlayed++;
            return;
        }
        else
        {
        }
    }
}
