#include <err.h>
#include <fcntl.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/audioio.h>

char*
mixer_devinfo_type2str(int t)
{
   switch (t) {
   case AUDIO_MIXER_CLASS:
      return "class";
   case AUDIO_MIXER_ENUM:
      return "enum";
   case AUDIO_MIXER_SET:
      return "set";
   case AUDIO_MIXER_VALUE:
      return "value";
   default:
      return "?????";
   }
}

void
print_devinfo(mixer_devinfo_t *d)
{
   printf("%3d: type = %-7s label.name = %-15s mixer_class = %2d next = %2d prev = %2d\n",
         d->index,
         mixer_devinfo_type2str(d->type),
         d->label.name,
         d->mixer_class,
         d->next,
         d->prev);
}

void
print_mute(int fd, int idx)
{
   mixer_ctrl_t vinfo;
   vinfo.dev  = idx;
   vinfo.type = AUDIO_MIXER_ENUM;

   if (-1 == ioctl(fd, AUDIO_MIXER_READ, &vinfo))
      printf("failed to get mute :(\n");
   else
      printf("\tord = %d\n", vinfo.un.ord);
}

void
print_volume(int fd, int idx)
{
   mixer_ctrl_t vinfo;
   vinfo.dev  = idx;
   vinfo.type = AUDIO_MIXER_VALUE;

   if (-1 == ioctl(fd, AUDIO_MIXER_READ, &vinfo))
      printf("failed to get volume :(\n");
   else {
      printf("\tnum_channels = %d mono = %3d left = %3d right = %3d\n",
            vinfo.un.value.num_channels,
            vinfo.un.value.level[AUDIO_MIXER_LEVEL_MONO],
            vinfo.un.value.level[AUDIO_MIXER_LEVEL_LEFT],
            vinfo.un.value.level[AUDIO_MIXER_LEVEL_RIGHT]);
   }
}

/* test driver */
int
main()
{
   char *device;
   int fd;

   int outputs_class = -1;
   int outputs_master_idx = -1;
   int outputs_mute_idx = -1;

   /* determine mixer device & open it */
   device = getenv("MIXERDEVICE");
   if (NULL == device)
      device = "/dev/mixer";

   if (0 >= (fd = open(device, O_RDWR)))
      err(1, "failed to open %s", device);

   /* iterate through devices configured */
   mixer_devinfo_t devinfo;
   devinfo.index = 0;
   while (-1 != ioctl(fd, AUDIO_MIXER_DEVINFO, &devinfo)) {
      print_devinfo(&devinfo);

      if (0 == strncmp(devinfo.label.name, AudioCoutputs, MAX_AUDIO_DEV_LEN)) {
         outputs_class = devinfo.mixer_class;
         printf("have outputs: output_class -> %d\n", outputs_class);
      }

      if (0 == strncmp(devinfo.label.name, AudioNmaster, MAX_AUDIO_DEV_LEN)
      && devinfo.mixer_class == outputs_class) {
         outputs_master_idx = devinfo.index;
         printf("have outputs.master: output_master_idx -> %d\n", outputs_master_idx);
      }

      if (0 == strncmp(devinfo.label.name, AudioNmute, MAX_AUDIO_DEV_LEN)
      && devinfo.mixer_class == outputs_class) {
         outputs_mute_idx = devinfo.index;
         printf("have outputs.mute: output_mute_idx -> %d\n", outputs_mute_idx);
      }

      devinfo.index++;
   }

   printf("\noutputs.master volume & mute:\n");
   print_volume(fd, outputs_master_idx);
   print_mute(fd, outputs_mute_idx);

   /* cleanup */
   close(fd);
}
