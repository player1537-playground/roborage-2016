#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

int main(int argc, char **argv) {
  int i, n, fd, num_axes, num_buttons;
  int *axes, *buttons;
  struct js_event js;

  fd = open("/dev/input/js0", O_RDONLY);
  if (fd < 0) {
    perror("open()");
    exit(EXIT_FAILURE);
  }

  ioctl(fd, JSIOCGAXES, &num_axes);
  ioctl(fd, JSIOCGBUTTONS, &num_buttons);

  axes = malloc(num_axes * sizeof(*axes));
  for (i=0; i<num_axes; ++i) {
    axes[i] = 0;
  }

  buttons = malloc(num_buttons * sizeof(*axes));
  for (i=0; i<num_buttons; ++i) {
    buttons[i] = 0;
  }

  fcntl(fd, F_SETFL, O_NONBLOCK);

  while (1) {
    n = read(fd, &js, sizeof(js));

    if (n == 0) {
      continue;
    }

    switch (js.type & ~JS_EVENT_INIT) {
    case JS_EVENT_AXIS:
      axes[js.number] = js.value;
      break;
    case JS_EVENT_BUTTON:
      buttons[js.number] = js.value;
      break;
    }

    for (i=0; i<num_axes; ++i) {
      printf("A%d: %6d  ", i, axes[i]);
    }

    for(i=0 ; i<num_buttons; ++i) {
      if (buttons[i]) {
        printf("B%d  ", i);
      }
    }

    printf("  \n");
    fflush(stdout);
  }

  close(fd);
  exit(EXIT_SUCCESS);
}
