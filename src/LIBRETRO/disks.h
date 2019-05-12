#include <stdint.h>
#include <libretro.h>

#define DISK_COUNT    16
#define DRIVE_NONE    0
#define DRIVE_A       1
#define DRIVE_B       2
#define FILENAME_SIZE 1024
#define NO_DISK       -1

typedef struct disk_t
{
   char    basename[FILENAME_SIZE];
   char    filename[FILENAME_SIZE];

   uint8_t drive_index;  /* Which drive is this disk inserted into?    */
   bool    is_user_disk; /* Is this an arbitrary data disk for saving? */
} disk_t;

typedef struct drive_swap_t
{
   /* Is the user swapping drive A, B, or nothing? */
   uint8_t state;

   /* Which disk is currently loaded? */
   int8_t  index_a;
   int8_t  index_b;

   /* How many disks exist in the buffer? */
   uint8_t count;
} drive_swap_t;

void retro_disks_init  ();
bool retro_disks_append(const char *new_filename);
void retro_disks_cycle (retro_environment_t cb, bool right);
void retro_disks_set   (uint8_t drive, uint8_t index);
void retro_disks_start (retro_environment_t cb, bool drive_a);

extern disk_t retro_disks[16];
