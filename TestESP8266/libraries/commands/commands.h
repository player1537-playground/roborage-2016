#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct command {
  uint8_t type;
  uint8_t payload[6];
  uint8_t checksum;
};

struct motor_command {
  uint8_t type;
  uint8_t left_speed;
  uint8_t left_mode;
  uint8_t right_speed;
  uint8_t right_mode;
  uint8_t zero[2];
  uint8_t checksum;
};

/** Initializes a command.
 *
 * @param[out] command The command to be initialized.
 * @returns 0 if successful. Anything else is an error.
 */
int command_init(struct command *command);

/** Finalizes a command by setting its checksum byte.
 *
 * @param[in,out] command The command to finalize.
 * @returns 0 if successful else error.
 */
int command_finish(struct command *command);

/** Checks if a given command is valid according to its checksum.
 *
 * @param[in] command The command to be checked.
 * @returns 1 if command is valid else 0.
 */
int command_is_valid(struct command *command);



#ifdef __cplusplus
}
#endif

#endif
