#ifndef _VEHICLES_H_
#define _VEHICLES_H_

#include <lcom/lcf.h>

#define NUM_VEHICLES 3

typedef struct {
    const char *label;
    xpm_map_t *xpms;
    int num_sprites;
} vehicle_def_t;

/**
 * @brief Retrieves the vehicle definition for a given index.
 * @param index The index of the vehicle (0 to NUM_VEHICLES-1).
 * @return Pointer to the vehicle definition, or the first vehicle if index is out of range.
 */
const vehicle_def_t *vehicle_get(int index);

/** North-facing sprite for menu preview / cursor. */
xpm_map_t vehicle_preview_xpm(int index);

#endif /* _VEHICLES_H_ */
