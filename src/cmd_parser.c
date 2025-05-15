#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>
#include "stm32f4xx_hal.h" // for UNUSED()
#include "cmd_line_buffer.h"
#include "cmd_parser.h"
#include "IMU.h"

// Type for each command table entry
typedef struct
{
    void (*func)(int argc, char *argv[]);   // Command function pointer
    const char * cmd;                       // Command name
    const char * args;                      // Command arguments syntax
    const char * help;                      // Command description
} CMD_T;

// Forward declaration for built-in help command
static void _help(int, char *[]);

// Modules that provide commands
#include "heartbeat_cmd.h"
#include "controller_cmd.h"

// Motor-test integration
#include "motor.h"
#include "data_logging.h"

// Forward declarations for new commands
static void _cmd_setMotorVoltage(int argc, char *argv[]);

static void _cmd_IMULoggingStart(int argc, char *argv[]);


// Command table
static CMD_T cmd_table[] = {
    { _help               , "help"     , "" , "Displays this help message" },
    { heartbeat_cmd       , "heartbeat", "[start|stop]", "Get status or start/stop heartbeat task" },
    { controller_cmd      , "ctrl"     , "[init]|[get u1|u2|u3]|[set x1..x8 <value>]", "Get or set control outputs/states" },
    {_cmd_IMULoggingStart           , "startIMULog"                 , ""                        , "Start IMU Data Logging"},
    {_cmd_setMotorVoltage           , "setVoltage"                  , "[mtr1 V | mtr2 V | mtr3]"       , "Sets Motor Voltage (-3.3 < V < 3.3)"},
};
enum {CMD_TABLE_SIZE = sizeof(cmd_table)/sizeof(CMD_T)};
enum {CMD_MAX_TOKENS = 6};

// Help command implementation
static void _print_chip_pinout(void);

void _help(int argc, char *argv[])
{
    UNUSED(argc); UNUSED(argv);
    printf("\nPin configuration and commands:\n\n");
    _print_chip_pinout();
    printf("\n   Command      Arguments    Description\n");
    printf("-------------------------------------------\n");
    for (int i = 0; i < CMD_TABLE_SIZE; i++) {
        printf("%12s %-12s %s\n", cmd_table[i].cmd, cmd_table[i].args, cmd_table[i].help);
    }
}

// Display Nucleo pinout (omitted for brevity)
static void _print_chip_pinout(void) { /* ... */ }

// Command parser & dispatcher
static int _makeargv(char *s, char *argv[], int argvsize);

void cmd_parse(char *cmd)
{
    if (!cmd || !*cmd) return;
    char *argv[CMD_MAX_TOKENS];
    int argc = _makeargv(cmd, argv, CMD_MAX_TOKENS);
    for (int i = 0; i < CMD_TABLE_SIZE; i++) {
        if (strcmp(argv[0], cmd_table[i].cmd) == 0) {
            cmd_table[i].func(argc, argv);
            return;
        }
    }
    printf("Unknown command: '%s'\r\n", argv[0]);
}

int _makeargv(char *s, char *argv[], int argvsize)
{
    char *p = s;
    int argc = 0;
    for (int i = 0; i < argvsize; ++i) {
        while (isspace(*p)) p++;
        if (*p) argv[argc++] = p;
        else break;
        while (*p && !isspace(*p)) p++;
        if (*p) *p++ = '\0';
    }
    return argc;
}


void _cmd_IMULoggingStart(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    imu_logging_start();
}

void _cmd_setMotorVoltage(int argc, char *argv[])
{
    // Ensure correct number of arguments
    if (argc != 4)
    {
        printf("Usage: setVoltage <voltage_motor1> <voltage_motor2> <voltage_motor3>\n");

        return;
    }

    // Parse voltage argument
    float voltage_motor1 = atof(argv[1]);
    float voltage_motor2 = atof(argv[2]);
    float voltage_motor3 = atof(argv[3]);

    // Set the motor Voltage
    motor_set_voltage(voltage_motor1, voltage_motor2, voltage_motor3);

    // Print confirmation
    printf("\nMotor 1 voltage set to: %f\nMotor 2 voltage set to: %f\nMotor 3 voltage set to: %f\n", voltage_motor1, voltage_motor2, voltage_motor3);
}
