#include "commands.h"

int command_init(struct command *command) {
  /* Currently a no-op */
  (void)command;
  return 0;
}

int command_finish(struct command *command) {
  int i;
  uint8_t checksum;

  checksum = 0;
  for (i=0; i<sizeof(command->payload); ++i) {
    checksum += command->payload[i];
  }

  command->checksum = checksum;

  return 0;
}

int command_is_valid(struct command *command) {
  int i;
  uint8_t checksum;

  checksum = 0;
  for (i=0; i<sizeof(command->payload); ++i) {
    checksum += command->payload[i];
  }

  return checksum == command->checksum;
}
