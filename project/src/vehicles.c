#include "vehicles.h"
#include "car.h"
#include "taxi_pixmaps.h"
#include "police_pixmaps.h"
#include "ambulance_pixmaps.h"

static const vehicle_def_t vehicles[NUM_VEHICLES] = {
    { "TAXI",     (xpm_map_t *) taxi_xpms,     TAXI_XPM_COUNT },
    { "POLICE",   (xpm_map_t *) police_xpms,   POLICE_XPM_COUNT },
    { "AMBULANCE",(xpm_map_t *) ambulance_xpms, AMBULANCE_XPM_COUNT },
};

const vehicle_def_t *vehicle_get(int index) {
    if (index < 0 || index >= NUM_VEHICLES)
        index = 0;
    return &vehicles[index];
}

xpm_map_t vehicle_preview_xpm(int index) {
    const vehicle_def_t *v = vehicle_get(index);
    int sprite_idx = 0; 
    return v->xpms[sprite_idx];
}
