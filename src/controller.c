#include <stddef.h>
#include "controller.h"
#include "stm32f4xx_hal.h"

// Number of states and outputs (3 inputs)
#ifndef CTRL_N_STATE
#define CTRL_N_STATE 8
#endif
#ifndef CTRL_N_OUTPUT
#define CTRL_N_OUTPUT 3
#endif

static float _ctrl_state[CTRL_N_STATE];
static float _ctrl_output[CTRL_N_OUTPUT];

/* Continuous LQR gain matrix K (3 inputs × 8 states) */
static const float K[CTRL_N_OUTPUT][CTRL_N_STATE] = {
    // Voltage Input 1 gains
    { -51.8201908227429f,   -1.33182756258797e-12f,  -0.462187313823241f,  -255.716873180975f,
      -6.54155240311749e-12f, -1.03465534096428f,    0.827311209723870f,   0.827311209723943f },
    // Voltage Input 2 gains
    {  11.4184051161131f,   -33.6937899639430f,      -0.676222284007653f,   56.3761862705296f,
     -166.817289569588f,      0.475226354217808f,    -0.287956657129318f,   0.722093341620820f },
    // Voltage Input 3 gains
    {  11.4184051161135f,    33.6937899639438f,      -0.676222284007181f,   56.3761862705315f,
      166.817289569592f,      0.475226354217895f,     0.722093341620857f,  -0.287956657129229f }
};

void ctrl_init(void)
{
    for (size_t i = 0; i < CTRL_N_STATE; ++i) {
        _ctrl_state[i] = 0.0f;
    }
}
    
void ctrl_set_state(const float state[CTRL_N_STATE])
{
    for (size_t i = 0; i < CTRL_N_STATE; ++i) {
        _ctrl_state[i] = state[i];
    }
}

void ctrl_set_x1(const float x1) { _ctrl_state[0] = x1; }
void ctrl_set_x2(const float x2) { _ctrl_state[1] = x2; }
void ctrl_set_x3(const float x3) { _ctrl_state[2] = x3; }
void ctrl_set_x4(const float x4) { _ctrl_state[3] = x4; }
void ctrl_set_x5(const float x5) { _ctrl_state[4] = x5; }
void ctrl_set_x6(const float x6) { _ctrl_state[5] = x6; }
void ctrl_set_x7(const float x7) { _ctrl_state[6] = x7; }
void ctrl_set_x8(const float x8) { _ctrl_state[7] = x8; }

float * ctrl_get_state(void)
{
    return _ctrl_state;
}

float * ctrl_run(void)
{
    // Manually expanded control outputs u = -K * x
    _ctrl_output[0] = -(
        K[0][0]*_ctrl_state[0] + K[0][1]*_ctrl_state[1] + K[0][2]*_ctrl_state[2] + K[0][3]*_ctrl_state[3] +
        K[0][4]*_ctrl_state[4] + K[0][5]*_ctrl_state[5] + K[0][6]*_ctrl_state[6] + K[0][7]*_ctrl_state[7]
    );

    _ctrl_output[1] = -(
        K[1][0]*_ctrl_state[0] + K[1][1]*_ctrl_state[1] + K[1][2]*_ctrl_state[2] + K[1][3]*_ctrl_state[3] +
        K[1][4]*_ctrl_state[4] + K[1][5]*_ctrl_state[5] + K[1][6]*_ctrl_state[6] + K[1][7]*_ctrl_state[7]
    );

    _ctrl_output[2] = -(
        K[2][0]*_ctrl_state[0] + K[2][1]*_ctrl_state[1] + K[2][2]*_ctrl_state[2] + K[2][3]*_ctrl_state[3] +
        K[2][4]*_ctrl_state[4] + K[2][5]*_ctrl_state[5] + K[2][6]*_ctrl_state[6] + K[2][7]*_ctrl_state[7]
    );

    return _ctrl_output;
}
