#include <file/file_path.h>
#include <streams/file_stream.h>
#include <string/stdstring.h>
#include <vfs/vfs_implementation.h>

#include "quasi88.h"
#include "initval.h"
#include "pc88main.h"
#include "pc88sub.h"
#include "memory.h"
#include "font.h"
#include "emu.h"
#include "drive.h"
#include "event.h"
#include "keyboard.h"
#include "snddrv.h"
#include "screen.h"
#include "z80.h"
#include "intr.h"
#include "fdc.h"

#include "disks.h"

disk_t retro_disks[16];
static drive_swap_t retro_drive_swap;

void disk_display_message(retro_environment_t cb)
{
   if (retro_drive_swap.state == DRIVE_NONE)
      return;
   else
   {
      bool    drive_a = retro_drive_swap.state == DRIVE_A;
      int8_t *index = drive_a ? &retro_drive_swap.index_a : &retro_drive_swap.index_b;
      struct  retro_message rmsg;
      char   *str = (char*)calloc(2048, sizeof(char));

      if (*index == NO_DISK)
         snprintf(str, 2048, "Drive %c: %s", drive_a ? '1' : '2', "No disk");
      else
         snprintf(str, 2048, "Drive %c: %s", drive_a ? '1' : '2', retro_disks[*index].basename);

      rmsg.frames = 60 * 3;
      rmsg.msg = str;
      cb(RETRO_ENVIRONMENT_SET_MESSAGE, &rmsg);
   }
}

bool is_user_disk(const char *filename)
{
   return !(strncmp(path_get_extension(filename), "u88", 3) || 
      strncmp(path_get_extension(filename), "U88", 3));
}

/* Public */
void retro_disks_init()
{
   retro_drive_swap.count   = 0;
   retro_drive_swap.index_a = NO_DISK;
   retro_drive_swap.index_b = NO_DISK;
   retro_drive_swap.state   = DRIVE_NONE;
}

bool retro_disks_append(const char *new_filename)
{
   if (retro_drive_swap.count >= DISK_COUNT)
      return false;
   else
   {
      disk_t *new_disk = &retro_disks[retro_drive_swap.count];

      strncpy(new_disk->filename, new_filename, FILENAME_SIZE);
      strncpy(new_disk->basename, path_basename(new_filename), FILENAME_SIZE);
      new_disk->drive_index = DRIVE_NONE;
      new_disk->is_user_disk = is_user_disk(new_filename);
      retro_drive_swap.count++;

      return true;
   }
}

void retro_disks_start(retro_environment_t cb, bool drive_a)
{
   if (drive_a)
      retro_drive_swap.state = DRIVE_A;
   else
      retro_drive_swap.state = DRIVE_B;
   disk_display_message(cb);
}

void retro_disks_cycle(retro_environment_t cb, bool right)
{
   if (retro_drive_swap.state == DRIVE_NONE)
      return;
   else
   {
      int8_t  *index = retro_drive_swap.state == DRIVE_A ? 
          &retro_drive_swap.index_a : &retro_drive_swap.index_b;
      uint8_t  i;

      if (right)
      {
         *index += 1;
         if (*index >= retro_drive_swap.count)
            *index = NO_DISK;
      }
      else
      {
         *index -= 1;
         if (*index < 0)
            *index = NO_DISK;
      }

      retro_disks_set(retro_drive_swap.state - 1, *index); //remove
      disk_display_message(cb);
   }
}

void retro_disks_set(uint8_t drive, uint8_t index)
{
   quasi88_disk_insert(drive, retro_disks[index].filename, 0, 0);
}
